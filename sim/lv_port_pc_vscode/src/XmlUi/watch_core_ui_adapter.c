#include "XmlUi/watch_core_ui_adapter.h"

#include <stdint.h>
#include <string.h>

#include "magic_watch_ui.h"
#include "screens/screen_health_shortcuts_gen.h"

static const char * const health_card_ids[WATCH_CORE_HEALTH_CARD_COUNT] = {
    "heart",
    "spo2",
    "breathe",
    "stress",
};

static const WatchCoreHealthFeature health_card_features[WATCH_CORE_HEALTH_CARD_COUNT] = {
    WATCH_CORE_HEALTH_FEATURE_HEART_RATE,
    WATCH_CORE_HEALTH_FEATURE_SPO2,
    WATCH_CORE_HEALTH_FEATURE_BREATHE,
    WATCH_CORE_HEALTH_FEATURE_STRESS,
};

static void health_card_handler(const char * card_id, void * user_data);
static void back_button_event_cb(lv_event_t * event);
static void bind_health_shortcut_screen(WatchCoreUiAdapter * adapter, lv_obj_t * screen);
static void create_health_card_hit_targets(lv_obj_t * screen);
static void apply_snapshot_to_subjects(WatchCoreUiAdapter * adapter);
static void dispatch_event(WatchCoreUiAdapter * adapter, WatchCoreUiEvent event);
static void drain_core_events(WatchCoreUiAdapter * adapter);
static void handle_page_intent(WatchCoreUiAdapter * adapter, WatchCorePageIntent intent);
static void load_health_detail(WatchCoreUiAdapter * adapter, WatchCoreHealthFeature feature);
static void load_screen(WatchCoreUiAdapter * adapter, lv_obj_t * screen);
static WatchCoreHealthFeature feature_from_card_id(const char * card_id);
static void style_screen_base(lv_obj_t * screen);

bool watch_core_ui_adapter_init(WatchCoreUiAdapter * adapter, WatchCore * core)
{
    if (adapter == NULL || core == NULL) {
        return false;
    }

    memset(adapter, 0, sizeof(*adapter));
    adapter->core = core;

    WatchCoreUiModelSnapshot snapshot;
    watch_core_get_ui_snapshot(core, &snapshot);

    for (uint32_t i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_subject_init_string(
            &adapter->health_metric_subjects[i],
            adapter->health_metric_subject_buffers[i],
            adapter->health_metric_subject_previous_buffers[i],
            WATCH_CORE_METRIC_TEXT_MAX,
            snapshot.health_metric_text[i]);
    }

    adapter->subjects_initialized = true;
    magic_watch_ui_set_health_card_event_handler(health_card_handler, adapter);

    return watch_core_ui_adapter_load_health_shortcuts(adapter);
}

bool watch_core_ui_adapter_load_health_shortcuts(WatchCoreUiAdapter * adapter)
{
    if (adapter == NULL || adapter->core == NULL || !adapter->subjects_initialized) {
        return false;
    }

    lv_obj_t * screen = screen_health_shortcuts_create();
    if (screen == NULL) {
        return false;
    }

    bind_health_shortcut_screen(adapter, screen);
    apply_snapshot_to_subjects(adapter);
    load_screen(adapter, screen);
    return true;
}

static void health_card_handler(const char * card_id, void * user_data)
{
    WatchCoreUiAdapter * adapter = (WatchCoreUiAdapter *)user_data;
    if (adapter == NULL) {
        return;
    }

    dispatch_event(adapter, watch_core_make_health_card_clicked_event(feature_from_card_id(card_id)));
}

static void back_button_event_cb(lv_event_t * event)
{
    WatchCoreUiAdapter * adapter = (WatchCoreUiAdapter *)lv_event_get_user_data(event);
    if (adapter == NULL) {
        return;
    }

    dispatch_event(adapter, watch_core_make_back_event());
}

static void bind_health_shortcut_screen(WatchCoreUiAdapter * adapter, lv_obj_t * screen)
{
    for (uint32_t i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_obj_t * card = magic_watch_ui_get_health_card(screen, i);
        lv_obj_t * metric_label = magic_watch_ui_get_health_card_metric_label(card);

        if (metric_label != NULL) {
            lv_label_bind_text(metric_label, &adapter->health_metric_subjects[i], NULL);
        }
    }

    create_health_card_hit_targets(screen);
}

static void create_health_card_hit_targets(lv_obj_t * screen)
{
    const int32_t x[WATCH_CORE_HEALTH_CARD_COUNT] = {
        HEALTH_GRID_LEFT,
        HEALTH_COL2_X,
        HEALTH_GRID_LEFT,
        HEALTH_COL2_X,
    };
    const int32_t y[WATCH_CORE_HEALTH_CARD_COUNT] = {
        HEALTH_GRID_TOP,
        HEALTH_GRID_TOP,
        HEALTH_ROW2_Y,
        HEALTH_ROW2_Y,
    };

    for (uint32_t i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_obj_t * hit = lv_obj_create(screen);
        if (hit == NULL) {
            continue;
        }

        lv_obj_remove_style_all(hit);
        lv_obj_set_x(hit, x[i]);
        lv_obj_set_y(hit, y[i]);
        lv_obj_set_width(hit, HEALTH_CARD_W);
        lv_obj_set_height(hit, HEALTH_CARD_H);
        lv_obj_set_ext_click_area(hit, 6);
        lv_obj_add_flag(hit, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_flag(hit, LV_OBJ_FLAG_SCROLLABLE, false);
        lv_obj_add_event_cb(hit, magic_watch_health_card_clicked, LV_EVENT_CLICKED, (void *)health_card_ids[i]);
    }
}

static void apply_snapshot_to_subjects(WatchCoreUiAdapter * adapter)
{
    WatchCoreUiModelSnapshot snapshot;
    watch_core_get_ui_snapshot(adapter->core, &snapshot);

    for (uint32_t i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_subject_copy_string(&adapter->health_metric_subjects[i], snapshot.health_metric_text[i]);
    }
}

static void dispatch_event(WatchCoreUiAdapter * adapter, WatchCoreUiEvent event)
{
    LV_LOG_USER("UiEvent %s feature=%s",
                watch_core_ui_event_name(event.type),
                watch_core_health_feature_name(event.feature));

    if (!watch_core_push_event(adapter->core, event)) {
        LV_LOG_WARN("watch_core event queue is full");
        return;
    }

    drain_core_events(adapter);
}

static void drain_core_events(WatchCoreUiAdapter * adapter)
{
    while (true) {
        WatchCorePageIntent intent = watch_core_process_next_event(adapter->core);
        if (intent.type == WATCH_CORE_PAGE_INTENT_NONE) {
            break;
        }
        handle_page_intent(adapter, intent);
    }
}

static void handle_page_intent(WatchCoreUiAdapter * adapter, WatchCorePageIntent intent)
{
    LV_LOG_USER("PageIntent %s feature=%s",
                watch_core_page_intent_name(intent.type),
                watch_core_health_feature_name(intent.feature));

    switch (intent.type) {
        case WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS:
            (void)watch_core_ui_adapter_load_health_shortcuts(adapter);
            break;
        case WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL:
            load_health_detail(adapter, intent.feature);
            break;
        case WATCH_CORE_PAGE_INTENT_NONE:
        default:
            break;
    }
}

static void load_health_detail(WatchCoreUiAdapter * adapter, WatchCoreHealthFeature feature)
{
    lv_obj_t * screen = lv_obj_create(NULL);
    if (screen == NULL) {
        return;
    }

    style_screen_base(screen);

    lv_obj_t * title = lv_label_create(screen);
    lv_label_set_text(title, "Health Detail");
    lv_obj_set_x(title, 20);
    lv_obj_set_y(title, 34);
    lv_obj_set_width(title, 200);
    lv_obj_set_style_text_color(title, FG_PRIMARY, 0);

    lv_obj_t * feature_label = lv_label_create(screen);
    lv_label_set_text(feature_label, watch_core_health_feature_name(feature));
    lv_obj_set_x(feature_label, 20);
    lv_obj_set_y(feature_label, 82);
    lv_obj_set_width(feature_label, 200);
    lv_obj_set_style_text_color(feature_label, FG_PRIMARY, 0);

    lv_obj_t * hint = lv_label_create(screen);
    lv_label_set_text(hint, "Placeholder page");
    lv_obj_set_x(hint, 20);
    lv_obj_set_y(hint, 116);
    lv_obj_set_width(hint, 200);
    lv_obj_set_style_text_color(hint, FG_MUTED, 0);

    lv_obj_t * back = lv_button_create(screen);
    lv_obj_set_x(back, 20);
    lv_obj_set_y(back, 206);
    lv_obj_set_width(back, 200);
    lv_obj_set_height(back, 42);
    lv_obj_set_style_bg_color(back, CARD_DARK, 0);
    lv_obj_set_style_radius(back, RADIUS_MD, 0);
    lv_obj_add_event_cb(back, back_button_event_cb, LV_EVENT_CLICKED, adapter);

    lv_obj_t * back_label = lv_label_create(back);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    load_screen(adapter, screen);
}

static void load_screen(WatchCoreUiAdapter * adapter, lv_obj_t * screen)
{
    if (screen == NULL) {
        return;
    }

    lv_obj_t * previous_screen = adapter->active_screen;
    adapter->active_screen = screen;
    lv_screen_load(screen);

    if (previous_screen != NULL && previous_screen != screen) {
        lv_obj_delete_async(previous_screen);
    }
}

static WatchCoreHealthFeature feature_from_card_id(const char * card_id)
{
    if (card_id == NULL) {
        return WATCH_CORE_HEALTH_FEATURE_INVALID;
    }

    for (uint32_t i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        if (strcmp(card_id, health_card_ids[i]) == 0) {
            return health_card_features[i];
        }
    }

    return WATCH_CORE_HEALTH_FEATURE_INVALID;
}

static void style_screen_base(lv_obj_t * screen)
{
    lv_obj_set_style_bg_color(screen, HEALTH_SCREEN_BG, 0);
    lv_obj_set_style_text_color(screen, FG_PRIMARY, 0);
    lv_obj_set_flag(screen, LV_OBJ_FLAG_SCROLLABLE, false);
}

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

static const lv_coord_t click_drag_threshold = 12;

static void health_card_handler(const char * card_id, void * user_data);
static void health_hit_target_clicked(void * user_data);
static void back_button_clicked(void * user_data);
static void bind_health_shortcut_screen(WatchCoreUiAdapter * adapter, lv_obj_t * screen);
static void create_health_card_hit_targets(WatchCoreUiAdapter * adapter, lv_obj_t * screen);
static void guarded_click_attach(
    lv_obj_t * object,
    WatchCoreUiGuardedClick * click,
    WatchCoreUiGuardedClickHandler handler,
    void * user_data,
    const char * log_label);
static void guarded_click_event_cb(lv_event_t * event);
static void click_guard_handle_event(WatchCoreUiClickGuardState * guard, lv_event_t * event);
static bool click_guard_allows(WatchCoreUiClickGuardState * guard);
static void apply_snapshot_to_subjects(WatchCoreUiAdapter * adapter);
static void format_battery_text(
    WatchCoreBatteryState battery,
    char * out_text,
    uint32_t out_text_size);
static void dispatch_event(WatchCoreUiAdapter * adapter, WatchCoreUiEvent event);
static void sync_core_to_view(WatchCoreUiAdapter * adapter, WatchCorePageIntent last_intent);
static void apply_page_state(WatchCoreUiAdapter * adapter, WatchCorePageState page_state);
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
    lv_subject_init_string(
        &adapter->battery_subject,
        adapter->battery_subject_buffer,
        adapter->battery_subject_previous_buffer,
        WATCH_CORE_UI_BATTERY_TEXT_MAX,
        "--%");

    adapter->subjects_initialized = true;
    magic_watch_ui_set_health_card_event_handler(health_card_handler, adapter);
    {
        WatchCorePageIntent initial_intent;
        initial_intent.type = WATCH_CORE_PAGE_INTENT_NONE;
        initial_intent.feature = WATCH_CORE_HEALTH_FEATURE_INVALID;
        sync_core_to_view(adapter, initial_intent);
    }
    return adapter->active_screen != NULL;
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

bool watch_core_ui_adapter_sync_snapshot(WatchCoreUiAdapter * adapter)
{
    if (adapter == NULL || adapter->core == NULL || !adapter->subjects_initialized) {
        return false;
    }

    apply_snapshot_to_subjects(adapter);
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

static void health_hit_target_clicked(void * user_data)
{
    WatchCoreHealthHitTarget * target = (WatchCoreHealthHitTarget *)user_data;
    if (target == NULL || target->adapter == NULL) {
        return;
    }

    dispatch_event(
        target->adapter,
        watch_core_make_health_card_clicked_event(feature_from_card_id(target->card_id)));
}

static void back_button_clicked(void * user_data)
{
    WatchCoreUiAdapter * adapter = (WatchCoreUiAdapter *)user_data;
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

    lv_obj_t * battery_label = magic_watch_ui_get_health_shortcuts_battery_label(screen);
    if (battery_label != NULL) {
        lv_label_bind_text(battery_label, &adapter->battery_subject, NULL);
    }

    create_health_card_hit_targets(adapter, screen);
}

static void create_health_card_hit_targets(WatchCoreUiAdapter * adapter, lv_obj_t * screen)
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

        adapter->health_hit_targets[i].adapter = adapter;
        adapter->health_hit_targets[i].card_id = health_card_ids[i];

        lv_obj_remove_style_all(hit);
        lv_obj_set_x(hit, x[i]);
        lv_obj_set_y(hit, y[i]);
        lv_obj_set_width(hit, HEALTH_CARD_W);
        lv_obj_set_height(hit, HEALTH_CARD_H);
        lv_obj_set_ext_click_area(hit, 6);
        lv_obj_add_flag(hit, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_flag(hit, LV_OBJ_FLAG_SCROLLABLE, false);
        guarded_click_attach(
            hit,
            &adapter->health_hit_targets[i].click,
            health_hit_target_clicked,
            &adapter->health_hit_targets[i],
            health_card_ids[i]);
    }
}

static void guarded_click_attach(
    lv_obj_t * object,
    WatchCoreUiGuardedClick * click,
    WatchCoreUiGuardedClickHandler handler,
    void * user_data,
    const char * log_label)
{
    if (object == NULL || click == NULL || handler == NULL) {
        return;
    }

    click->guard.active = false;
    click->guard.moved = false;
    click->guard.press_point.x = 0;
    click->guard.press_point.y = 0;
    click->handler = handler;
    click->user_data = user_data;
    click->log_label = log_label;

    lv_obj_add_event_cb(object, guarded_click_event_cb, LV_EVENT_ALL, click);
}

static void guarded_click_event_cb(lv_event_t * event)
{
    WatchCoreUiGuardedClick * click = (WatchCoreUiGuardedClick *)lv_event_get_user_data(event);
    if (click == NULL) {
        return;
    }

    if (lv_event_get_code(event) != LV_EVENT_CLICKED) {
        click_guard_handle_event(&click->guard, event);
        return;
    }

    if (!click_guard_allows(&click->guard)) {
        LV_LOG_USER("UiEvent ignored after drag target=%s",
                    click->log_label != NULL ? click->log_label : "unknown");
        return;
    }

    if (click->handler != NULL) {
        click->handler(click->user_data);
    }
}

static void click_guard_handle_event(WatchCoreUiClickGuardState * guard, lv_event_t * event)
{
    if (guard == NULL || event == NULL) {
        return;
    }

    switch (lv_event_get_code(event)) {
        case LV_EVENT_PRESSED: {
            lv_point_t point = {0, 0};
            lv_indev_t * indev = lv_event_get_indev(event);
            if (indev != NULL) {
                lv_indev_get_point(indev, &point);
            }
            guard->press_point = point;
            guard->active = true;
            guard->moved = false;
            break;
        }
        case LV_EVENT_PRESSING: {
            if (!guard->active) {
                break;
            }
            lv_point_t point = {0, 0};
            lv_indev_t * indev = lv_event_get_indev(event);
            if (indev != NULL) {
                lv_indev_get_point(indev, &point);
            }
            const lv_coord_t dx = point.x - guard->press_point.x;
            const lv_coord_t dy = point.y - guard->press_point.y;
            if (LV_ABS(dx) >= click_drag_threshold || LV_ABS(dy) >= click_drag_threshold) {
                guard->moved = true;
            }
            break;
        }
        case LV_EVENT_PRESS_LOST:
            guard->moved = true;
            guard->active = false;
            break;
        default:
            break;
    }
}

static bool click_guard_allows(WatchCoreUiClickGuardState * guard)
{
    if (guard == NULL) {
        return false;
    }

    const bool allows = !guard->moved;
    guard->active = false;
    guard->moved = false;
    return allows;
}

static void apply_snapshot_to_subjects(WatchCoreUiAdapter * adapter)
{
    WatchCoreUiModelSnapshot snapshot;
    char battery_text[WATCH_CORE_UI_BATTERY_TEXT_MAX];

    watch_core_get_ui_snapshot(adapter->core, &snapshot);

    for (uint32_t i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        lv_subject_copy_string(&adapter->health_metric_subjects[i], snapshot.health_metric_text[i]);
    }

    format_battery_text(snapshot.battery, battery_text, sizeof(battery_text));
    lv_subject_copy_string(&adapter->battery_subject, battery_text);
}

static void format_battery_text(
    WatchCoreBatteryState battery,
    char * out_text,
    uint32_t out_text_size)
{
    int written;

    if (out_text == NULL || out_text_size == 0U) {
        return;
    }

    if (!battery.present) {
        written = lv_snprintf(out_text, out_text_size, "--%%");
    }
    else if (battery.charging) {
        written = lv_snprintf(out_text, out_text_size, "%u%% +", (unsigned int)battery.percent);
    }
    else {
        written = lv_snprintf(out_text, out_text_size, "%u%%", (unsigned int)battery.percent);
    }

    if (written < 0) {
        out_text[0] = '\0';
    }
}

static void dispatch_event(WatchCoreUiAdapter * adapter, WatchCoreUiEvent event)
{
    WatchCorePageIntent last_intent;

    LV_LOG_USER("UiEvent %s feature=%s",
                watch_core_ui_event_name(event.type),
                watch_core_health_feature_name(event.feature));

    if (!watch_core_push_event(adapter->core, event)) {
        LV_LOG_WARN("watch_core event queue is full");
        return;
    }

    last_intent = watch_core_process_pending_events(adapter->core);
    sync_core_to_view(adapter, last_intent);
}

static void sync_core_to_view(WatchCoreUiAdapter * adapter, WatchCorePageIntent last_intent)
{
    WatchCorePageState page_state;

    apply_snapshot_to_subjects(adapter);
    watch_core_get_current_page_state(adapter->core, &page_state);

    LV_LOG_USER("PageIntent(last) %s feature=%s",
                watch_core_page_intent_name(last_intent.type),
                watch_core_health_feature_name(last_intent.feature));
    LV_LOG_USER("PageState(current) %s feature=%s",
                watch_core_page_name(page_state.type),
                watch_core_health_feature_name(page_state.feature));

    apply_page_state(adapter, page_state);
}

static void apply_page_state(WatchCoreUiAdapter * adapter, WatchCorePageState page_state)
{
    switch (page_state.type) {
        case WATCH_CORE_PAGE_HEALTH_SHORTCUTS:
            (void)watch_core_ui_adapter_load_health_shortcuts(adapter);
            break;
        case WATCH_CORE_PAGE_HEALTH_DETAIL:
            load_health_detail(adapter, page_state.feature);
            break;
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
    guarded_click_attach(back, &adapter->back_click, back_button_clicked, adapter, "back");

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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "watch_core/watch_core.h"

static int s_failures = 0;

static void expect_true(bool condition, const char *message)
{
    if (!condition) {
        ++s_failures;
        fprintf(stderr, "FAIL: %s\n", message);
    }
}

static void expect_string_eq(const char *actual, const char *expected, const char *message)
{
    if ((actual == NULL) || (expected == NULL) || (strcmp(actual, expected) != 0)) {
        ++s_failures;
        fprintf(stderr,
                "FAIL: %s (expected=\"%s\" actual=\"%s\")\n",
                message,
                expected != NULL ? expected : "(null)",
                actual != NULL ? actual : "(null)");
    }
}

static void expect_int_eq(int actual, int expected, const char *message)
{
    if (actual != expected) {
        ++s_failures;
        fprintf(stderr, "FAIL: %s (expected=%d actual=%d)\n", message, expected, actual);
    }
}

static void expect_page_state_eq(
    WatchCorePageState actual, WatchCorePageType expected_type, WatchCoreHealthFeature expected_feature, const char *message)
{
    if ((actual.type != expected_type) || (actual.feature != expected_feature)) {
        ++s_failures;
        fprintf(stderr,
                "FAIL: %s (expected page=%s feature=%d, got page=%s feature=%d)\n",
                message,
                watch_core_page_name(expected_type),
                (int)expected_feature,
                watch_core_page_name(actual.type),
                (int)actual.feature);
    }
}

static void expect_power_state_eq(
    WatchCorePowerState actual, WatchCorePowerState expected, const char *message)
{
    if (actual != expected) {
        ++s_failures;
        fprintf(stderr,
                "FAIL: %s (expected power_state=%d actual=%d)\n",
                message,
                (int)expected,
                (int)actual);
    }
}

static void expect_power_action_eq(
    WatchCorePowerAction actual,
    WatchCorePowerActionType expected_type,
    WatchCorePowerState expected_source_state,
    WatchCorePowerState expected_target_state,
    const char *message)
{
    if (actual.type != expected_type ||
        actual.source_state != expected_source_state ||
        actual.target_state != expected_target_state) {
        ++s_failures;
        fprintf(stderr,
                "FAIL: %s (expected action=%d source=%d target=%d, actual action=%d source=%d target=%d)\n",
                message,
                (int)expected_type,
                (int)expected_source_state,
                (int)expected_target_state,
                (int)actual.type,
                (int)actual.source_state,
                (int)actual.target_state);
    }
}

static void test_default_page_semantics_are_proven_by_public_behavior(void)
{
    WatchCore core;
    WatchCorePageIntent intent;

    watch_core_init(&core);
    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "back enqueue after init should succeed");
    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "back after init should be no-op, proving default shortcut semantics");

    expect_true(watch_core_push_event(
                    &core,
                    watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_HEART_RATE)),
                "card click enqueue after init should succeed");
    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                  "card click after init should enter detail");
    expect_int_eq(intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_HEART_RATE,
                  "detail intent after init should carry clicked feature");

    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "back enqueue after detail should succeed");
    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS,
                  "back after detail should return to shortcuts");
    expect_int_eq(intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_INVALID,
                  "shortcut return intent should clear feature");
}

static void test_public_page_state_reports_default_route(void)
{
    WatchCore core;
    WatchCorePageState page_state;

    watch_core_init(&core);
    watch_core_get_current_page_state(&core, &page_state);

    expect_page_state_eq(page_state,
                         WATCH_CORE_PAGE_HEALTH_SHORTCUTS,
                         WATCH_CORE_HEALTH_FEATURE_INVALID,
                         "public page state after init should be health shortcuts");
}

static void test_health_card_events_map_to_detail_intents(void)
{
    WatchCoreHealthFeature features[WATCH_CORE_HEALTH_CARD_COUNT] = {
        WATCH_CORE_HEALTH_FEATURE_HEART_RATE,
        WATCH_CORE_HEALTH_FEATURE_SPO2,
        WATCH_CORE_HEALTH_FEATURE_BREATHE,
        WATCH_CORE_HEALTH_FEATURE_STRESS,
    };
    uint32_t i;

    for (i = 0U; i < WATCH_CORE_HEALTH_CARD_COUNT; ++i) {
        WatchCore core;
        WatchCorePageIntent intent;

        watch_core_init(&core);
        expect_true(watch_core_push_event(&core, watch_core_make_health_card_clicked_event(features[i])),
                    "health card click should enqueue");
        intent = watch_core_process_next_event(&core);

        expect_int_eq(intent.type,
                      WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                      "health card click should load detail");
        expect_int_eq(intent.feature, features[i], "detail intent should keep clicked feature");
    }
}

static void test_back_from_detail_returns_to_shortcuts(void)
{
    WatchCore core;
    WatchCorePageIntent intent;

    watch_core_init(&core);
    expect_true(watch_core_push_event(
                    &core,
                    watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_HEART_RATE)),
                "detail navigation enqueue should succeed");
    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                  "card click should enter detail before back");

    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "back enqueue from detail should succeed");
    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS,
                  "back from detail should return to shortcuts");
    expect_int_eq(intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_INVALID,
                  "shortcut intent should clear feature");
}

static void test_public_page_state_tracks_navigation_result(void)
{
    WatchCore core;
    WatchCorePageState page_state;

    watch_core_init(&core);
    expect_true(watch_core_push_event(
                    &core,
                    watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_STRESS)),
                "detail navigation enqueue should succeed for page state tracking");
    (void)watch_core_process_next_event(&core);
    watch_core_get_current_page_state(&core, &page_state);
    expect_page_state_eq(page_state,
                         WATCH_CORE_PAGE_HEALTH_DETAIL,
                         WATCH_CORE_HEALTH_FEATURE_STRESS,
                         "public page state should reflect entered detail");

    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "back enqueue should succeed for page state tracking");
    (void)watch_core_process_next_event(&core);
    watch_core_get_current_page_state(&core, &page_state);
    expect_page_state_eq(page_state,
                         WATCH_CORE_PAGE_HEALTH_SHORTCUTS,
                         WATCH_CORE_HEALTH_FEATURE_INVALID,
                         "public page state should reflect shortcut return");
}

static void test_back_on_shortcuts_is_no_op(void)
{
    WatchCore core;
    WatchCorePageIntent intent;

    watch_core_init(&core);
    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "back enqueue on shortcuts should succeed");
    intent = watch_core_process_next_event(&core);

    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "back on shortcuts should be no-op");
}

static void test_invalid_feature_does_not_jump(void)
{
    WatchCore core;
    WatchCorePageIntent intent;

    watch_core_init(&core);
    expect_true(
        watch_core_push_event(&core,
                              watch_core_make_health_card_clicked_event(
                                  WATCH_CORE_HEALTH_FEATURE_INVALID)),
        "invalid feature enqueue should still enter queue");
    intent = watch_core_process_next_event(&core);

    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "invalid feature should not create page intent");
}

static void test_empty_queue_returns_no_op(void)
{
    WatchCore core;
    WatchCorePageIntent intent;

    watch_core_init(&core);
    intent = watch_core_process_next_event(&core);

    expect_int_eq(intent.type, WATCH_CORE_PAGE_INTENT_NONE, "empty queue should return no-op");
    expect_int_eq(intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_INVALID,
                  "empty queue no-op feature should stay invalid");
}

static void test_fifo_preserves_order(void)
{
    WatchCore core;
    WatchCorePageIntent intent;

    watch_core_init(&core);
    expect_true(watch_core_push_event(
                    &core,
                    watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_HEART_RATE)),
                "first enqueue should succeed");
    expect_true(
        watch_core_push_event(
            &core,
            watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_STRESS)),
        "second enqueue should succeed");

    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                  "first FIFO item should be detail intent");
    expect_int_eq(intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_HEART_RATE,
                  "first FIFO item should keep first feature");

    intent = watch_core_process_next_event(&core);
    expect_int_eq(intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                  "second FIFO item should be detail intent");
    expect_int_eq(intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_STRESS,
                  "second FIFO item should keep second feature");
}

static void test_ring_buffer_full_rejects_new_event_without_reordering(void)
{
    WatchCore core;
    WatchCorePageIntent intent;
    WatchCoreHealthFeature sequence[] = {
        WATCH_CORE_HEALTH_FEATURE_HEART_RATE,
        WATCH_CORE_HEALTH_FEATURE_SPO2,
        WATCH_CORE_HEALTH_FEATURE_BREATHE,
        WATCH_CORE_HEALTH_FEATURE_STRESS,
        WATCH_CORE_HEALTH_FEATURE_HEART_RATE,
        WATCH_CORE_HEALTH_FEATURE_SPO2,
        WATCH_CORE_HEALTH_FEATURE_BREATHE,
        WATCH_CORE_HEALTH_FEATURE_STRESS,
    };
    const size_t sequence_count = sizeof(sequence) / sizeof(sequence[0]);
    uint32_t i;

    watch_core_init(&core);
    for (i = 0U; i < sequence_count; ++i) {
        expect_true(watch_core_push_event(&core, watch_core_make_health_card_clicked_event(sequence[i])),
                    "buffer fill should succeed until capacity");
    }

    expect_true(
        !watch_core_push_event(
            &core,
            watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_HEART_RATE)),
        "full buffer should reject new event");

    for (i = 0U; i < sequence_count; ++i) {
        intent = watch_core_process_next_event(&core);
        expect_int_eq(intent.type,
                      WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                      "filled buffer should still drain valid detail intents");
        expect_int_eq(intent.feature, sequence[i], "full buffer should preserve prior FIFO order");
    }
}

static void test_snapshot_updates_are_readable(void)
{
    WatchCore core;
    WatchCoreUiModelSnapshot snapshot;

    watch_core_init(&core);
    expect_true(watch_core_set_health_metric(&core, WATCH_CORE_HEALTH_FEATURE_HEART_RATE, "101"),
                "set metric should succeed for valid feature");
    watch_core_get_ui_snapshot(&core, &snapshot);

    expect_string_eq(snapshot.health_metric_text[WATCH_CORE_HEALTH_FEATURE_HEART_RATE],
                     "101",
                     "snapshot should reflect latest metric");
}

static void test_metric_text_is_safely_truncated(void)
{
    WatchCore core;
    WatchCoreUiModelSnapshot snapshot;
    const char *long_text = "1234567890ABCDEFGHIJK";
    char expected[WATCH_CORE_METRIC_TEXT_MAX];
    size_t i;

    watch_core_init(&core);
    expect_true(watch_core_set_health_metric(&core, WATCH_CORE_HEALTH_FEATURE_SPO2, long_text),
                "set metric should accept long text and truncate");
    watch_core_get_ui_snapshot(&core, &snapshot);

    for (i = 0U; i + 1U < WATCH_CORE_METRIC_TEXT_MAX; ++i) {
        expected[i] = long_text[i];
    }
    expected[WATCH_CORE_METRIC_TEXT_MAX - 1U] = '\0';

    expect_string_eq(snapshot.health_metric_text[WATCH_CORE_HEALTH_FEATURE_SPO2],
                     expected,
                     "snapshot should contain truncated metric text");
    expect_true(snapshot.health_metric_text[WATCH_CORE_HEALTH_FEATURE_SPO2]
                                         [WATCH_CORE_METRIC_TEXT_MAX - 1U] == '\0',
                "truncated metric text must stay NUL-terminated");
}

static void test_process_pending_events_drains_to_stable_state(void)
{
    WatchCore core;
    WatchCorePageIntent last_intent;
    WatchCorePageState page_state;

    watch_core_init(&core);
    expect_true(
        watch_core_push_event(
            &core,
            watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_HEART_RATE)),
        "first pending event enqueue should succeed");
    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "second pending event enqueue should succeed");

    last_intent = watch_core_process_pending_events(&core);
    watch_core_get_current_page_state(&core, &page_state);

    expect_int_eq(last_intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_SHORTCUTS,
                  "pending drain should report last non-none navigation action");
    expect_int_eq(last_intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_INVALID,
                  "pending drain final shortcut action should clear feature");
    expect_page_state_eq(page_state,
                         WATCH_CORE_PAGE_HEALTH_SHORTCUTS,
                         WATCH_CORE_HEALTH_FEATURE_INVALID,
                         "pending drain should leave final stable shortcut state");
    expect_int_eq(watch_core_process_next_event(&core).type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "after pending drain no extra events should remain");
}

static void test_pending_drain_continues_after_shortcut_back_no_op(void)
{
    WatchCore core;
    WatchCorePageIntent last_intent;
    WatchCorePageState page_state;

    watch_core_init(&core);
    expect_true(watch_core_push_event(&core, watch_core_make_back_event()),
                "shortcut back enqueue should succeed before valid click");
    expect_true(
        watch_core_push_event(
            &core,
            watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_SPO2)),
        "valid click enqueue should succeed after shortcut back no-op");

    last_intent = watch_core_process_pending_events(&core);
    watch_core_get_current_page_state(&core, &page_state);

    expect_int_eq(last_intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                  "pending drain should continue after shortcut back no-op and report later detail action");
    expect_int_eq(last_intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_SPO2,
                  "pending drain should keep the later valid detail feature");
    expect_page_state_eq(page_state,
                         WATCH_CORE_PAGE_HEALTH_DETAIL,
                         WATCH_CORE_HEALTH_FEATURE_SPO2,
                         "pending drain should end on the later valid detail page");
    expect_int_eq(watch_core_process_next_event(&core).type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "pending drain after shortcut back no-op should still leave queue empty");
}

static void test_pending_drain_continues_after_invalid_feature_no_op(void)
{
    WatchCore core;
    WatchCorePageIntent last_intent;
    WatchCorePageState page_state;

    watch_core_init(&core);
    expect_true(
        watch_core_push_event(&core,
                              watch_core_make_health_card_clicked_event(
                                  WATCH_CORE_HEALTH_FEATURE_INVALID)),
        "invalid feature enqueue should succeed before valid click");
    expect_true(
        watch_core_push_event(
            &core,
            watch_core_make_health_card_clicked_event(WATCH_CORE_HEALTH_FEATURE_BREATHE)),
        "valid click enqueue should succeed after invalid feature no-op");

    last_intent = watch_core_process_pending_events(&core);
    watch_core_get_current_page_state(&core, &page_state);

    expect_int_eq(last_intent.type,
                  WATCH_CORE_PAGE_INTENT_LOAD_HEALTH_DETAIL,
                  "pending drain should continue after invalid feature no-op");
    expect_int_eq(last_intent.feature,
                  WATCH_CORE_HEALTH_FEATURE_BREATHE,
                  "pending drain should report the later valid feature after invalid no-op");
    expect_page_state_eq(page_state,
                         WATCH_CORE_PAGE_HEALTH_DETAIL,
                         WATCH_CORE_HEALTH_FEATURE_BREATHE,
                         "pending drain should end on the later valid detail after invalid no-op");
    expect_int_eq(watch_core_process_next_event(&core).type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "pending drain after invalid feature no-op should still leave queue empty");
}

static void test_power_state_defaults_to_screen_on(void)
{
    WatchCore core;
    WatchCorePowerState power_state;

    watch_core_init(&core);
    watch_core_get_power_state(&core, &power_state);

    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_ON,
                          "power state after init should default to screen on");
}

static void test_power_request_screen_off_does_not_mutate_state_before_commit(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    watch_core_get_power_state(&core, &power_state);

    expect_power_action_eq(action,
                           WATCH_CORE_POWER_ACTION_TURN_SCREEN_OFF,
                           WATCH_CORE_POWER_STATE_SCREEN_ON,
                           WATCH_CORE_POWER_STATE_SCREEN_OFF,
                           "screen off request should create turn-screen-off action");
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_ON,
                          "screen off request should not mutate state before commit");
}

static void test_power_failed_screen_off_commit_keeps_screen_on(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);

    expect_true(watch_core_commit_power_action(&core, action, false),
                "failed screen off apply should still be accepted as a valid commit attempt");
    watch_core_get_power_state(&core, &power_state);
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_ON,
                          "failed screen off apply should keep screen on");
}

static void test_power_successful_screen_off_commit_moves_to_screen_off(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);

    expect_true(watch_core_commit_power_action(&core, action, true),
                "successful screen off apply should commit");
    watch_core_get_power_state(&core, &power_state);
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_OFF,
                          "successful screen off apply should move to screen off");
}

static void test_power_request_wake_does_not_mutate_state_before_commit(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "setup screen off commit should succeed before wake request");

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_WAKE);
    watch_core_get_power_state(&core, &power_state);

    expect_power_action_eq(action,
                           WATCH_CORE_POWER_ACTION_WAKE_SCREEN,
                           WATCH_CORE_POWER_STATE_SCREEN_OFF,
                           WATCH_CORE_POWER_STATE_SCREEN_ON,
                           "wake request should create wake-screen action");
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_OFF,
                          "wake request should not mutate state before commit");
}

static void test_power_failed_wake_commit_keeps_screen_off(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "setup screen off commit should succeed before failed wake");

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_WAKE);
    expect_true(watch_core_commit_power_action(&core, action, false),
                "failed wake apply should still be accepted as a valid commit attempt");
    watch_core_get_power_state(&core, &power_state);
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_OFF,
                          "failed wake apply should keep screen off");
}

static void test_power_successful_wake_commit_moves_to_screen_on(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "setup screen off commit should succeed before wake");

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_WAKE);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "successful wake apply should commit");
    watch_core_get_power_state(&core, &power_state);
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_ON,
                          "successful wake apply should move to screen on");
}

static void test_power_duplicate_requests_return_none(void)
{
    WatchCore core;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_WAKE);
    expect_power_action_eq(action,
                           WATCH_CORE_POWER_ACTION_NONE,
                           WATCH_CORE_POWER_STATE_SCREEN_ON,
                           WATCH_CORE_POWER_STATE_SCREEN_ON,
                           "wake request while already on should be a no-op");

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "setup screen off commit should succeed before duplicate screen off request");

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_power_action_eq(action,
                           WATCH_CORE_POWER_ACTION_NONE,
                           WATCH_CORE_POWER_STATE_SCREEN_OFF,
                           WATCH_CORE_POWER_STATE_SCREEN_OFF,
                           "screen off request while already off should be a no-op");
}

static void test_power_invalid_or_mismatched_commit_is_rejected(void)
{
    WatchCore core;
    WatchCorePowerState power_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    action.type = WATCH_CORE_POWER_ACTION_NONE;
    action.source_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
    action.target_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
    expect_true(!watch_core_commit_power_action(&core, action, true),
                "committing a none action should be rejected");

    action.type = WATCH_CORE_POWER_ACTION_WAKE_SCREEN;
    action.source_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
    action.target_state = WATCH_CORE_POWER_STATE_SCREEN_ON;
    expect_true(!watch_core_commit_power_action(&core, action, true),
                "committing an invalid wake action shape should be rejected");

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "setup screen off commit should succeed before mismatch check");
    expect_true(!watch_core_commit_power_action(&core, action, true),
                "committing an action whose source state no longer matches should be rejected");

    watch_core_get_power_state(&core, &power_state);
    expect_power_state_eq(power_state,
                          WATCH_CORE_POWER_STATE_SCREEN_OFF,
                          "rejected mismatched commit should not change power state");
}

static void test_power_round_trip_preserves_page_state_snapshot_and_queue_contract(void)
{
    WatchCore core;
    WatchCoreUiModelSnapshot before_snapshot;
    WatchCoreUiModelSnapshot after_snapshot;
    WatchCorePageState before_page_state;
    WatchCorePageState after_page_state;
    WatchCorePowerAction action;

    watch_core_init(&core);
    expect_true(watch_core_set_health_metric(&core, WATCH_CORE_HEALTH_FEATURE_STRESS, "42"),
                "snapshot setup should succeed before power round trip");
    watch_core_get_ui_snapshot(&core, &before_snapshot);
    watch_core_get_current_page_state(&core, &before_page_state);

    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_SCREEN_OFF);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "screen off commit should succeed during round trip");
    action = watch_core_request_power_action(&core, WATCH_CORE_POWER_REQUEST_WAKE);
    expect_true(watch_core_commit_power_action(&core, action, true),
                "wake commit should succeed during round trip");

    watch_core_get_ui_snapshot(&core, &after_snapshot);
    watch_core_get_current_page_state(&core, &after_page_state);

    expect_page_state_eq(after_page_state,
                         before_page_state.type,
                         before_page_state.feature,
                         "power round trip should preserve page state");
    expect_string_eq(after_snapshot.health_metric_text[WATCH_CORE_HEALTH_FEATURE_STRESS],
                     before_snapshot.health_metric_text[WATCH_CORE_HEALTH_FEATURE_STRESS],
                     "power round trip should preserve snapshot text");
    expect_int_eq(watch_core_process_next_event(&core).type,
                  WATCH_CORE_PAGE_INTENT_NONE,
                  "power round trip should not enqueue UI navigation events");
}

int main(void)
{
    test_default_page_semantics_are_proven_by_public_behavior();
    test_public_page_state_reports_default_route();
    test_health_card_events_map_to_detail_intents();
    test_back_from_detail_returns_to_shortcuts();
    test_public_page_state_tracks_navigation_result();
    test_back_on_shortcuts_is_no_op();
    test_invalid_feature_does_not_jump();
    test_empty_queue_returns_no_op();
    test_fifo_preserves_order();
    test_ring_buffer_full_rejects_new_event_without_reordering();
    test_snapshot_updates_are_readable();
    test_metric_text_is_safely_truncated();
    test_process_pending_events_drains_to_stable_state();
    test_pending_drain_continues_after_shortcut_back_no_op();
    test_pending_drain_continues_after_invalid_feature_no_op();
    test_power_state_defaults_to_screen_on();
    test_power_request_screen_off_does_not_mutate_state_before_commit();
    test_power_failed_screen_off_commit_keeps_screen_on();
    test_power_successful_screen_off_commit_moves_to_screen_off();
    test_power_request_wake_does_not_mutate_state_before_commit();
    test_power_failed_wake_commit_keeps_screen_off();
    test_power_successful_wake_commit_moves_to_screen_on();
    test_power_duplicate_requests_return_none();
    test_power_invalid_or_mismatched_commit_is_rejected();
    test_power_round_trip_preserves_page_state_snapshot_and_queue_contract();

    if (s_failures != 0) {
        fprintf(stderr, "%d contract test(s) failed.\n", s_failures);
        return 1;
    }

    printf("watch_core contract tests passed.\n");
    return 0;
}

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

int main(void)
{
    test_default_page_semantics_are_proven_by_public_behavior();
    test_health_card_events_map_to_detail_intents();
    test_back_from_detail_returns_to_shortcuts();
    test_back_on_shortcuts_is_no_op();
    test_invalid_feature_does_not_jump();
    test_empty_queue_returns_no_op();
    test_fifo_preserves_order();
    test_ring_buffer_full_rejects_new_event_without_reordering();
    test_snapshot_updates_are_readable();
    test_metric_text_is_safely_truncated();

    if (s_failures != 0) {
        fprintf(stderr, "%d contract test(s) failed.\n", s_failures);
        return 1;
    }

    printf("watch_core contract tests passed.\n");
    return 0;
}

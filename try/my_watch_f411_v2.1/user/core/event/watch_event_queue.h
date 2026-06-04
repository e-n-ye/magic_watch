#ifndef WATCH_EVENT_QUEUE_H
#define WATCH_EVENT_QUEUE_H

#include <stdint.h>

#include "app/input/watch_input_intent.h"

#define WATCH_EVENT_QUEUE_CAPACITY 12U

typedef enum {
    WATCH_APP_EVENT_NONE = 0,
    WATCH_APP_EVENT_INPUT_INTENT,
} watch_app_event_type_t;

typedef struct {
    watch_app_event_type_t type;
    union {
        watch_input_intent_t input_intent;
    } payload;
} watch_app_event_t;

typedef struct {
    watch_app_event_t items[WATCH_EVENT_QUEUE_CAPACITY];
    uint8_t read_index;
    uint8_t write_index;
    uint8_t drop_count;
} watch_event_queue_t;

void watch_event_queue_init(watch_event_queue_t *queue);
uint8_t watch_event_queue_push(watch_event_queue_t *queue, watch_app_event_t event);
uint8_t watch_event_queue_pop(watch_event_queue_t *queue, watch_app_event_t *event);
uint8_t watch_event_queue_is_empty(const watch_event_queue_t *queue);
uint8_t watch_event_queue_drop_count(const watch_event_queue_t *queue);

#endif /* WATCH_EVENT_QUEUE_H */

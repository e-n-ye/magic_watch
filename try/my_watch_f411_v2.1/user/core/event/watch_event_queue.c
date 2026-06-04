#include "core/event/watch_event_queue.h"

static uint8_t next_index(uint8_t index)
{
    return (uint8_t)((index + 1U) % WATCH_EVENT_QUEUE_CAPACITY);
}

void watch_event_queue_init(watch_event_queue_t *queue)
{
    if (queue == 0) {
        return;
    }

    queue->read_index = 0U;
    queue->write_index = 0U;
    queue->drop_count = 0U;
}

uint8_t watch_event_queue_push(watch_event_queue_t *queue, watch_app_event_t event)
{
    uint8_t next;

    if (queue == 0) {
        return 0U;
    }

    next = next_index(queue->write_index);
    if (next == queue->read_index) {
        if (queue->drop_count < 255U) {
            queue->drop_count++;
        }
        return 0U;
    }

    queue->items[queue->write_index] = event;
    queue->write_index = next;
    return 1U;
}

uint8_t watch_event_queue_pop(watch_event_queue_t *queue, watch_app_event_t *event)
{
    if ((queue == 0) || (event == 0) || (queue->read_index == queue->write_index)) {
        return 0U;
    }

    *event = queue->items[queue->read_index];
    queue->read_index = next_index(queue->read_index);
    return 1U;
}

uint8_t watch_event_queue_is_empty(const watch_event_queue_t *queue)
{
    if (queue == 0) {
        return 1U;
    }

    return (queue->read_index == queue->write_index) ? 1U : 0U;
}

uint8_t watch_event_queue_drop_count(const watch_event_queue_t *queue)
{
    if (queue == 0) {
        return 0U;
    }

    return queue->drop_count;
}

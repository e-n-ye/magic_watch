/**
 * @file magic_watch_ui.h
 */

#ifndef MAGIC_WATCH_UI_H
#define MAGIC_WATCH_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>

#include "magic_watch_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*magic_watch_health_card_event_handler_t)(const char * card_id, void * user_data);

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the component library
 */
void magic_watch_ui_init(const char * asset_path);

void magic_watch_ui_set_health_card_event_handler(
    magic_watch_health_card_event_handler_t handler,
    void * user_data);

lv_obj_t * magic_watch_ui_get_health_card(lv_obj_t * screen, uint32_t index);

lv_obj_t * magic_watch_ui_get_health_card_metric_label(lv_obj_t * card);

lv_obj_t * magic_watch_ui_get_health_shortcuts_battery_label(lv_obj_t * screen);

/**
 * Placeholder click hook for health shortcut cards.
 * The component passes a string card id via user_data.
 */
void magic_watch_health_card_clicked(lv_event_t * e);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*MAGIC_WATCH_UI_H*/

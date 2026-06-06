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

#include "magic_watch_ui_gen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

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

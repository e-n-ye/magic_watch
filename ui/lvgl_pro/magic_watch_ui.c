/**
 * @file magic_watch_ui.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "magic_watch_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void magic_watch_ui_init(const char * asset_path)
{
    magic_watch_ui_init_gen(asset_path);

    /* Add your own custom code here if needed */
}

void magic_watch_health_card_clicked(lv_event_t * e)
{
    const char * card_id = (const char *)lv_event_get_user_data(e);

    /* Reserved for Q1-4 UI Adapter / UiEvent wiring. */
    (void)card_id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

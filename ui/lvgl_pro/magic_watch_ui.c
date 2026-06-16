/**
 * @file magic_watch_ui.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "magic_watch_ui.h"

#include <string.h>

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

static magic_watch_health_card_event_handler_t health_card_handler;
static void * health_card_handler_user_data;

static lv_obj_t * magic_watch_ui_find_child_by_name(lv_obj_t * parent, const char * name);

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

void magic_watch_ui_set_health_card_event_handler(
    magic_watch_health_card_event_handler_t handler,
    void * user_data)
{
    health_card_handler = handler;
    health_card_handler_user_data = user_data;
}

lv_obj_t * magic_watch_ui_get_health_card(lv_obj_t * screen, uint32_t index)
{
    if (screen == NULL || index >= 4U) {
        return NULL;
    }

    return lv_obj_get_child(screen, (int32_t)index);
}

lv_obj_t * magic_watch_ui_get_health_card_metric_label(lv_obj_t * card)
{
    if (card == NULL) {
        return NULL;
    }

    return lv_obj_get_child(card, 1);
}

lv_obj_t * magic_watch_ui_get_health_shortcuts_battery_label(lv_obj_t * screen)
{
    return magic_watch_ui_find_child_by_name(screen, "battery_label");
}

void magic_watch_health_card_clicked(lv_event_t * e)
{
    const char * card_id = (const char *)lv_event_get_user_data(e);

    if (health_card_handler != NULL) {
        health_card_handler(card_id, health_card_handler_user_data);
    }
}

static lv_obj_t * magic_watch_ui_find_child_by_name(lv_obj_t * parent, const char * name)
{
    if (parent == NULL || name == NULL) {
        return NULL;
    }

    const uint32_t child_count = lv_obj_get_child_count(parent);
    for (uint32_t i = 0U; i < child_count; ++i) {
        lv_obj_t * child = lv_obj_get_child(parent, (int32_t)i);
        const char * child_name = lv_obj_get_name(child);
        if (child_name != NULL && strcmp(child_name, name) == 0) {
            return child;
        }
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

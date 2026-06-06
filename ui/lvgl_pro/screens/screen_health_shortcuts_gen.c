/**
 * @file screen_health_shortcuts_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "screen_health_shortcuts_gen.h"
#include "../magic_watch_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * screen_health_shortcuts_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_name_static(lv_obj_0, "screen_health_shortcuts_#");
    lv_obj_set_style_bg_color(lv_obj_0, HEALTH_SCREEN_BG, 0);
    lv_obj_set_style_text_color(lv_obj_0, FG_PRIMARY, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_t * health_shortcut_card_0 = health_shortcut_card_create(lv_obj_0, "heart", health_heart_64, "--", FG_PRIMARY);
    lv_obj_set_x(health_shortcut_card_0, HEALTH_GRID_LEFT);
    lv_obj_set_y(health_shortcut_card_0, HEALTH_GRID_TOP);
    lv_obj_set_style_bg_color(health_shortcut_card_0, CARD_DARK, 0);

    lv_obj_t * health_shortcut_card_1 = health_shortcut_card_create(lv_obj_0, "spo2", health_spo2_64, "98 %", FG_PRIMARY);
    lv_obj_set_x(health_shortcut_card_1, HEALTH_COL2_X);
    lv_obj_set_y(health_shortcut_card_1, HEALTH_GRID_TOP);
    lv_obj_set_style_bg_color(health_shortcut_card_1, CARD_RED, 0);

    lv_obj_t * health_shortcut_card_2 = health_shortcut_card_create(lv_obj_0, "breathe", health_breathe_64, "18", FG_PRIMARY);
    lv_obj_set_x(health_shortcut_card_2, HEALTH_GRID_LEFT);
    lv_obj_set_y(health_shortcut_card_2, HEALTH_ROW2_Y);
    lv_obj_set_style_bg_color(health_shortcut_card_2, CARD_BLUE, 0);

    lv_obj_t * health_shortcut_card_3 = health_shortcut_card_create(lv_obj_0, "stress", health_stress_64, "34", FG_PRIMARY);
    lv_obj_set_x(health_shortcut_card_3, HEALTH_COL2_X);
    lv_obj_set_y(health_shortcut_card_3, HEALTH_ROW2_Y);
    lv_obj_set_style_bg_color(health_shortcut_card_3, CARD_DARK, 0);

    lv_obj_t * lv_obj_1 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_1, 92);
    lv_obj_set_y(lv_obj_1, 250);
    lv_obj_set_width(lv_obj_1, PAGER_DOT_SIZE);
    lv_obj_set_height(lv_obj_1, PAGER_DOT_SIZE);
    lv_obj_set_flag(lv_obj_1, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_bg_color(lv_obj_1, PAGER_INACTIVE, 0);
    lv_obj_set_style_radius(lv_obj_1, PAGER_DOT_SIZE, 0);
    lv_obj_remove_style_all(lv_obj_1);

    lv_obj_t * lv_obj_2 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_2, 106);
    lv_obj_set_y(lv_obj_2, 250);
    lv_obj_set_width(lv_obj_2, PAGER_DOT_SIZE);
    lv_obj_set_height(lv_obj_2, PAGER_DOT_SIZE);
    lv_obj_set_flag(lv_obj_2, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_bg_color(lv_obj_2, PAGER_ACTIVE, 0);
    lv_obj_set_style_radius(lv_obj_2, PAGER_DOT_SIZE, 0);
    lv_obj_remove_style_all(lv_obj_2);

    lv_obj_t * lv_obj_3 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_3, 120);
    lv_obj_set_y(lv_obj_3, 250);
    lv_obj_set_width(lv_obj_3, PAGER_DOT_SIZE);
    lv_obj_set_height(lv_obj_3, PAGER_DOT_SIZE);
    lv_obj_set_flag(lv_obj_3, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_bg_color(lv_obj_3, PAGER_INACTIVE, 0);
    lv_obj_set_style_radius(lv_obj_3, PAGER_DOT_SIZE, 0);
    lv_obj_remove_style_all(lv_obj_3);

    lv_obj_t * lv_obj_4 = lv_obj_create(lv_obj_0);
    lv_obj_set_x(lv_obj_4, 134);
    lv_obj_set_y(lv_obj_4, 250);
    lv_obj_set_width(lv_obj_4, PAGER_DOT_SIZE);
    lv_obj_set_height(lv_obj_4, PAGER_DOT_SIZE);
    lv_obj_set_flag(lv_obj_4, LV_OBJ_FLAG_SCROLLABLE, false);
    lv_obj_set_style_bg_color(lv_obj_4, PAGER_INACTIVE, 0);
    lv_obj_set_style_radius(lv_obj_4, PAGER_DOT_SIZE, 0);
    lv_obj_remove_style_all(lv_obj_4);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


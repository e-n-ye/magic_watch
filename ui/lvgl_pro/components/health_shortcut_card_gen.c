/**
 * @file health_shortcut_card_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "health_shortcut_card_gen.h"
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

lv_obj_t * health_shortcut_card_create(lv_obj_t * parent, const char * card_id, const void * icon_src, const char * metric_text, lv_color_t metric_color)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t style_card;
    static lv_style_t style_metric;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&style_card);
        lv_style_set_width(&style_card, HEALTH_CARD_W);
        lv_style_set_height(&style_card, HEALTH_CARD_H);
        lv_style_set_pad_all(&style_card, SPACE_SM);
        lv_style_set_radius(&style_card, RADIUS_MD);
        lv_style_set_bg_opa(&style_card, (255 * 100 / 100));
        lv_style_set_bg_color(&style_card, CARD_DARK);

        lv_style_init(&style_metric);
        lv_style_set_text_color(&style_metric, FG_PRIMARY);

        style_inited = true;
    }

    lv_obj_t * lv_button_0 = lv_button_create(parent);
    lv_obj_set_name_static(lv_button_0, "health_shortcut_card_#");
    lv_obj_set_flag(lv_button_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_remove_style_all(lv_button_0);
    lv_obj_add_style(lv_button_0, &style_card, 0);
    lv_obj_t * lv_image_0 = lv_image_create(lv_button_0);
    lv_image_set_src(lv_image_0, icon_src);
    lv_obj_set_x(lv_image_0, -15);
    lv_obj_set_y(lv_image_0, -15);

    lv_obj_t * lv_label_0 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_0, metric_text);
    lv_obj_set_align(lv_label_0, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_text_color(lv_label_0, metric_color, 0);
    lv_obj_set_width(lv_label_0, lv_pct(100));
    lv_obj_add_style(lv_label_0, &style_metric, 0);

    LV_TRACE_OBJ_CREATE("finished");

    return lv_button_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


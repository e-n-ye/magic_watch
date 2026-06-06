/**
 * @file health_shortcut_card_gen.h
 */

#ifndef HEALTH_SHORTCUT_CARD_H
#define HEALTH_SHORTCUT_CARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
    #include "lvgl_private.h"
#else
    #include "lvgl/lvgl.h"
    #include "lvgl/lvgl_private.h"
#endif

#ifdef LV_USE_XML
    #include "lv_xml/lv_xml.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * health_shortcut_card_create(lv_obj_t * parent, const char * card_id, const void * icon_src, const char * metric_text, lv_color_t metric_color);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*HEALTH_SHORTCUT_CARD_H*/
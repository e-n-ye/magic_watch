/**
 * @file magic_watch_ui_gen.h
 */

#ifndef MAGIC_WATCH_UI_GEN_H
#define MAGIC_WATCH_UI_GEN_H

#ifndef UI_SUBJECT_STRING_LENGTH
#define UI_SUBJECT_STRING_LENGTH 256
#endif

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

/**
 * Primary watch preview width
 */
#define SCREEN_WIDTH 240

/**
 * Primary watch preview height
 */
#define SCREEN_HEIGHT 280

/**
 * Top safe inset for compact watch layouts
 */
#define SAFE_TOP 12

/**
 * Bottom safe inset for compact watch layouts
 */
#define SAFE_BOTTOM 12

/**
 * Left and right safe inset for compact watch layouts
 */
#define SAFE_SIDE 12

/**
 * Tight internal spacing
 */
#define SPACE_2XS 4

/**
 * Small spacing between related elements
 */
#define SPACE_XS 8

/**
 * Default block spacing on the watch home screen
 */
#define SPACE_SM 12

/**
 * Section spacing for grouped content
 */
#define SPACE_MD 16

/**
 * Large separation between screen regions
 */
#define SPACE_LG 20

/**
 * Small card radius
 */
#define RADIUS_SM 12

/**
 * Primary card radius for 240x280 layouts
 */
#define RADIUS_MD 18

/**
 * Large container radius
 */
#define RADIUS_LG 24

/**
 * Minimum recommended touch target
 */
#define TOUCH_MIN 44

/**
 * Baseline card height for quarter shortcuts
 */
#define CARD_MIN_H 96

/**
 * Primary watch background
 */
#define BG_BASE lv_color_hex(0x08111a)

/**
 * Card and container background
 */
#define BG_SURFACE lv_color_hex(0x122033)

/**
 * Primary foreground text
 */
#define FG_PRIMARY lv_color_hex(0xf4f7fb)

/**
 * Secondary foreground text
 */
#define FG_MUTED lv_color_hex(0x94a4b8)

/**
 * Default accent for health shortcuts
 */
#define ACCENT_HEALTH lv_color_hex(0x4fd1c5)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/*-------------------
 * Permanent screens
 *------------------*/

/*----------------
 * Global styles
 *----------------*/

extern lv_style_t screen_base;
extern lv_style_t surface_card;

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/*----------------
 * Event Callbacks
 *----------------*/

/**
 * Initialize the component library
 */

void magic_watch_ui_init_gen(const char * asset_path);

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*MAGIC_WATCH_UI_GEN_H*/
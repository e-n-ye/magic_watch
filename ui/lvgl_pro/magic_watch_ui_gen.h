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
 * Quarter card width for the 240x280 home preview
 */
#define HEALTH_CARD_W 102

/**
 * Quarter card height for the 240x280 home preview
 */
#define HEALTH_CARD_H 102

/**
 * Gap between the four shortcut cards
 */
#define HEALTH_GRID_GAP 12

/**
 * Top offset of the 2x2 shortcut stage
 */
#define HEALTH_GRID_TOP 30

/**
 * Left inset of the 2x2 shortcut stage
 */
#define HEALTH_GRID_LEFT 12

/**
 * Second column x position for the shortcut stage
 */
#define HEALTH_COL2_X 126

/**
 * Second row y position for the shortcut stage
 */
#define HEALTH_ROW2_Y 144

/**
 * Size of the page indicator dots
 */
#define PAGER_DOT_SIZE 8

/**
 * Primary watch background
 */
#define BG_BASE lv_color_hex(0x08111a)

/**
 * Health shortcut screen background
 */
#define HEALTH_SCREEN_BG lv_color_hex(0x000000)

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

/**
 * Heart rate accent
 */
#define ACCENT_HEART lv_color_hex(0xff6b6b)

/**
 * SpO2 accent
 */
#define ACCENT_OXYGEN lv_color_hex(0x5cc8ff)

/**
 * Breath accent
 */
#define ACCENT_BREATH lv_color_hex(0x7ce38b)

/**
 * Mood accent
 */
#define ACCENT_MOOD lv_color_hex(0xffc857)

/**
 * Dark text used inside accent badges
 */
#define BADGE_TEXT lv_color_hex(0x071018)

/**
 * Dark card background used by most shortcuts
 */
#define CARD_DARK lv_color_hex(0x041a28)

/**
 * Warm highlight card background
 */
#define CARD_RED lv_color_hex(0xff4f7a)

/**
 * Cool highlight card background
 */
#define CARD_BLUE lv_color_hex(0x68d9e9)

/**
 * Active page indicator color
 */
#define PAGER_ACTIVE lv_color_hex(0x82d4ff)

/**
 * Inactive page indicator color
 */
#define PAGER_INACTIVE lv_color_hex(0x35506f)

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
extern lv_style_t screen_title;
extern lv_style_t screen_caption;

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

extern const void * health_heart;
extern const void * health_heart_36;
extern const void * health_heart_64;
extern const void * health_spo2;
extern const void * health_spo2_36;
extern const void * health_spo2_64;
extern const void * health_breathe;
extern const void * health_breathe_36;
extern const void * health_breathe_64;
extern const void * health_stress;
extern const void * health_stress_36;
extern const void * health_stress_64;

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

/*Include all the widgets, components and screens of this library*/
#include "components/health_shortcut_card_gen.h"
#include "screens/screen_health_shortcuts_gen.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*MAGIC_WATCH_UI_GEN_H*/
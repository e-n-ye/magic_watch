/**
 * @file magic_watch_ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "magic_watch_ui_gen.h"

#if LV_USE_XML
#endif /* LV_USE_XML */

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

/*----------------
 * Translations
 *----------------*/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/

/*----------------
 * Images
 *----------------*/

/*----------------
 * Global styles
 *----------------*/

lv_style_t screen_base;
lv_style_t surface_card;

/*----------------
 * Subjects
 *----------------*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void magic_watch_ui_init_gen(const char * asset_path)
{
    char buf[256];


    /*----------------
     * Fonts
     *----------------*/


    /*----------------
     * Images
     *----------------*/
    /*----------------
     * Global styles
     *----------------*/

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&screen_base);
        lv_style_set_bg_color(&screen_base, BG_BASE);
        lv_style_set_text_color(&screen_base, FG_PRIMARY);

        lv_style_init(&surface_card);
        lv_style_set_bg_color(&surface_card, BG_SURFACE);
        lv_style_set_text_color(&surface_card, FG_PRIMARY);
        lv_style_set_radius(&surface_card, RADIUS_MD);

        style_inited = true;
    }

    /*----------------
     * Subjects
     *----------------*/
    /*----------------
     * Translations
     *----------------*/

#if LV_USE_XML
    /* Register widgets */

    /* Register fonts */

    /* Register subjects */

    /* Register callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
#endif

#if LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manaully from XML using lv_xml_create() */
#endif
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/
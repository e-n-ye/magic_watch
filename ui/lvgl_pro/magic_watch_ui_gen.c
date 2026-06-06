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

const void * health_heart;
const void * health_heart_36;
const void * health_heart_64;
const void * health_spo2;
const void * health_spo2_36;
const void * health_spo2_64;
const void * health_breathe;
const void * health_breathe_36;
const void * health_breathe_64;
const void * health_stress;
const void * health_stress_36;
const void * health_stress_64;

/*----------------
 * Global styles
 *----------------*/

lv_style_t screen_base;
lv_style_t surface_card;
lv_style_t screen_title;
lv_style_t screen_caption;

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
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_heart.png");
    health_heart = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_heart_36.png");
    health_heart_36 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_heart_64.png");
    health_heart_64 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_spo2.png");
    health_spo2 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_spo2_36.png");
    health_spo2_36 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_spo2_64.png");
    health_spo2_64 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_breathe.png");
    health_breathe = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_breathe_36.png");
    health_breathe_36 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_breathe_64.png");
    health_breathe_64 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_stress.png");
    health_stress = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_stress_36.png");
    health_stress_36 = lv_strdup(buf);
    lv_snprintf(buf, 256, "%s%s", asset_path, "images/health_stress_64.png");
    health_stress_64 = lv_strdup(buf);

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

        lv_style_init(&screen_title);
        lv_style_set_text_color(&screen_title, FG_PRIMARY);

        lv_style_init(&screen_caption);
        lv_style_set_text_color(&screen_caption, FG_MUTED);

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
    lv_xml_register_image(NULL, "health_heart", health_heart);
    lv_xml_register_image(NULL, "health_heart_36", health_heart_36);
    lv_xml_register_image(NULL, "health_heart_64", health_heart_64);
    lv_xml_register_image(NULL, "health_spo2", health_spo2);
    lv_xml_register_image(NULL, "health_spo2_36", health_spo2_36);
    lv_xml_register_image(NULL, "health_spo2_64", health_spo2_64);
    lv_xml_register_image(NULL, "health_breathe", health_breathe);
    lv_xml_register_image(NULL, "health_breathe_36", health_breathe_36);
    lv_xml_register_image(NULL, "health_breathe_64", health_breathe_64);
    lv_xml_register_image(NULL, "health_stress", health_stress);
    lv_xml_register_image(NULL, "health_stress_36", health_stress_36);
    lv_xml_register_image(NULL, "health_stress_64", health_stress_64);
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
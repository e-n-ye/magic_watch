/******************************************************************************
 * @file    lv_port_indev.c
 * @brief   LVGL input device driver for CST816 touch and encoder
 * @note    V2.0: Added encoder support for dial navigation
 *****************************************************************************/
#include "lv_port_indev.h"
#include "user_config.h"

/* Touch driver */
#if USE_BSP_TOUCH
#include "bsp_touch.h"
#endif

/* Encoder driver */
#if USE_BSP_ENCODER
#include "bsp_encoder.h"
#include "bsp_key.h"
#endif

/* Power management */
#if USE_APP_POWER
#include "app/inc/app_power.h"
#endif

/*--------------------- Input Device Handles ---------------------*/
lv_indev_t *indev_touchpad = NULL;
lv_indev_t *indev_encoder = NULL;
lv_group_t *default_group = NULL;

/*--------------------- Touch Read Callback ---------------------*/

#if USE_BSP_TOUCH
/**
 * @brief  Read touchpad state
 */
static void touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;
    bsp_touch_data_t touch;

    /* Read touch data */
    if (bsp_touch_read(&touch) == 0 && touch.finger_num > 0)
    {
        last_x = touch.x;
        last_y = touch.y;
        data->state = LV_INDEV_STATE_PR;

#if USE_APP_POWER
        /* Notify power module of touch activity (wake from Idle/prevent sleep) */
        app_power_reset_idle_timer();
#endif
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /* Set coordinates */
    data->point.x = last_x;
    data->point.y = last_y;
}
#endif

/*--------------------- Encoder Read Callback ---------------------*/

#if USE_BSP_ENCODER
/**
 * @brief  Read encoder state
 * @note   - Encoder rotation: enc_diff (negative=up/prev, positive=down/next)
 *         - Encoder button (PB2): state (pressed/released)
 *         - Delta is inverted to match LVGL convention (clockwise rotates down list)
 */
static void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    /* Read encoder rotation delta */
    int16_t delta = bsp_encoder_get_delta();

    /* Limit delta to ±1 to ensure smooth focus navigation */
    /* This prevents focus from jumping over multiple items during fast rotation */
    if (delta > 0) {
        data->enc_diff = -1;  /* Clockwise -> next/down (inverted) */
#if USE_APP_POWER
        app_power_reset_idle_timer();  /* Notify power module of encoder rotation */
#endif
    } else if (delta < 0) {
        data->enc_diff = 1;   /* Counter-clockwise -> prev/up (inverted) */
#if USE_APP_POWER
        app_power_reset_idle_timer();  /* Notify power module of encoder rotation */
#endif
    } else {
        data->enc_diff = 0;   /* No rotation */
    }

    /* Read encoder button state (PB2) */
    uint8_t key_state = bsp_key_get_state(KID_ENCODER);
    if (key_state) {
        data->state = LV_INDEV_STATE_PR;
#if USE_APP_POWER
        app_power_reset_idle_timer();  /* Notify power module of button press */
#endif
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}
#endif

/*--------------------- Public Functions ---------------------*/

/**
 * @brief  Initialize LVGL input device drivers
 * @note   Initializes touchpad and encoder input devices
 */
void lv_port_indev_init(void)
{
    /*------------------
     * Touchpad
     *-----------------*/
#if USE_BSP_TOUCH
    static lv_indev_drv_t touchpad_drv;

    /* Initialize touchpad driver */
    lv_indev_drv_init(&touchpad_drv);
    touchpad_drv.type = LV_INDEV_TYPE_POINTER;
    touchpad_drv.read_cb = touchpad_read;

    /* Register touchpad input device */
    indev_touchpad = lv_indev_drv_register(&touchpad_drv);
#endif

    /*------------------
     * Encoder
     *-----------------*/
#if USE_BSP_ENCODER
    static lv_indev_drv_t encoder_drv;

    /* Initialize encoder driver */
    lv_indev_drv_init(&encoder_drv);
    encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_drv.read_cb = encoder_read;

    /* Register encoder input device */
    indev_encoder = lv_indev_drv_register(&encoder_drv);

    /* Create default group for encoder navigation */
    default_group = lv_group_create();
    lv_group_set_default(default_group);

    /* Bind encoder to the default group */
    lv_indev_set_group(indev_encoder, default_group);
#endif
}

/**
 * @brief  Get the default input group
 * @return Pointer to default group, NULL if not available
 */
lv_group_t* lv_port_indev_get_group(void)
{
    return default_group;
}

/**
 * @brief  Add an object to the default group (for encoder control)
 * @param  obj: LVGL object to add
 */
void lv_port_indev_add_to_group(lv_obj_t *obj)
{
    if (default_group != NULL && obj != NULL) {
        lv_group_add_obj(default_group, obj);
    }
}

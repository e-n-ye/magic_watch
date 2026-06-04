/******************************************************************************
 * @file    lv_port_indev.h
 * @brief   LVGL input device driver for touch and encoder
 * @note    V2.0: Added encoder support
 *****************************************************************************/
#ifndef LV_PORT_INDEV_H
#define LV_PORT_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/*--------------------- Input Device Handles ---------------------*/

/* Touch input device handle */
extern lv_indev_t *indev_touchpad;

/* Encoder input device handle */
extern lv_indev_t *indev_encoder;

/* Default group for encoder navigation */
extern lv_group_t *default_group;

/*--------------------- Public Functions ---------------------*/

/**
 * @brief  Initialize LVGL input device drivers
 * @note   Initializes touchpad and encoder input devices
 */
void lv_port_indev_init(void);

/**
 * @brief  Get the default input group
 * @return Pointer to default group, NULL if not available
 */
lv_group_t* lv_port_indev_get_group(void);

/**
 * @brief  Add an object to the default group (for encoder control)
 * @param  obj: LVGL object to add
 */
void lv_port_indev_add_to_group(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_INDEV_H */

/******************************************************************************
 * @file    lv_port_disp.h
 * @brief   LVGL display driver for ST7789 LCD
 *****************************************************************************/
#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
 * @brief  Initialize LVGL display driver
 */
void lv_port_disp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_DISP_H */

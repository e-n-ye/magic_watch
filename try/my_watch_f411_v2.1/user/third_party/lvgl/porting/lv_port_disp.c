/******************************************************************************
 * @file    lv_port_disp.c
 * @brief   LVGL display driver for ST7789 LCD
 *****************************************************************************/
#include "lv_port_disp.h"
#include "bsp_spi_lcd.h"

/*--------------------- Display Buffer Configuration ---------------------*/
/* Memory optimization: single buffer, 40 lines (saves ~50KB RAM) */
#define DISP_BUF_LINES      40                              /* Lines per buffer */
#define DISP_BUF_SIZE       (LCD_WIDTH * DISP_BUF_LINES)    /* 240 x 40 = 9600 pixels */

/*--------------------- Display Buffers ---------------------*/
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf_1[DISP_BUF_SIZE];
/* buf_2 removed - using single buffer mode to save RAM */

/*--------------------- Flush Callback ---------------------*/

/**
 * @brief  Flush display buffer to LCD
 */
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    /* Use BSP LCD DMA transfer */
    bsp_lcd_color_fill_dma(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p);

    /* Inform LVGL that flushing is done */
    lv_disp_flush_ready(drv);
}

/*--------------------- Public Functions ---------------------*/

/**
 * @brief  Initialize LVGL display driver
 */
void lv_port_disp_init(void)
{
    /* Initialize display draw buffer (single buffer mode) */
    lv_disp_draw_buf_init(&draw_buf, buf_1, NULL, DISP_BUF_SIZE);

    /* Initialize display driver */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    /* Set display resolution */
    disp_drv.hor_res = LCD_WIDTH;   /* 240 */
    disp_drv.ver_res = LCD_HEIGHT;  /* 280 */

    /* Set flush callback */
    disp_drv.flush_cb = disp_flush;

    /* Set draw buffer */
    disp_drv.draw_buf = &draw_buf;

    /* Register display driver */
    lv_disp_drv_register(&disp_drv);
}

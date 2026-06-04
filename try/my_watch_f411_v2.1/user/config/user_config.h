#ifndef USER_CONFIG_H
#define USER_CONFIG_H

/*
 * First F411 bring-up slice:
 * - keep CubeMX generated code as the hardware owner;
 * - keep all handwritten code under user/;
 * - enable only LCD bring-up and encoder/key semantic digestion.
 */

#define WATCH_USE_LCD_BRINGUP      1
#define WATCH_USE_INPUT_BRINGUP    1

#define WATCH_LCD_WIDTH            240U
#define WATCH_LCD_HEIGHT           280U

#define WATCH_INPUT_SCAN_PERIOD_MS 10U
#define SENSOR_TASK_BASE_INTERVAL  20U

#endif /* USER_CONFIG_H */

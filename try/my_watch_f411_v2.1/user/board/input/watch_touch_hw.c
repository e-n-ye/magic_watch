#include "watch_touch_hw.h"

#include "main.h"

#define WATCH_TOUCH_I2C_ADDR_WRITE 0x2AU
#define WATCH_TOUCH_I2C_ADDR_READ  0x2BU

#define WATCH_TOUCH_REG_GESTURE    0x01U
#define WATCH_TOUCH_REG_FINGER_NUM 0x02U
#define WATCH_TOUCH_REG_XPOS_H     0x03U
#define WATCH_TOUCH_REG_XPOS_L     0x04U
#define WATCH_TOUCH_REG_YPOS_H     0x05U
#define WATCH_TOUCH_REG_YPOS_L     0x06U

#define WATCH_TOUCH_I2C_DELAY_LOOPS 12U

static uint8_t s_touch_initialized;

static void watch_touch_delay_short(void)
{
    volatile uint32_t i;

    for (i = 0U; i < WATCH_TOUCH_I2C_DELAY_LOOPS; ++i) {
        __NOP();
    }
}

static void watch_touch_sda_high(void)
{
    HAL_GPIO_WritePin(TP_SDA_GPIO_Port, TP_SDA_Pin, GPIO_PIN_SET);
}

static void watch_touch_sda_low(void)
{
    HAL_GPIO_WritePin(TP_SDA_GPIO_Port, TP_SDA_Pin, GPIO_PIN_RESET);
}

static void watch_touch_scl_high(void)
{
    HAL_GPIO_WritePin(TP_SCL_GPIO_Port, TP_SCL_Pin, GPIO_PIN_SET);
}

static void watch_touch_scl_low(void)
{
    HAL_GPIO_WritePin(TP_SCL_GPIO_Port, TP_SCL_Pin, GPIO_PIN_RESET);
}

static GPIO_PinState watch_touch_sda_read(void)
{
    return HAL_GPIO_ReadPin(TP_SDA_GPIO_Port, TP_SDA_Pin);
}

static void watch_touch_sda_input_mode(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.Pin = TP_SDA_Pin;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_SDA_GPIO_Port, &gpio_init);
}

static void watch_touch_sda_output_mode(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.Pin = TP_SDA_Pin;
    gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TP_SDA_GPIO_Port, &gpio_init);
}

static void watch_touch_i2c_start(void)
{
    watch_touch_sda_output_mode();
    watch_touch_sda_high();
    watch_touch_scl_high();
    watch_touch_delay_short();
    watch_touch_sda_low();
    watch_touch_delay_short();
    watch_touch_scl_low();
}

static void watch_touch_i2c_stop(void)
{
    watch_touch_sda_output_mode();
    watch_touch_scl_low();
    watch_touch_sda_low();
    watch_touch_delay_short();
    watch_touch_scl_high();
    watch_touch_delay_short();
    watch_touch_sda_high();
    watch_touch_delay_short();
}

static uint8_t watch_touch_i2c_write_byte(uint8_t value)
{
    uint8_t bit;
    uint8_t ack;

    watch_touch_sda_output_mode();
    for (bit = 0U; bit < 8U; ++bit) {
        if ((value & 0x80U) != 0U) {
            watch_touch_sda_high();
        } else {
            watch_touch_sda_low();
        }
        value <<= 1;
        watch_touch_delay_short();
        watch_touch_scl_high();
        watch_touch_delay_short();
        watch_touch_scl_low();
    }

    watch_touch_sda_input_mode();
    watch_touch_delay_short();
    watch_touch_scl_high();
    watch_touch_delay_short();
    ack = (watch_touch_sda_read() == GPIO_PIN_RESET) ? 1U : 0U;
    watch_touch_scl_low();
    watch_touch_sda_output_mode();
    return ack;
}

static uint8_t watch_touch_i2c_read_byte(uint8_t ack)
{
    uint8_t bit;
    uint8_t value;

    value = 0U;
    watch_touch_sda_input_mode();
    for (bit = 0U; bit < 8U; ++bit) {
        value <<= 1;
        watch_touch_scl_high();
        watch_touch_delay_short();
        if (watch_touch_sda_read() == GPIO_PIN_SET) {
            value |= 0x01U;
        }
        watch_touch_scl_low();
        watch_touch_delay_short();
    }

    watch_touch_sda_output_mode();
    if (ack != 0U) {
        watch_touch_sda_low();
    } else {
        watch_touch_sda_high();
    }
    watch_touch_delay_short();
    watch_touch_scl_high();
    watch_touch_delay_short();
    watch_touch_scl_low();
    watch_touch_sda_high();
    return value;
}

static uint8_t watch_touch_read_registers(uint8_t start_reg, uint8_t *buffer, uint8_t length)
{
    uint8_t i;

    if ((buffer == 0) || (length == 0U)) {
        return 1U;
    }

    watch_touch_i2c_start();
    if (watch_touch_i2c_write_byte(WATCH_TOUCH_I2C_ADDR_WRITE) == 0U) {
        watch_touch_i2c_stop();
        return 1U;
    }
    if (watch_touch_i2c_write_byte(start_reg) == 0U) {
        watch_touch_i2c_stop();
        return 1U;
    }

    watch_touch_i2c_start();
    if (watch_touch_i2c_write_byte(WATCH_TOUCH_I2C_ADDR_READ) == 0U) {
        watch_touch_i2c_stop();
        return 1U;
    }

    for (i = 0U; i < length; ++i) {
        buffer[i] = watch_touch_i2c_read_byte((i + 1U) < length ? 1U : 0U);
    }

    watch_touch_i2c_stop();
    return 0U;
}

void watch_touch_hw_init(void)
{
    if (s_touch_initialized != 0U) {
        return;
    }

    watch_touch_sda_output_mode();
    watch_touch_sda_high();
    watch_touch_scl_high();

    HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(5U);
    HAL_GPIO_WritePin(TP_RST_GPIO_Port, TP_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50U);

    s_touch_initialized = 1U;
}

uint8_t watch_touch_hw_read(watch_touch_sample_t *sample)
{
    uint8_t raw[6];
    uint16_t x;
    uint16_t y;

    if ((s_touch_initialized == 0U) || (sample == 0)) {
        return 1U;
    }

    if (watch_touch_read_registers(WATCH_TOUCH_REG_GESTURE, raw, sizeof(raw)) != 0U) {
        return 1U;
    }

    sample->gesture = (watch_touch_gesture_t)raw[0];
    sample->finger_num = raw[WATCH_TOUCH_REG_FINGER_NUM - WATCH_TOUCH_REG_GESTURE] & 0x0FU;
    x = (uint16_t)(((uint16_t)(raw[WATCH_TOUCH_REG_XPOS_H - WATCH_TOUCH_REG_GESTURE] & 0x0FU) << 8)
        | raw[WATCH_TOUCH_REG_XPOS_L - WATCH_TOUCH_REG_GESTURE]);
    y = (uint16_t)(((uint16_t)(raw[WATCH_TOUCH_REG_YPOS_H - WATCH_TOUCH_REG_GESTURE] & 0x0FU) << 8)
        | raw[WATCH_TOUCH_REG_YPOS_L - WATCH_TOUCH_REG_GESTURE]);

    sample->x = x;
    sample->y = y;
    return 0U;
}

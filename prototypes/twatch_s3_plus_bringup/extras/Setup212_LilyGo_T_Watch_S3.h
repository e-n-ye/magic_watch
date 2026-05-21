// TFT_eSPI setup for LILYGO T-Watch S3 Plus, copied from the official
// TTGO_TWatch_Library reference so this bring-up project stays self-contained.
#define USER_SETUP_ID 212

#define ST7789_DRIVER

#define TFT_WIDTH 240
#define TFT_HEIGHT 240

#define TFT_INVERSION_ON

#define TFT_MISO -1
#define TFT_MOSI 13
#define TFT_SCLK 18
#define TFT_CS 12
#define TFT_DC 38
#define TFT_RST -1

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000


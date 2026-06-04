#include "bsp_w25q128.h"

#include "main.h"
#include "spi.h"

#define W25Q128_CMD_WRITE_ENABLE   0x06U
#define W25Q128_CMD_READ_STATUS    0x05U
#define W25Q128_CMD_READ_DATA      0x03U
#define W25Q128_CMD_PAGE_PROGRAM   0x02U
#define W25Q128_CMD_SECTOR_ERASE   0x20U
#define W25Q128_CMD_READ_JEDEC_ID  0x9FU
#define W25Q128_STATUS_BUSY        0x01U
#define W25Q128_SPI_TIMEOUT_MS     1000U

static void flash_select(void)
{
    HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET);
}

static void flash_unselect(void)
{
    HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);
}

static int flash_tx(const uint8_t *data, uint16_t len)
{
    return (HAL_SPI_Transmit(&hspi3, (uint8_t *)data, len, W25Q128_SPI_TIMEOUT_MS) == HAL_OK) ? 0 : -1;
}

static int flash_rx(uint8_t *data, uint16_t len)
{
    return (HAL_SPI_Receive(&hspi3, data, len, W25Q128_SPI_TIMEOUT_MS) == HAL_OK) ? 0 : -1;
}

static uint8_t flash_read_status(void)
{
    uint8_t cmd = W25Q128_CMD_READ_STATUS;
    uint8_t status = 0U;

    flash_select();
    (void)flash_tx(&cmd, 1U);
    (void)flash_rx(&status, 1U);
    flash_unselect();

    return status;
}

static int flash_wait_ready(uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();

    while ((flash_read_status() & W25Q128_STATUS_BUSY) != 0U) {
        if ((HAL_GetTick() - start) > timeout_ms) {
            return -1;
        }
    }

    return 0;
}

static int flash_write_enable(void)
{
    uint8_t cmd = W25Q128_CMD_WRITE_ENABLE;

    flash_select();
    if (flash_tx(&cmd, 1U) != 0) {
        flash_unselect();
        return -1;
    }
    flash_unselect();
    return 0;
}

int bsp_w25q128_init(void)
{
    uint32_t id = 0U;

    flash_unselect();
    if (bsp_w25q128_read_id(&id) != 0) {
        return -1;
    }

    return (id == W25Q128_JEDEC_ID) ? 0 : -1;
}

int bsp_w25q128_read_id(uint32_t *id)
{
    uint8_t cmd = W25Q128_CMD_READ_JEDEC_ID;
    uint8_t data[3] = {0U, 0U, 0U};

    if (id == 0) {
        return -1;
    }

    flash_select();
    if ((flash_tx(&cmd, 1U) != 0) || (flash_rx(data, 3U) != 0)) {
        flash_unselect();
        return -1;
    }
    flash_unselect();

    *id = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | (uint32_t)data[2];
    return 0;
}

int bsp_w25q128_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t cmd[4];
    uint32_t offset = 0U;

    if ((data == 0) || (len == 0U) || ((addr + len) > W25Q128_CAPACITY)) {
        return -1;
    }

    if (flash_wait_ready(1000U) != 0) {
        return -1;
    }

    cmd[0] = W25Q128_CMD_READ_DATA;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >> 8);
    cmd[3] = (uint8_t)(addr & 0xFFU);

    flash_select();
    if (flash_tx(cmd, 4U) != 0) {
        flash_unselect();
        return -1;
    }

    while (offset < len) {
        uint16_t chunk = (uint16_t)(((len - offset) > 0xFFFFU) ? 0xFFFFU : (len - offset));

        if (flash_rx(&data[offset], chunk) != 0) {
            flash_unselect();
            return -1;
        }

        offset += chunk;
    }

    flash_unselect();

    return 0;
}

int bsp_w25q128_erase_sector(uint32_t addr)
{
    uint8_t cmd[4];

    if ((addr >= W25Q128_CAPACITY) || ((addr % W25Q128_SECTOR_SIZE) != 0U)) {
        return -1;
    }

    if ((flash_wait_ready(1000U) != 0) || (flash_write_enable() != 0)) {
        return -1;
    }

    cmd[0] = W25Q128_CMD_SECTOR_ERASE;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >> 8);
    cmd[3] = (uint8_t)(addr & 0xFFU);

    flash_select();
    if (flash_tx(cmd, 4U) != 0) {
        flash_unselect();
        return -1;
    }
    flash_unselect();

    return flash_wait_ready(1000U);
}

int bsp_w25q128_write_page(uint32_t addr, const uint8_t *data, uint16_t len)
{
    uint8_t cmd[4];

    if ((data == 0) || (len == 0U) || (len > W25Q128_PAGE_SIZE) || ((addr + len) > W25Q128_CAPACITY)) {
        return -1;
    }

    if ((flash_wait_ready(1000U) != 0) || (flash_write_enable() != 0)) {
        return -1;
    }

    cmd[0] = W25Q128_CMD_PAGE_PROGRAM;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >> 8);
    cmd[3] = (uint8_t)(addr & 0xFFU);

    flash_select();
    if ((flash_tx(cmd, 4U) != 0) || (flash_tx(data, len) != 0)) {
        flash_unselect();
        return -1;
    }
    flash_unselect();

    return flash_wait_ready(20U);
}

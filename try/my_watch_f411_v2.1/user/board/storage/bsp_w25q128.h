#ifndef BSP_W25Q128_H
#define BSP_W25Q128_H

#include <stdint.h>

#define W25Q128_PAGE_SIZE    256U
#define W25Q128_SECTOR_SIZE  4096U
#define W25Q128_CAPACITY     (16UL * 1024UL * 1024UL)
#define W25Q128_JEDEC_ID     0xEF4018UL

int bsp_w25q128_init(void);
int bsp_w25q128_read_id(uint32_t *id);
int bsp_w25q128_read(uint32_t addr, uint8_t *data, uint32_t len);
int bsp_w25q128_erase_sector(uint32_t addr);
int bsp_w25q128_write_page(uint32_t addr, const uint8_t *data, uint16_t len);

#endif /* BSP_W25Q128_H */

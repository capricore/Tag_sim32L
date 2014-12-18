#ifndef FLASHCTRL_H
#define FLASHCTRL_H

#include "SI32_CLKCTRL_A_Type.h"
#include "SI32_VMON_B_Type.h"
#include "SI32_FLASHCTRL_A_Type.h"
#include "SI32_RSTSRC_A_Type.h"

#define PAGE_MASK 0xfffffc00
#define FLASH_PAGE_SIZE 1024

void read_from_flash_to_buff(uint32_t addr, uint8_t *buff, uint16_t len);

void write_data_to_flash(uint32_t addr,  uint8_t *buff, uint16_t len);

void write_to_flash_from_buff(uint32_t addr,  uint8_t *buff, uint16_t len);

void erase_flash(uint32_t addr, uint16_t pages, uint16_t value);

#endif


#ifndef SYSPARAM_OPTION_H
#define SYSPARAM_OPTION_H

#include <stdint.h>

#define PARAM_BASE_ADDR 0X7C00

void set_sysparam(uint32_t addr, uint8_t *pparam, uint16_t len);

void copy_sysparam_to_buff(void);

void set_sysparam_to_buff(uint32_t addr, uint8_t *pparam, uint16_t len);

void copy_sysparam_to_flash(void);

void get_sysparam(uint32_t addr, uint8_t *pparam, uint16_t len);

#endif


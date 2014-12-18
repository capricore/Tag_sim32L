#include "sysparam_include.h" 
#include "arch_include.h"

uint8_t param_temp_buff[FLASH_PAGE_SIZE];

void set_sysparam(uint32_t addr, uint8_t *pparam, uint16_t len)
{
	read_from_flash_to_buff(PARAM_BASE_ADDR, param_temp_buff, FLASH_PAGE_SIZE);
	while(len--)
	{
		param_temp_buff[addr] = *pparam++;
	}
	write_data_to_flash(PARAM_BASE_ADDR, param_temp_buff, FLASH_PAGE_SIZE);
}

void copy_sysparam_to_buff(void)
{
	read_from_flash_to_buff(PARAM_BASE_ADDR, param_temp_buff, FLASH_PAGE_SIZE);
}

void set_sysparam_to_buff(uint32_t addr, uint8_t *pparam, uint16_t len)
{
	while(len--)
	{
		param_temp_buff[addr++] = *pparam++;
	}
}

void copy_sysparam_to_flash(void)
{
	write_data_to_flash(PARAM_BASE_ADDR, param_temp_buff, FLASH_PAGE_SIZE);
}

void get_sysparam(uint32_t addr, uint8_t *pparam, uint16_t len)
{
	addr += PARAM_BASE_ADDR;
	read_from_flash_to_buff(addr, pparam, len);
}




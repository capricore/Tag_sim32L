#ifndef __FLASH_OPERATION_H
#define __FLASH_OPERATION_H

#ifdef  __cplusplus
	extern "C" {
#endif

#include <stdint.h>
		
#define SYS_INFO_SIZE  				((uint32_t)20)   // uint16_t
#define SYS_INFO_START_ADDR			((uint32_t)0x0000f800)
#define ERASE_TIMEOUT_MS			((uint32_t)5000) //ms
#define PROGRAM_WORD_TIMEOUT_MS		((uint32_t)1000) //ms

void write_sys_info_to_flash(uint16_t * sys_info_buf);

void read_parameter_from_flash(uint16_t * buf);
		
#ifdef  __cplusplus
	extern }
#endif
	
#endif //__FLASH_OPERATION_H

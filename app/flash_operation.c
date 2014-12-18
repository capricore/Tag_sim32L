#include "flash_operation.h"
#include "SI32_FLASHCTRL_A_Type.h"
#include "FLASHCTRL.h"

extern uint32_t write_flash_time_out;

void write_sys_info_to_flash(uint16_t * sys_info_buf) {
//	uint32_t i;
//	uint32_t sys_info_size = SYS_INFO_SIZE;
//	uint32_t sys_info_start_addr = SYS_INFO_START_ADDR;
//	uint16_t sys_info_data_temp = 0;
	write_data_to_flash(SYS_INFO_START_ADDR,  (uint8_t *)sys_info_buf, SYS_INFO_SIZE*2);

// 	FLASH_Unlock();
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

// 	write_flash_time_out = 0;
// 	while(1) {
// 		if (write_flash_time_out >= ERASE_TIMEOUT_MS) {
// 			return ;
// 		}
// 		if (FLASH_ErasePage(sys_info_start_addr) == FLASH_COMPLETE) {
// 			break;
// 		}
// 	}
// 	
// 	write_flash_time_out = 0;
// 	for(i=0; i<sys_info_size; i++) {
// 		sys_info_data_temp = *(sys_info_buf+i);
// 		while(1) {
// 			if (write_flash_time_out >= PROGRAM_WORD_TIMEOUT_MS) {
// 				return ;
// 			}
// 			if (FLASH_ProgramHalfWord(sys_info_start_addr+2*i, sys_info_data_temp) == FLASH_COMPLETE) {
// 				break;
// 			}
// 		}
// 	}

// 	FLASH_Lock();
	return ;
}

void read_parameter_from_flash(uint16_t * buf) {
	
	int i;

	for(i=0; i<SYS_INFO_SIZE; i++) {
		*(buf+i) = *(uint16_t*)(SYS_INFO_START_ADDR+i*2);
	}
}

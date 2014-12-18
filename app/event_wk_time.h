#ifndef __EVENT_WK_TIME_H
#define __EVENT_WK_TIME_H

#ifdef  __cplusplus
	extern "C" {
#endif 
		
#include <stdint.h>

void set_next_wake_time(uint8_t period);
void send_cave_pkt(uint8_t seq, uint16_t dest_panID, uint16_t dest_addr);
void send_en_pkt(void);
void send_hrt_pkt(void);
void send_ex_pkt(void);
		
#ifdef  __cplusplus
	extern }
#endif

#endif //__EVENT_WK_TIME_H

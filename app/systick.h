#ifndef __SYSTICK_H
#define __SYSTICK_H

#ifdef  __cplusplus
	extern "C" {
#endif

#include "rf2xx_include.h"
		

void Systick_Init(void);
void Systick_off(void);
void delay_init(uint8_t sys_clk);

void delay_us(uint32_t t);
void delay_ms(uint32_t t);
void delay_s(uint32_t t);
		
#ifdef  __cplusplus
	extern }
#endif

#endif //__SYSTICK_H

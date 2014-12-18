#ifndef __GCPU_H__
#define __GCPU_H__

#include <stdint.h>

// MODE FUNCTIONS
void cpu_update(void);

// SUPPORT FUNCTIONS
void set_ahb_clock(uint32_t freq);
uint32_t get_msTicks(void);
void halt(void);

// NVIC table location
#define si32McuOption_map_vectors_to_ram 0

// APB Clock Frequency in Hz
extern uint32_t SystemPeripheralClock;

#if si32McuOption_map_vectors_to_ram
void (*si32_nvic_table[SIM3U1XX_MCU_NVIC_COUNT])(void);
#define si32McuOption_vector_ram_base si32_nvic_table

#endif

#endif 


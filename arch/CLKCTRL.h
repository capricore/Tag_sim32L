#ifndef __GCLKCTRL_H__
#define __GCLKCTRL_H__

#include <stdint.h>
/*
000: AHB clock source isthe Low-Power Oscillator.
001: AHB clock source is the Low-Frequency Oscillator.
010: AHB clock source is the RTC0TCLK signal.
011: AHB clock source isthe External Oscillator.
100: AHB clock source is the VIORFCLK input pin.
101: AHB clock source is the PLL.
110: AHB clock source is a divided version of the Low-Power Oscillator
*/
#define CLOCK_LOW_POWER_OSC			0
#define CLOCK_LOW_FREQUENCY_OSC		1
#define CLOCK_RTC0					2
#define CLOCK_EXTERNAL_OSC			3
#define CLOCK_VIORFCLK				4
#define CLOCK_PLL					5
#define CLOCK_DIVIDED_LOW_POWER_OSC	6


#define SYSTEMCORECLOCK_20M		20000000
#define SYSTEMCORECLOCK_10M		10000000
#define SYSTEMCORECLOCK_5M		5000000
#define SYSTEMCORECLOCK_2_5M	2500000
#define SYSTEMCORECLOCK_1_25M	1250000

#define SYSTEMCORECLOCK_16M		16000000
#define SYSTEMCORECLOCK_8M		8000000
#define SYSTEMCORECLOCK_4M		4000000
#define SYSTEMCORECLOCK_2M		2000000
#define SYSTEMCORECLOCK_1M		1000000

/*
000: AHB clock divided by 1.
001: AHB clock divided by 2.
010: AHB clock divided by 4.
011: AHB clock divided by 8.
100: AHB clock divided by 16.
101: AHB clock divided by 32.
110: AHB clock divided by 64.
111: AHB clock divided by 128
*/
#define AHB_CLOCK_DEVIDED_1		0
#define AHB_CLOCK_DEVIDED_2		1
#define AHB_CLOCK_DEVIDED_4		2
#define AHB_CLOCK_DEVIDED_8		3
#define AHB_CLOCK_DEVIDED_16	4
#define AHB_CLOCK_DEVIDED_32	5
#define AHB_CLOCK_DEVIDED_64	6
#define AHB_CLOCK_DEVIDED_128	7



#define SYSTEMPERIPHCLOCK_20M 	20000000 
#define SYSTEMPERIPHCLOCK_10M 	10000000 
#define SYSTEMPERIPHCLOCK_5M 	5000000 
#define SYSTEMPERIPHCLOCK_2_5M 	2500000
#define SYSTEMPERIPHCLOCK_1_25M 	1250000
#define SYSTEMPERIPHCLOCK_16M		16000000
#define SYSTEMPERIPHCLOCK_8M		8000000
#define SYSTEMPERIPHCLOCK_4M		4000000
#define SYSTEMPERIPHCLOCK_2M		2000000
#define SYSTEMPERIPHCLOCK_1M		1000000


#define PERIPH_DIVIDER1 	1
#define PERIPH_DIVIDER2		2


typedef struct system_core_clock_t
{
	uint32_t clk_source;
	uint32_t core_clock;
}system_core_clock;

typedef struct system_periph_clock_t
{
	uint32_t 	divider;
	uint32_t	apb_clock;
}system_periph_clock;

void sysclk_init_from_reset(void);

void set_system_clock(uint32_t clk_source, uint32_t clock);

uint32_t get_system_clock(void) ;

int set_periph_clock(uint32_t divider);

uint32_t get_periph_clock(void);


#endif 


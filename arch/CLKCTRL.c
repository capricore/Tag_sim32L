#include "ClkCtrl.h"
#include "CPU.h"
#include <SI32_CLKCTRL_A_Type.h>
#include <si32_device.h>

system_core_clock system_clock;
system_periph_clock periph_clock;

uint32_t extosc_clock_base = 16000000;

void sysclk_init_from_reset(void)
{
	set_system_clock(CLOCK_LOW_POWER_OSC, SYSTEMCORECLOCK_20M);
	set_periph_clock(PERIPH_DIVIDER2);
}

void set_system_clock(uint32_t clk_source, uint32_t clock)
{
	switch(clk_source)
	{
		case CLOCK_LOW_POWER_OSC:
			SI32_CLKCTRL_A_select_ahb_source_low_power_oscillator(SI32_CLKCTRL_0);
			switch(clock)
			{
				case SYSTEMCORECLOCK_20M:
					SystemCoreClock = SYSTEMCORECLOCK_20M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);					
					break;
				case SYSTEMCORECLOCK_10M:
					SystemCoreClock = SYSTEMCORECLOCK_10M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_2);					
					break;
				case SYSTEMCORECLOCK_5M:
					SystemCoreClock = SYSTEMCORECLOCK_5M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_4);						
					break;
				case SYSTEMCORECLOCK_2_5M:
					SystemCoreClock = SYSTEMCORECLOCK_2_5M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_8);						
					break;
				case SYSTEMCORECLOCK_1_25M:
					SystemCoreClock = SYSTEMCORECLOCK_1_25M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_16);						
					break;					
				default:
					SystemCoreClock = SYSTEMCORECLOCK_20M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);	
					break;
			}
			
			break;
		case CLOCK_LOW_FREQUENCY_OSC:			
			SI32_CLKCTRL_A_select_ahb_source_low_frequency_oscillator(SI32_CLKCTRL_0);
			switch(clock)
			{
				case SYSTEMCORECLOCK_2_5M:
					SystemCoreClock = SYSTEMCORECLOCK_2_5M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);						
					break;
				case SYSTEMCORECLOCK_1_25M:
					SystemCoreClock = SYSTEMCORECLOCK_1_25M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_2);						
					break;					
				default:
					SystemCoreClock = SYSTEMCORECLOCK_2_5M;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);	
					break;
			}			
			break;
		case CLOCK_RTC0:
			SI32_CLKCTRL_A_select_ahb_source_rtc0tclk(SI32_CLKCTRL_0);
			break;
		case CLOCK_EXTERNAL_OSC:
			SI32_CLKCTRL_A_select_ahb_source_external_oscillator(SI32_CLKCTRL_0);
			switch(clock)
			{
				case SYSTEMCORECLOCK_16M:
					switch(extosc_clock_base)
					{
						case SYSTEMCORECLOCK_16M:
							SystemCoreClock = SYSTEMCORECLOCK_16M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);							
							break;
						case SYSTEMCORECLOCK_8M:
							SystemCoreClock = SYSTEMCORECLOCK_8M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
						default:
							SystemCoreClock = SYSTEMCORECLOCK_16M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
					}						
					break;
				case SYSTEMCORECLOCK_8M:
					switch(extosc_clock_base)
					{
						case SYSTEMCORECLOCK_16M:
							SystemCoreClock = SYSTEMCORECLOCK_8M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_2);							
							break;
						case SYSTEMCORECLOCK_8M:
							SystemCoreClock = SYSTEMCORECLOCK_8M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
						default:
							SystemCoreClock = SYSTEMCORECLOCK_8M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
					}										
					break;
				case SYSTEMCORECLOCK_4M:
					switch(extosc_clock_base)
					{
						case SYSTEMCORECLOCK_16M:
							SystemCoreClock = SYSTEMCORECLOCK_4M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_4);							
							break;
						case SYSTEMCORECLOCK_8M:
							SystemCoreClock = SYSTEMCORECLOCK_4M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_2);								
							break;
						default:
							SystemCoreClock = SYSTEMCORECLOCK_4M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
					}						
					break;		
				case SYSTEMCORECLOCK_2M:
					switch(extosc_clock_base)
					{
						case SYSTEMCORECLOCK_16M:
							SystemCoreClock = SYSTEMCORECLOCK_2M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_8);							
							break;
						case SYSTEMCORECLOCK_8M:
							SystemCoreClock = SYSTEMCORECLOCK_2M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_4);								
							break;
						default:
							SystemCoreClock = SYSTEMCORECLOCK_2M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
					}					
					break;	
				case SYSTEMCORECLOCK_1M:
					switch(extosc_clock_base)
					{
						case SYSTEMCORECLOCK_16M:
							SystemCoreClock = SYSTEMCORECLOCK_1M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_16);							
							break;
						case SYSTEMCORECLOCK_8M:
							SystemCoreClock = SYSTEMCORECLOCK_1M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_8);								
							break;
						default:
							SystemCoreClock = SYSTEMCORECLOCK_1M;
							SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);								
							break;
					}					
					break;						
				default:
					SystemCoreClock = extosc_clock_base;
					SI32_CLKCTRL_A_select_ahb_divider(SI32_CLKCTRL_0, AHB_CLOCK_DEVIDED_1);	
					break;
			}			
			break;			
		case CLOCK_VIORFCLK:
			SI32_CLKCTRL_A_select_ahb_source_viorfclock(SI32_CLKCTRL_0);
			break;
		case CLOCK_PLL:
			SI32_CLKCTRL_A_select_ahb_source_pll(SI32_CLKCTRL_0);
			break;
		case CLOCK_DIVIDED_LOW_POWER_OSC:
			SI32_CLKCTRL_A_select_ahb_source_divided_low_power_oscillator(SI32_CLKCTRL_0);
			break;
		default:
			break;

	}
	
	system_clock.clk_source = clk_source;
	system_clock.core_clock = SystemCoreClock;	
}



uint32_t get_system_clock(void) 
{
	return system_clock.core_clock;
}


int set_periph_clock(uint32_t divider)
{
	int ret = 0;
	switch(divider)
	{
		case PERIPH_DIVIDER1:
			periph_clock.apb_clock = SystemCoreClock;
			SystemPeripheralClock = SystemCoreClock;
			SI32_CLKCTRL_A_select_apb_divider_1(SI32_CLKCTRL_0);
			break;
		case PERIPH_DIVIDER2:			
			periph_clock.apb_clock = (SystemCoreClock >> 1);
			SystemPeripheralClock = (SystemCoreClock >> 1);
			SI32_CLKCTRL_A_select_apb_divider_2(SI32_CLKCTRL_0);
			break;
		default:
			periph_clock.apb_clock = SystemCoreClock;
			SI32_CLKCTRL_A_select_apb_divider_1(SI32_CLKCTRL_0);
			ret = -1;
			break;
	}
	return ret;
}

uint32_t get_periph_clock(void)
{
	return periph_clock.apb_clock;
}


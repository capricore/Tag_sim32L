#include "exciter_include.h"

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

#define WAVE_125K	1
#define WAVE_4K	2

uint16_t get_wave_reload_value(uint32_t fclk)
{
	uint16_t reload_value;
	uint32_t apb_clk = get_periph_clock();
	switch(fclk)
	{
		case WAVE_125K:
			switch(apb_clk)
			{
				case SYSTEMPERIPHCLOCK_20M:
					reload_value = 0xffff - 80;
					break;
				case SYSTEMPERIPHCLOCK_10M:
					reload_value = 0xffff - 40;
					break;
				case SYSTEMPERIPHCLOCK_5M:
					reload_value = 0xffff - 20;
					break;
				case SYSTEMPERIPHCLOCK_2_5M:
					reload_value = 0xffff - 10;
					break;			
				case SYSTEMPERIPHCLOCK_1_25M:
					reload_value = 0xffff - 5;
					break;
				case SYSTEMPERIPHCLOCK_16M:
					reload_value = 0xffff - 64;
					break;
				case SYSTEMPERIPHCLOCK_8M:
					reload_value = 0xffff - 32;
					break;
				case SYSTEMPERIPHCLOCK_4M:
					reload_value = 0xffff - 16;
					break;
				case SYSTEMPERIPHCLOCK_2M:
					reload_value = 0xffff - 8;
					break;			
				case SYSTEMPERIPHCLOCK_1M:
					reload_value = 0xffff - 4;
					break;	
				default:
					break;
						
			}			
			break;
		case WAVE_4K:
			switch(apb_clk)
			{
				case SYSTEMPERIPHCLOCK_20M:
					reload_value = 0xffff - 4880;
					break;
				case SYSTEMPERIPHCLOCK_10M:
					reload_value = 0xffff - 2440;
					break;
				case SYSTEMPERIPHCLOCK_5M:
					reload_value = 0xffff - 1220;
					break;
				case SYSTEMPERIPHCLOCK_2_5M:
					reload_value = 0xffff - 610;
					break;			
				case SYSTEMPERIPHCLOCK_1_25M:
					reload_value = 0xffff - 305;
					break;
				case SYSTEMPERIPHCLOCK_16M:
					reload_value = 0xffff - 3904;
					break;
				case SYSTEMPERIPHCLOCK_8M:
					reload_value = 0xffff - 1952;
					break;
				case SYSTEMPERIPHCLOCK_4M:
					reload_value = 0xffff - 976;
					break;
				case SYSTEMPERIPHCLOCK_2M:
					reload_value = 0xffff - 488;
					break;			
				case SYSTEMPERIPHCLOCK_1M:
					reload_value = 0xffff - 244;
					break;	
				default:
					break;

			}
			break;
		default:
			break;
	}
	return reload_value;
}

void wave_125k_init(void)
{
	SI32_TIMER_A_Type* SI32_TIMER = SI32_TIMER_1;
	uint16_t reload_value = get_wave_reload_value(WAVE_125K);
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_TIMER1);
	
	SI32_TIMER_A_select_square_wave_output_mode(SI32_TIMER);
	SI32_TIMER_A_disable_high_overflow_interrupt(SI32_TIMER);
	SI32_TIMER_A_disable_high_extra_interrupt(SI32_TIMER);
	SI32_TIMER_A_enable_stall_in_debug_mode(SI32_TIMER);
	SI32_TIMER_A_select_high_clock_source_apb_clock(SI32_TIMER);
	SI32_TIMER_A_select_split_timer_mode(SI32_TIMER);
	
	SI32_TIMER_A_set_high_count(SI32_TIMER,reload_value);
	SI32_TIMER_A_set_high_reload(SI32_TIMER,reload_value);

	SI32_TIMER_A_clear_high_overflow_interrupt(SI32_TIMER);
	SI32_PBCFG_A_enable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_TMR1EXEN);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0100);	
}

void wave_4k_init(void)
{
	SI32_TIMER_A_Type* SI32_TIMER = SI32_TIMER_1;
	uint16_t reload_value = get_wave_reload_value(WAVE_4K);
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_TIMER1);
	
	SI32_TIMER_A_select_low_auto_reload_mode(SI32_TIMER);
	SI32_TIMER_A_enable_low_overflow_interrupt(SI32_TIMER);
	SI32_TIMER_A_disable_low_extra_interrupt(SI32_TIMER);
	SI32_TIMER_A_select_low_clock_source_apb_clock(SI32_TIMER);
	SI32_TIMER_A_set_low_count(SI32_TIMER,reload_value);
	SI32_TIMER_A_set_low_reload(SI32_TIMER,reload_value);
	
	SI32_TIMER_A_clear_low_overflow_interrupt(SI32_TIMER);
	NVIC_ClearPendingIRQ( TIMER1L_IRQn  );
	NVIC_EnableIRQ( TIMER1L_IRQn  );
}

void wave_125k_start(void)
{	
	SI32_PBCFG_A_enable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_TMR1EXEN);
	SI32_TIMER_A_start_high_timer(SI32_TIMER_1);
}

void wave_125k_stop(void)
{
	SI32_TIMER_A_stop_high_timer(SI32_TIMER_1);
	SI32_PBCFG_A_disable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_TMR1EXEN);
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_0, 0x0100);
	
}

// void wave_125k_start(void)
// {	
// 	SI32_TIMER_A_stop_high_timer(SI32_TIMER_1);
// 	SI32_PBCFG_A_disable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_TMR1EXEN);
// 	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_0, 0x0100);
// }

// void wave_125k_stop(void)
// {
// 	SI32_PBCFG_A_enable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_TMR1EXEN);
// 	SI32_TIMER_A_start_high_timer(SI32_TIMER_1);	
// 	
// }
void wave_4k_start(void)
{
	SI32_TIMER_A_start_low_timer(SI32_TIMER_1);
}

void wave_4k_stop(void)
{
	SI32_TIMER_A_stop_low_timer(SI32_TIMER_1);
}


uint32_t exciter_cnt = 0;
void TIMER1L_IRQHandler(void)
{
	SI32_TIMER_A_clear_low_overflow_interrupt(SI32_TIMER_1);
	NVIC_ClearPendingIRQ( TIMER1L_IRQn  );
	
	exciter_cnt++;

	switch(exciter_state)
	{
		case EXCITER_IDLE:
			break;
		case EXCITER_BURST:
			if(burst_cnt < CARRIER_BURST_LEN)
			{
				wave_125k_start();
				burst_cnt++;
			}
			else
			{
				wave_125k_stop();
				burst_cnt = 0;
				preamble_cnt = 0;
				exciter_state = EXCITER_PREAMBLE;
			}
			break;
		case EXCITER_PREAMBLE:
			if(preamble_cnt < PREAMBLE_LEN-1)
			{
				if(preamble_cnt&0x01)
				{
					wave_125k_stop();
				}
				else
				{
					wave_125k_start();
				}
				preamble_cnt++;
			}
			else
			{
				if(preamble_cnt&0x01)
				{
					wave_125k_stop();
				}
				else
				{
					wave_125k_start();
				}				
				preamble_cnt = 0;
				pattern_cnt = 0;
				pattern_temp = pattern_data;
				exciter_state = EXCITER_PATTERN;
				
			}
			
			break;
		case EXCITER_PATTERN:
			
			if(pattern_cnt < PATTERN_LEN-1)
			{
				if(pattern_temp&0x8000)
				{
					wave_125k_start();
				}
				else
				{
					wave_125k_stop();
				}
				pattern_temp <<= 1;
				pattern_cnt++;
			}
			else
			{
				if(pattern_temp&0x8000)
				{
					wave_125k_start();
				}
				else
				{
					wave_125k_stop();
				}		
				pattern_cnt = 0;
				exciter_data_cnt = 0;
				exciter_send_index = 0;
				exciter_data_len = (exciter_man_data.data_len)<<3;
				p_exciter_send_buff = exciter_man_data.data;
				exciter_state = EXCITER_DATA;				
			}
			break;
		case EXCITER_DATA:
			if(exciter_data_cnt < exciter_data_len)
			{
				if(!(exciter_data_cnt & 0x07))
					exciter_send_temp = *p_exciter_send_buff++;
				if(exciter_send_temp&EXCITER_MASK)
				{
					wave_125k_start();
				}
				else
				{
					wave_125k_stop();
				}
				exciter_data_cnt++;
				exciter_send_temp <<= 1;
				
			}
			else
			{
				wave_125k_stop();
				wave_4k_stop();
				exciter_state = EXCITER_IDLE;
				exciter_over_event = 1;
			}
			break;
	}
}





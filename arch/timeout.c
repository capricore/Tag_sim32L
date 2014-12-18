#include "arch_include.h"


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
uint8_t clkdiv_us;
uint8_t clkdiv_ms;

uint16_t factor_us;
uint16_t factor_ms;

timeout_item us_timeout_array[US_TIMEOUT_ARRAY_LEN];
timeout_item ms_timeout_array[MS_TIMEOUT_ARRAY_LEN];
uint16_t timeout_current_us_time = 0;
uint16_t timeout_current_ms_time = 0;
uint8_t us_timeout_index;
uint8_t ms_timeout_index;
	
void timeout_init(void)
{
	uint32_t clock = get_periph_clock();
	switch (clock)
	{
		case SYSTEMPERIPHCLOCK_20M:
			clkdiv_ms = 56;
			factor_ms = 100;
			clkdiv_us = 56;
			factor_us = 10;
			break;
		case SYSTEMPERIPHCLOCK_10M:
			clkdiv_ms = 56;
			factor_ms = 50;
			clkdiv_us = 56;
			factor_us = 20; 		
			break;
		case SYSTEMPERIPHCLOCK_5M:
			clkdiv_ms = 16;
			factor_ms = 20;
			clkdiv_us = 16;
			factor_us = 50; 				
			break;
		case SYSTEMPERIPHCLOCK_2_5M:
			clkdiv_ms = 16;
			factor_ms = 10;
			clkdiv_us = 16;
			factor_us = 100;				
			break;
		case SYSTEMPERIPHCLOCK_1_25M:
			clkdiv_ms = 16;
			factor_ms = 5;
			clkdiv_us = 16;
			factor_us = 200;			
			break;
		case SYSTEMPERIPHCLOCK_16M:
			clkdiv_ms = 96;
			factor_ms = 100;
			clkdiv_us = 96;
			factor_us = 10; 		
			break;
		case SYSTEMPERIPHCLOCK_8M:
			clkdiv_ms = 96;
			factor_ms = 50;
			clkdiv_us = 96;
			factor_us = 20; 			
			break;
		case SYSTEMPERIPHCLOCK_4M:
			clkdiv_ms = 56;
			factor_ms = 20;
			clkdiv_us = 56;
			factor_us = 50; 			
			break;
		case SYSTEMPERIPHCLOCK_2M:
			clkdiv_ms = 56;
			factor_ms = 10;
			clkdiv_us = 56;
			factor_us = 100;				
			break;
		case SYSTEMPERIPHCLOCK_1M:
			clkdiv_ms = 56;
			factor_ms = 5;
			clkdiv_us = 56;
			factor_us = 200;			
			break;
		default:
			break;

	}
	us_timer_init();
	ms_timer_init();
	timeout_item_init();
	
}

void us_timer_init(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_TIMER0);	
	SI32_TIMER_A_select_high_auto_reload_mode(SI32_TIMER_0);
	SI32_TIMER_A_enable_high_overflow_interrupt(SI32_TIMER_0);
	SI32_TIMER_A_disable_high_extra_interrupt(SI32_TIMER_0);
	SI32_TIMER_A_enable_stall_in_debug_mode(SI32_TIMER_0);
	SI32_TIMER_A_select_high_clock_source_timer_clock_divider(SI32_TIMER_0);
	
	SI32_TIMER_A_select_split_timer_mode(SI32_TIMER_0);
	SI32_TIMER_A_set_clock_divider_counter(SI32_TIMER_0,clkdiv_us);
	SI32_TIMER_A_set_clock_divider_reload(SI32_TIMER_0,clkdiv_us);

	SI32_TIMER_A_set_high_count(SI32_TIMER_0,0);
	SI32_TIMER_A_set_high_reload(SI32_TIMER_0,0x100);
//	SI32_TIMER_A_start_high_timer(SI32_TIMER_0);
	SI32_TIMER_A_stop_high_timer(SI32_TIMER_0);	
	SI32_TIMER_A_clear_high_overflow_interrupt(SI32_TIMER_0);	
	NVIC_ClearPendingIRQ( TIMER0H_IRQn  );
	NVIC_EnableIRQ( TIMER0H_IRQn  );	
}

void ms_timer_init(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_TIMER0);	
	SI32_TIMER_A_select_low_auto_reload_mode(SI32_TIMER_0);
	SI32_TIMER_A_enable_low_overflow_interrupt(SI32_TIMER_0);
	SI32_TIMER_A_disable_low_extra_interrupt(SI32_TIMER_0);
	SI32_TIMER_A_enable_stall_in_debug_mode(SI32_TIMER_0);
	SI32_TIMER_A_select_low_clock_source_timer_clock_divider(SI32_TIMER_0);
//	SI32_TIMER_A_set_clock_divider_reload(SI32_TIMER_0,clkdiv_ms);
	SI32_TIMER_A_set_low_count(SI32_TIMER_0,0);
	SI32_TIMER_A_set_low_reload(SI32_TIMER_0,0x100);
	SI32_TIMER_A_stop_low_timer(SI32_TIMER_0);	
//	SI32_TIMER_A_start_low_timer(SI32_TIMER_0);
	SI32_TIMER_A_clear_low_overflow_interrupt(SI32_TIMER_0);	
	NVIC_ClearPendingIRQ( TIMER0L_IRQn  );
	NVIC_EnableIRQ( TIMER0L_IRQn  );

}

void timeout_off(void)
{

}

void timeout_item_init(void)
{
	uint8_t i;
	us_timeout_index = 0;
	ms_timeout_index = 0;
	timeout_current_ms_time = 0;
	timeout_current_us_time = 0;	
	timeout_item *temp;
	for(i=0; i< US_TIMEOUT_ARRAY_LEN; i++)
	{
		temp = &us_timeout_array[i];
		temp->flag = 0;
		temp->time = 0;
		temp->pEvent = NULL;
	}
	for(i=0; i< MS_TIMEOUT_ARRAY_LEN; i++)
	{
		temp = &ms_timeout_array[i];
		temp->flag = 0;
		temp->time = 0;
		temp->pEvent = NULL;
	}

}

uint16_t get_timeout_time(uint8_t us_ms)
{
	uint16_t time = 0;
	if(us_ms == TIMEOUT_MS)
	{
		if(!timeout_current_ms_time)
			return 0;
		time = SI32_TIMER_A_get_low_count(SI32_TIMER_0);
		time = 0xffff - time;
		time -= timeout_current_ms_time;
	}
	else if(us_ms == TIMEOUT_US)
	{
		if(!timeout_current_us_time)
			return 0;
		time = SI32_TIMER_A_get_high_count(SI32_TIMER_0);
		time = 0xffff - time;
		time -= timeout_current_us_time;		
	}
	
	return time;
}

void set_timeout_time(uint16_t time, uint8_t us_ms)
{
	uint16_t set_time = 0xffff - time;
	if(us_ms == TIMEOUT_US)
	{
		SI32_TIMER_A_stop_high_timer(SI32_TIMER_0);	
		SI32_TIMER_A_set_high_count(SI32_TIMER_0,set_time);
		SI32_TIMER_A_clear_high_overflow_interrupt(SI32_TIMER_0);
		NVIC_ClearPendingIRQ( TIMER0H_IRQn  );			
		SI32_TIMER_A_start_high_timer(SI32_TIMER_0);	
	}
	else if(us_ms == TIMEOUT_MS)
	{
		SI32_TIMER_A_stop_low_timer(SI32_TIMER_0);	
		SI32_TIMER_A_set_low_count(SI32_TIMER_0,set_time);
		SI32_TIMER_A_clear_low_overflow_interrupt(SI32_TIMER_0);
		NVIC_ClearPendingIRQ( TIMER0L_IRQn  );				
		SI32_TIMER_A_start_low_timer(SI32_TIMER_0);	
	}
	return;
}

void stop_timeout_timer(uint8_t us_ms)
{
	if(us_ms == TIMEOUT_US)
	{
		SI32_TIMER_A_stop_high_timer(SI32_TIMER_0);	
	}
	else if(us_ms == TIMEOUT_US)
	{
		SI32_TIMER_A_stop_low_timer(SI32_TIMER_0);	
	}
}

void start_timeout_timer(uint8_t us_ms)
{
	if(us_ms == TIMEOUT_US)
	{
		SI32_TIMER_A_start_high_timer(SI32_TIMER_0);	
	}
	else if(us_ms == TIMEOUT_US)
	{
		SI32_TIMER_A_start_low_timer(SI32_TIMER_0);	
	}
}

uint32_t add_cnt = 0;
uint32_t update_cnt = 0;
void timeout_add_item(int time, void *pEvent, uint8_t us_ms)
{
	uint8_t i,j;
	uint8_t *p_timeout_index;
	timeout_item *p_array;
	timeout_item temp;
	int set_time;
	uint16_t *p_timeout_current_time;
	int current_time;
	int temp_time;
	add_cnt++;
	if(us_ms == TIMEOUT_MS)
	{
		p_timeout_index = &ms_timeout_index;
		p_array = ms_timeout_array;
		temp_time = time*factor_ms;
		p_timeout_current_time = &timeout_current_ms_time;
	}	
	else if(us_ms == TIMEOUT_US)
	{
		p_timeout_index = &us_timeout_index;
		p_array = us_timeout_array;
		temp_time = time/factor_us;
		p_timeout_current_time = &timeout_current_us_time;
	}
	stop_timeout_timer(us_ms);
	current_time = get_timeout_time(us_ms);

	for(i=0; i< *p_timeout_index; i++)
	{
		p_array[i].time -= current_time;
	}

	p_array[*p_timeout_index].time =  temp_time;
	p_array[*p_timeout_index].flag = 0;
	p_array[*p_timeout_index].pEvent = pEvent;
	*p_timeout_index += 1;
	for(i = 0; i < *p_timeout_index; i++)
	{
		for(j=0; j<*p_timeout_index-i-1; j++)
		{
			if(p_array[j].time > p_array[j+1].time)
			{
				temp = p_array[j];
				p_array[j] = p_array[j+1];
				p_array[j+1] = temp;
			}				
		}
	}
	set_time = p_array[0].time;
	if(set_time > 0xffff)
	{
		set_time = 0xffff;
	}
	*p_timeout_current_time = set_time;
	for(i=0; i<=*p_timeout_index; i++)
	{
		if(set_time == p_array[i].time)
			p_array[i].flag = 1;
		else
			p_array[i].flag = 0;
	}
	set_timeout_time((uint16_t)set_time, us_ms);
}

void timeout_remove_item(uint8_t *p_event, uint8_t us_ms)
{
	uint8_t i;
	uint8_t *p_timeout_index;
	timeout_item *p_array;
	uint8_t *p_event_temp;
	uint16_t *p_timeout_current_time;
	int current_time;
	int set_time;
	uint8_t event_cnt = 0;
	uint8_t i_temp;
	add_cnt++;
	if(us_ms == TIMEOUT_MS)
	{
		p_timeout_index = &ms_timeout_index;
		p_array = ms_timeout_array;
		p_timeout_current_time = &timeout_current_ms_time;
	}	
	else if(us_ms == TIMEOUT_US)
	{
		p_timeout_index = &us_timeout_index;
		p_array = us_timeout_array;
		p_timeout_current_time = &timeout_current_us_time;
	}
	stop_timeout_timer(us_ms);

	current_time = get_timeout_time(us_ms);
	for(i=0; i<*p_timeout_index; i++)
	{
		if(p_array[i].pEvent == p_event)
		{
			*p_event = 0;
			i_temp = i;
			break;
		}
	}
	if(i == *p_timeout_index)
	{
		if(*p_timeout_index)
			start_timeout_timer(us_ms);
		return;
	}

	for(i=event_cnt; i<*p_timeout_index; i++)
	{
		p_array[i].time -= current_time;
		if(i == i_temp)
		{
			p_array[i].time = 0;
			p_array[i].flag = 0;
			p_array[i].pEvent = NULL;
			event_cnt++;			
			continue;
		}
		else if((p_array[i].time < 0))
		{
			p_event_temp = p_array[i].pEvent;
			*p_event_temp = 1;			
			p_array[i].time = 0;
			p_array[i].flag = 0;
			p_array[i].pEvent = NULL;
			event_cnt++;
			continue;
		}
		p_array[i-event_cnt] = p_array[i];
	}
	*p_timeout_index -= event_cnt;
	if(*p_timeout_index)
	{
		set_time = p_array[0].time;
		if(set_time > 0xffff)
		{
			set_time = 0xffff;
		}
		*p_timeout_current_time = set_time;
		for(i=0; i<*p_timeout_index; i++)
		{
			if(p_array[i].time == set_time)
				p_array[i].flag = 1;
			else
				break;
		}
		set_timeout_time((uint16_t)set_time, us_ms);
	}


	

}
void timeout_updata_item(uint8_t us_ms)
{
	uint8_t i = 0;
	uint8_t event_cnt = 0;
	int set_time;
	uint8_t *p_timeout_index;
	timeout_item *p_array;
	uint8_t *p_event_temp;
	uint16_t *p_current_time;
	update_cnt++;
	if(us_ms == TIMEOUT_MS)
	{
		p_timeout_index = &ms_timeout_index;
		p_array = ms_timeout_array;
		p_current_time = &timeout_current_ms_time;
	}
	else if(us_ms == TIMEOUT_US)
	{
		p_timeout_index = &us_timeout_index;
		p_array = us_timeout_array;
		p_current_time = &timeout_current_us_time;
	}
	for(i=0; i < *p_timeout_index; i++)
	{
		if(p_array[i].flag)
		{
			p_event_temp = p_array[i].pEvent;
			*p_event_temp = 1;
			p_array[i].time = 0;
			p_array[i].flag = 0;
			p_array[i].pEvent = NULL;
			event_cnt++;
		}
		else
			break;
	}
	for(i=event_cnt; i<*p_timeout_index; i++)
	{
		p_array[i].time -= *p_current_time;
		if(p_array[i].time < 0)
		{
			p_event_temp = p_array[i].pEvent;
			*p_event_temp = 1;			
			p_array[i].time = 0;
			p_array[i].flag = 0;
			p_array[i].pEvent = NULL;
			event_cnt++;
			continue;
		}
		p_array[i-event_cnt] = p_array[i];
	}	
	*p_timeout_index -= event_cnt;
	if(*p_timeout_index)
	{
		set_time = p_array[0].time;
		if(set_time > 0xffff)
		{
			set_time = 0xffff;
		}
		*p_current_time = set_time;
		for(i=0; i<*p_timeout_index; i++)
		{
			if(p_array[i].time == set_time)
				p_array[i].flag = 1;
			else
				break;
		}
		set_timeout_time((uint16_t)set_time, us_ms);
	}
}

void us_timeout_interrupt_handler(void)
{
	SI32_TIMER_A_stop_high_timer(SI32_TIMER_0);	
	timeout_updata_item(TIMEOUT_US);
}

void ms_timeout_interrupt_handler(void)
{
	SI32_TIMER_A_stop_low_timer(SI32_TIMER_0);	
	timeout_updata_item(TIMEOUT_MS);
}


void TIMER0H_IRQHandler(void)
{
	SI32_TIMER_A_stop_high_timer(SI32_TIMER_0);
	SI32_TIMER_A_clear_high_overflow_interrupt(SI32_TIMER_0);
	NVIC_ClearPendingIRQ( TIMER0H_IRQn  );
	us_timeout_interrupt_handler();
}


uint32_t zenme = 0;
void TIMER0L_IRQHandler(void)
{
	zenme++;
	SI32_TIMER_A_stop_low_timer(SI32_TIMER_0);
	SI32_TIMER_A_clear_low_overflow_interrupt(SI32_TIMER_0);
	NVIC_ClearPendingIRQ( TIMER0L_IRQn  );
	ms_timeout_interrupt_handler();
}

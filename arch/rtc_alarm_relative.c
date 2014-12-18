#include "arch_include.h"
#include "rtc_alarm_relative.h"

rtc_alarm_relative_item rtc_alarm_relative_table[RTC_ALARM_RELATIVE_TABLE_LEN];
//rtc_alarm_relative_item *p_rtc_alarm_relative_table[RTC_ALARM_RELATIVE_TABLE_LEN];
uint8_t rtc_alarm_relative_map[RTC_ALARM_RELATIVE_TABLE_LEN];
rtc_alarm_relative_item rtc_alarm_relative_header;
rtc_alarm_relative_item *rtc_alarm_relative_head;

//extern uint32_t press_button_cnt;
void* rtc_alarm_relative_malloc(void)
{
	uint8_t i;
	uint8_t len = RTC_ALARM_RELATIVE_TABLE_LEN;
	for(i = 0 ; i < len; i++)
	{
		if(rtc_alarm_relative_map[i] == 0)
		{
			rtc_alarm_relative_map[i] = 1;
			return (void *)(&rtc_alarm_relative_table[i]);
		}	
	}
	return NULL;
}

void rtc_alarm_relative_free(rtc_alarm_relative_item *p_item)
{
	uint8_t pos;
	if(p_item)
	{
		p_item->next = NULL;
		pos = p_item->position;
		rtc_alarm_relative_map[pos] = 0;
	}	
}

int rtc_alarm_relative_init(void)
{
	int ret, i = 0;
	rtc_alarm_relative_head = &rtc_alarm_relative_header;
	rtc_alarm_relative_head->next = NULL;

	for(i = 0; i < RTC_ALARM_RELATIVE_TABLE_LEN; i++)
	{
		rtc_alarm_relative_table[i].pending_flag = 0;
		rtc_alarm_relative_table[i].pEvent = NULL;
		rtc_alarm_relative_table[i].long_time = 0;
		rtc_alarm_relative_table[i].rtc_time = 0;
		rtc_alarm_relative_table[i].absolute_time = 0;
		rtc_alarm_relative_table[i].position = i;
		rtc_alarm_relative_table[i].next = NULL;
		rtc_alarm_relative_map[i] = 0;
	}
	return ret;
}

int rtc_alarm_relative_add_item(uint32_t time, uint8_t *pEvent)
{
	int ret = 0;
	int reset_flag;
	uint64_t current_time;
	uint64_t set_time;
	uint8_t resort_flag = 0;
	uint32_t cpu_sr = cpu_sr;
	rtc_alarm_relative_item *p_temp;
	rtc_alarm_relative_item *p_item = rtc_alarm_relative_head;
	set_time = time<<5;
	ENTER_CRITICAL_REGION();
	current_time = get_current_time();
	set_time += current_time;
	while(p_item->next)
	{
		if(p_item->next->pEvent == pEvent)
		{	
			p_item->next->absolute_time = set_time;
			p_item->next->long_time = (uint32_t)(set_time>>32);
			p_item->next->rtc_time = (uint32_t)(set_time);
			p_item->pending_flag = 0;
			p_temp = p_item->next;
			p_item->next = p_item->next->next;
			p_temp->next = NULL;
			resort_flag = 1;
			break;
		}
		p_item = p_item->next;
	}
	if(resort_flag)
	{
		resort_flag = 0;
		reset_flag = rtc_alarm_relative_insert_item(p_temp);
	}
	else
	{
		p_temp = (rtc_alarm_relative_item *)rtc_alarm_relative_malloc();
		p_temp->pEvent = pEvent;
		p_temp->absolute_time = set_time;
		p_temp->long_time = (uint32_t)(set_time>>32);
		p_temp->rtc_time = (uint32_t)set_time;
		p_temp->pending_flag = 0;
		reset_flag = rtc_alarm_relative_insert_item(p_temp);
	}

	if(reset_flag)
	{
		rtc_alarm_relative_initial_alarm();
	}
	

	LEAVE_CRITICAL_REGION();
	return ret;
}

int rtc_alarm_relative_initial_alarm(void)
{
	rtc_alarm_relative_item *p_item = rtc_alarm_relative_head;
	uint64_t absolute_time;
	uint64_t current_time = get_current_time();
//	ttt_current = current_time;
	if(p_item->next)
	{
		absolute_time = p_item->next->absolute_time;
		while(p_item->next)
		{			
			if(absolute_time == p_item->next->absolute_time)
			{
				p_item->next->pending_flag = 1;
				p_item = p_item->next;
			}
			else
			{
				p_item->next->pending_flag = 0;
				p_item = p_item->next;
			}
		}
		rtc_alarm_relative_enable((uint32_t)absolute_time);
		
	}
	else
	{
		rtc_alarm_relative_disable();
	}
	return 0;
}

int rtc_alarm_relative_insert_item(rtc_alarm_relative_item *p_temp)
{
	int ret = 0;
	uint8_t insert_flag = 0;
	rtc_alarm_relative_item *p_item = rtc_alarm_relative_head;
	rtc_alarm_relative_item *p_head = rtc_alarm_relative_head;
	while(p_item->next)
	{
		if(p_temp->absolute_time <= p_item->next->absolute_time)
		{
			p_temp->next = p_item->next;
			p_item->next = p_temp;
			insert_flag = 1;
			break;
		}
		p_item = p_item->next;	
	}
	if(!insert_flag)
	{
		p_item->next = p_temp;
		p_temp->next = NULL;
	}
	if(p_head->next == p_temp)
		ret = 1;
	return ret;
}

int rtc_alarm_relative_delete_item(uint8_t *pEvent)
{
	uint32_t cpu_sr = cpu_sr;

	rtc_alarm_relative_item *p_temp;
	rtc_alarm_relative_item *p_item = rtc_alarm_relative_head;
	ENTER_CRITICAL_REGION();
	*pEvent = 0;
	while(p_item->next)
	{
		if(p_item->next->pEvent == pEvent)
		{
			p_temp = p_item->next;
			p_item->next = p_item->next->next;
			pEvent = p_temp->pEvent;
			rtc_alarm_relative_free(p_temp);
		}
	}
	LEAVE_CRITICAL_REGION();
	return 0;
}

int rtc_alarm_relative_handle_timeout_item(void)
{
	uint8_t *pEvent;
	rtc_alarm_relative_item *p_temp;
	rtc_alarm_relative_item *p_item = rtc_alarm_relative_head;
	uint64_t absolute_time;
	uint64_t current_time = get_current_time();	
	while(p_item->next)
	{
		absolute_time = p_item->next->absolute_time;
		if(p_item->next->pending_flag == 1)
		{
			pEvent = p_item->next->pEvent;
			*pEvent = 1;
			p_item->next->pending_flag = 0;
			p_item->next->complete_flag = 1;
			p_temp = p_item->next;
			p_item->next = p_item->next->next;
			rtc_alarm_relative_free(p_temp);
		}
		else if(absolute_time <= current_time)
		{
			pEvent = p_item->next->pEvent;
			*pEvent = 1;
			p_temp = p_item->next;
			p_item->next = p_item->next->next;
			rtc_alarm_relative_free(p_temp);			
		}
		else
		{
			p_item = p_item->next;
		}
	}	
	return 0;
}

void rtc_alarm_relative_update_item(void)
{
	rtc_alarm_relative_handle_timeout_item();	
	
	rtc_alarm_relative_initial_alarm();

}

int rtc_alarm_relative_adjust_item(int64_t offset)
{
	uint8_t alarm_cnt = 0;
	uint64_t absolute_time;
	uint32_t cpu_sr = cpu_sr;
	rtc_alarm_relative_item *p_item = rtc_alarm_relative_head;
	ENTER_CRITICAL_REGION();
	while(p_item->next)
	{
		alarm_cnt++;
		p_item->absolute_time += offset;
		absolute_time = p_item->absolute_time;
		p_item->long_time = (uint32_t)(absolute_time>>32);
		p_item->rtc_time = (uint32_t)absolute_time;
	}
	rtc_alarm_relative_initial_alarm();
	LEAVE_CRITICAL_REGION();
	return 0;
}



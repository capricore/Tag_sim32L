#ifndef RTC_ALARM_RELATIVE_H
#define RTC_ALARM_RELATIVE_H

#define RTC_ALARM_RELATIVE_TABLE_LEN		5

typedef struct rtc_alarm_relative_item_t
{
	uint64_t absolute_time;
	uint32_t long_time;
	uint32_t rtc_time;
	int pending_flag;
	uint8_t *pEvent;
	uint8_t complete_flag;
	uint8_t position;
	struct rtc_alarm_relative_item_t *next;
}rtc_alarm_relative_item;

int rtc_alarm_relative_init(void);

int rtc_alarm_relative_insert_item(rtc_alarm_relative_item *p_temp);

int rtc_alarm_relative_initial_alarm(void);

void rtc_alarm_relative_update_item(void);

int rtc_alarm_relative_handle_timeout_item(void);

int rtc_alarm_relative_add_item(uint32_t time, uint8_t *pEvent);

int rtc_alarm_relative_delete_item(uint8_t *pEvent);


#endif


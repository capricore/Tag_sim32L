#ifndef RTC0_H
#define RTC0_H

#include <si32_device.h>
#include "workstate.h"
#include <SI32_RTC_B_Type.h>

extern uint16_t config_interval;
extern uint8_t rtc_wake_event;
extern uint32_t interval;

uint32_t get_quater_seconds(void);

void rtc_init(void);

void rtc_set_from_reset(void);

void set_current_time(uint64_t time);

uint64_t get_current_time(void);

void rtc_overflow_enable(void);

void rtc_alarm_enable(uint32_t rtc_time);

void rtc_alarm_disable(void);

void rtc_alarm_relative_disable(void);

void rtc_alarm_relative_enable(uint32_t rtc_time);

void time_synchronizing(enum card_state state);
/*


void rtc_off(void);

void rtc_start(void);

void rtc_restart(void);

void rtc_stop(void);

uint32_t get_long_time(void);

void set_long_time(uint32_t long_time);

uint32_t get_short_time(void);

void set_rtc_alarm1(uint32_t time);

void reset_rtc_alarm1(void);

uint32_t get_interval(void);

interval_time get_interval_time(void);

uint32_t get_cycle_next_wake_time(void);

void set_cycle_next_wake_time(void);

void set_next_wake_time(uint32_t next_time);

uint32_t get_next_wakeup_time(void);

void rtc_sync(uint32_t long_time, uint32_t short_time, real_time recv_time);


void set_rtc_relative_alarm(uint32_t time);

void set_rtc_alarm(uint32_t time);

void disable_rtc_alarm(void);

void rtc_alarm1_disable(void);

void rtc_alarm1_enable(void);

void set_next_wakeup_time(uint32_t time);

void rtc_start_cycle_duty(void);

void rtc_stop_cycle_duty(void);

*/
#endif


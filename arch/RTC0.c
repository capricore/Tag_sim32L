#include "arch_include.h"
#include "rtc_alarm_relative.h"
#include "event_receive_pkt.h"
#include "event_wk_time.h"
#define COMPENSATE_TIME  41
#define EXCITE_TIME  12*32
#define EXCITE_TIME_RF  500*32
uint32_t long_counter;
uint8_t rtc_wake_event;
uint64_t next_wakeup_time;
uint32_t temp_alram1_time;
uint32_t overflow_value = 0xffffffff;
uint32_t overflow_offset = 32;
uint16_t  pkt_sub_time;
uint32_t pkt_time_long;
uint32_t pkt_next_time_long;
uint64_t time = 0;
extern uint8_t period;	
extern uint64_t received_time;
extern uint8_t time_waking;
extern struct time_info pkt_time_struct, pkt_cave_struct;
extern uint32_t inter;


uint64_t caca = 0;
uint64_t cur_time;
uint64_t cccc_time = 0;
uint64_t next_temp_time;
uint32_t real_time_offset = 0;
uint32_t read_alarm1 = 0;
uint32_t read_setcap;
int flag = 0;

void rtc_init(void)
{
	uint32_t i;
	SI32_CLKCTRL_A_enable_apb_to_modules_1(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG1_MISC0CEN_ENABLED_U32);

	rtc_set_from_reset();
	SI32_RTC_B_disable_auto_gain_control (SI32_RTC_0);
	SI32_RTC_B_enable_bias_doubler (SI32_RTC_0);
	SI32_RTC_B_enable_autostep (SI32_RTC_0);
	SI32_RTC_B_set_rtc_load_capacitance(SI32_RTC_0,0);
	SI32_RTC_B_enable_clock_out(SI32_RTC_0);
	SI32_RTC_B_enable_crystal_oscillator (SI32_RTC_0);
	SI32_RTC_B_set_clock_source_rtc (SI32_RTC_0);

	SI32_RTC_B_enable_rtc_timer(SI32_RTC_0);

	for (i = 0; i < 70000; i++);   // Should wait 20ms here before next poll
	while(!SI32_RTC_B_is_external_oscillator_valid(SI32_RTC_0));
	while(!SI32_RTC_B_is_load_capacitance_ready(SI32_RTC_0));
	SI32_RTC_B_disable_external_pin_clock_drive(SI32_RTC_0);
	SI32_RTC_B_enable_auto_gain_control(SI32_RTC_0);
	SI32_RTC_B_disable_bias_doubler(SI32_RTC_0);
	SI32_RTC_B_enable_missing_clock_detector(SI32_RTC_0);

	SI32_RTC_B_disable_alarm0_auto_reset(SI32_RTC_0);

	SI32_RTC_B_enable_high_speed_mode(SI32_RTC_0);

	SI32_PMU_A_disable_rtc0_fail_wake_event(SI32_PMU_0);
	SI32_PMU_A_enable_rtc0_alarm_wake_event(SI32_PMU_0);	

	NVIC_ClearPendingIRQ(RTC0ALRM_IRQn);
	NVIC_ClearPendingIRQ(RTC0FAIL_IRQn);
	NVIC_EnableIRQ(RTC0ALRM_IRQn);
	NVIC_DisableIRQ(RTC0FAIL_IRQn);

	rtc_overflow_enable();
	SI32_RTC_B_start_timer(SI32_RTC_0);
	
}

void rtc_set_from_reset(void)
{
	SI32_RTC_B_stop_timer(SI32_RTC_0);
	NVIC_DisableIRQ(RTC0ALRM_IRQn);
	NVIC_DisableIRQ(RTC0FAIL_IRQn);	
	SI32_RTC_B_disable_alarm0(SI32_RTC_0);  	
	SI32_RTC_B_disable_alarm1(SI32_RTC_0);  	
	SI32_RTC_B_disable_alarm2(SI32_RTC_0); 
	SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
	SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
	SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
	SI32_PMU_A_disable_rtc0_fail_wake_event(SI32_PMU_0);
	SI32_PMU_A_disable_rtc0_alarm_wake_event(SI32_PMU_0);		
	SI32_RTC_B_start_timer_set(SI32_RTC_0);
	SI32_RTC_B_write_setcap(SI32_RTC_0, 0);
	while(SI32_RTC_B_is_timer_set_in_progress(SI32_RTC_0));	

	NVIC_ClearPendingIRQ(RTC0ALRM_IRQn);	
}

void rtc_overflow_enable(void)
{
	if(!SI32_RTC_B_is_alarm0_enabled(SI32_RTC_0))
	{
		SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
		SI32_RTC_B_write_alarm0(SI32_RTC_0, overflow_value);
		SI32_RTC_B_enable_alarm0(SI32_RTC_0);  	
	}
}

void rtc_overflow_disable(void)
{
	SI32_RTC_B_disable_alarm0(SI32_RTC_0); 
}


void rtc_alarm_enable(uint32_t rtc_time)
{
	if(!(SI32_RTC_B_is_alarm1_enabled(SI32_RTC_0)))
	{
		SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
		SI32_RTC_B_write_alarm1(SI32_RTC_0, rtc_time);
		SI32_RTC_B_enable_alarm1(SI32_RTC_0);
	}
	else
	{
		SI32_RTC_B_write_alarm1(SI32_RTC_0, rtc_time);
	}
}

void rtc_alarm_disable(void)
{
	SI32_RTC_B_disable_alarm1(SI32_RTC_0); 
}

void rtc_alarm_relative_enable(uint32_t rtc_time)
{
	if(!(SI32_RTC_B_is_alarm2_enabled(SI32_RTC_0)))
	{
		SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
		SI32_RTC_B_write_alarm2(SI32_RTC_0, rtc_time);
		SI32_RTC_B_enable_alarm2(SI32_RTC_0);
	}
	else
	{
		SI32_RTC_B_write_alarm2(SI32_RTC_0, rtc_time);
	}	
}


void rtc_alarm_relative_disable(void)
{
	SI32_RTC_B_disable_alarm2(SI32_RTC_0); 
}

	uint32_t rtc_time;
	uint64_t ret_time;
uint64_t get_current_time(void)
{
	uint32_t cpu_sr = cpu_sr;
	ENTER_CRITICAL_REGION();
	SI32_RTC_B_start_timer_capture(SI32_RTC_0);	
	while(SI32_RTC_B_is_timer_capture_in_progress(SI32_RTC_0));
	rtc_time = SI32_RTC_B_read_setcap(SI32_RTC_0);	
	if(SI32_RTC_B_is_alarm0_interrupt_pending(SI32_RTC_0))
	{
		SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
		long_counter += 1;		
		rtc_time = 0;
	}
	ret_time = (((uint64_t)long_counter)<<overflow_offset)+rtc_time;
	LEAVE_CRITICAL_REGION();
	return ret_time;
}

uint32_t get_rtc_setcap(void)
{
	uint32_t setcap;
	SI32_RTC_B_start_timer_capture(SI32_RTC_0);	
	while(SI32_RTC_B_is_timer_capture_in_progress(SI32_RTC_0));
	setcap = SI32_RTC_B_read_setcap(SI32_RTC_0);		
	return setcap;
}

uint32_t get_quater_seconds(void)
{
	uint64_t time = get_current_time();
	time >>= 13;
	return ((uint32_t)time);
}

void set_current_time(uint64_t time)
{
	uint32_t rtc_time = (uint32_t)time;
	uint32_t cpu_sr = cpu_sr;
	ENTER_CRITICAL_REGION();

	SI32_RTC_B_start_timer_set(SI32_RTC_0);
	SI32_RTC_B_write_setcap(SI32_RTC_0, rtc_time);
	while(SI32_RTC_B_is_timer_set_in_progress(SI32_RTC_0));	
	long_counter = (uint32_t)(time>>32);
	if(time == 0XFFFFFFFF)
	{
		long_counter++;
		SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
	}	
	LEAVE_CRITICAL_REGION();
}
/*
uint32_t get_interval(void)
{
	interval = config_interval*0x8000;
	return interval;
}

void rtc_cycle_duty_enable(void)
{

}

uint32_t rtc_get_timer(void)
{
	uint32_t time;
	SI32_RTC_B_start_timer_capture(SI32_RTC_0);	
	while(SI32_RTC_B_is_timer_capture_in_progress(SI32_RTC_0));
	time = SI32_RTC_B_read_setcap(SI32_RTC_0);	
	return time;
}

void rtc_set_timer(uint32_t time)
{
	SI32_RTC_B_start_timer_set(SI32_RTC_0);
	SI32_RTC_B_write_setcap(SI32_RTC_0, time);
	while(SI32_RTC_B_is_timer_set_in_progress(SI32_RTC_0));
}

void rtc_start_button_press(void)
{
//	uint32_t time;
//	temp_alram1_time = SI32_RTC_B_read_alarm1(SI32_RTC_0);
	
}
void rtc_start_cycle_duty(void)
{
	uint32_t time = rtc_get_timer();	
	set_next_wakeup_time(time);
	rtc_alarm1_enable();
}

void rtc_stop_cycle_duty(void)
{
//	rtc_alarm1_disable();
}
void rtc_alarm2_enable(void)
{
	SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
	SI32_RTC_B_enable_alarm2(SI32_RTC_0);  
}

void rtc_alarm2_disable(void)
{
	SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
	SI32_RTC_B_disable_alarm2(SI32_RTC_0);  
}
void rtc_alarm1_enable(void)
{
	SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
	SI32_RTC_B_enable_alarm1(SI32_RTC_0);  
}
void rtc_alarm1_disable(void)
{
	SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
	SI32_RTC_B_disable_alarm1(SI32_RTC_0);  
	
}

void rtc_alarm0_enable(void)
{
	SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
	SI32_RTC_B_write_alarm0(SI32_RTC_0, 0Xffffffff);
	SI32_RTC_B_enable_alarm0(SI32_RTC_0);  
}

void rtc_alarm0_disable(void)
{
	SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
	SI32_RTC_B_disable_alarm0(SI32_RTC_0);  
}

uint32_t get_rtc_relative_alarm(void)
{
	uint32_t time = SI32_RTC_B_read_alarm1(SI32_RTC_0);
	return time;
}

void set_rtc_alarm(uint32_t time)
{
	SI32_RTC_B_write_alarm1(SI32_RTC_0, time);
	SI32_RTC_B_enable_alarm1(SI32_RTC_0); 
}

void set_rtc_relative_alarm(uint32_t time)
{
	SI32_RTC_B_write_alarm2(SI32_RTC_0, time);
	SI32_RTC_B_enable_alarm2(SI32_RTC_0); 
}

void disable_rtc_alarm(void)
{
	SI32_RTC_B_disable_alarm1(SI32_RTC_0); 
}

void set_rtc_alarm1(uint32_t time)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_1(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG1_MISC0CEN_ENABLED_U32);
	SI32_RTC_B_write_alarm1(SI32_RTC_0, time);
	NVIC_ClearPendingIRQ(RTC0ALRM_IRQn);
	SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
	NVIC_EnableIRQ(RTC0ALRM_IRQn);
	SI32_RTC_B_enable_alarm1(SI32_RTC_0); 	
	SI32_PMU_A_enable_rtc0_alarm_wake_event(SI32_PMU_0);
}


void reset_rtc_alarm1(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_1(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG1_MISC0CEN_ENABLED_U32);

	SI32_RTC_B_disable_alarm1(SI32_RTC_0); 
	SI32_RTC_B_write_alarm1(SI32_RTC_0, 0);
	SI32_PMU_A_disable_rtc0_alarm_wake_event(SI32_PMU_0);

}

void rtc_start(void)
{
	SI32_RTC_B_start_timer(SI32_RTC_0);
}

void rtc_restart(void)
{
	SI32_RTC_B_stop_timer(SI32_RTC_0);
	SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
	SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
	SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
	NVIC_ClearPendingIRQ(RTC0ALRM_IRQn);
	rtc_set_timer(0);
	SI32_RTC_B_start_timer(SI32_RTC_0);
}

void rtc_stop(void)
{
	SI32_RTC_B_stop_timer(SI32_RTC_0);
	SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
	SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
	SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
	NVIC_ClearPendingIRQ(RTC0ALRM_IRQn);	
}

void rtc_off(void)
{
   SI32_RTC_B_disable_alarm0(SI32_RTC_0);
   SI32_RTC_B_disable_alarm1(SI32_RTC_0);
   SI32_RTC_B_disable_alarm2(SI32_RTC_0);
   NVIC_ClearPendingIRQ(RTC0ALRM_IRQn);
   NVIC_ClearPendingIRQ(RTC0FAIL_IRQn);
   NVIC_DisableIRQ(RTC0ALRM_IRQn);
   NVIC_DisableIRQ(RTC0FAIL_IRQn);
   SI32_RTC_B_disable_rtc_timer(SI32_RTC_0);
   SI32_CLKCTRL_A_disable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG1_MISC0CEN_ENABLED_U32);
}


real_time get_real_time(void)
{
	real_time time;
	uint32_t time_ss;
	ENTER_CRITICAL_REGION();
	SI32_RTC_B_start_timer_capture(SI32_RTC_0);
	while(SI32_RTC_B_is_timer_capture_in_progress(SI32_RTC_0));
	time_ss = SI32_RTC_B_read_setcap(SI32_RTC_0);
	time.time_ss = (time_ss&0x7fff);
	time.time_sec = long_counter+(time_ss>>15);
	LEAVE_CRITICAL_REGION();
	return time;
}

interval_time get_interval_time(void)
{
	interval_time counter;
	ENTER_CRITICAL_REGION();
	counter.long_counter = long_counter;
	SI32_RTC_B_start_timer_capture(SI32_RTC_0);
	while(SI32_RTC_B_is_timer_capture_in_progress(SI32_RTC_0));
	counter.short_conter = SI32_RTC_B_read_setcap(SI32_RTC_0);	
	LEAVE_CRITICAL_REGION();
	return counter;
}

uint32_t get_cycle_next_wake_time(void)
{
	uint32_t counter;
	ENTER_CRITICAL_REGION();
	counter = long_counter + config_interval;
	LEAVE_CRITICAL_REGION();
	return counter;
}

void set_cycle_next_wake_time(void)
{
	ENTER_CRITICAL_REGION();
	next_wakeup_time = long_counter + config_interval;
	LEAVE_CRITICAL_REGION();	
}

void set_next_wake_time(uint32_t next_time)
{
	ENTER_CRITICAL_REGION();
	next_wakeup_time = next_time;
	LEAVE_CRITICAL_REGION();
}

uint32_t get_next_wakeup_time(void)
{
	ENTER_CRITICAL_REGION();
	return next_wakeup_time;
	LEAVE_CRITICAL_REGION();
}
uint32_t get_long_time(void)
{
	uint32_t long_time;
	ENTER_CRITICAL_REGION();
	long_time = long_counter;
	LEAVE_CRITICAL_REGION();
	return long_time;
}

void set_long_time(uint32_t long_time)
{
	ENTER_CRITICAL_REGION();
	long_counter = long_time;
	LEAVE_CRITICAL_REGION();
}

void set_cycle_wakeup_time(void)
{
	uint32_t time = SI32_RTC_B_read_alarm0(SI32_RTC_0);
	time += interval;
	SI32_RTC_B_write_alarm0(SI32_RTC_0, time);
}

void set_seconds_wakeup_time(uint32_t rtc)
{
	uint32_t time  = SI32_RTC_B_read_alarm0(SI32_RTC_0);
	time &= ~((uint32_t)0x7fff);
	time |= rtc;
	SI32_RTC_B_write_alarm0(SI32_RTC_0, time);
}

void set_next_wakeup_time(uint32_t time)
{
	SI32_RTC_B_write_alarm1(SI32_RTC_0, time);
}

uint32_t get_short_time(void)
{
	uint32_t short_time;
	SI32_RTC_B_start_timer_capture(SI32_RTC_0);
	while(SI32_RTC_B_is_timer_capture_in_progress(SI32_RTC_0));
	short_time = SI32_RTC_B_read_setcap(SI32_RTC_0);
	return short_time;
}
void rtc_sync(uint32_t long_time, uint32_t short_time, real_time recv_time)
{
	real_time temp_time;
	real_time current_time = get_real_time();
	ENTER_CRITICAL_REGION();
	if(recv_time.time_ss>current_time.time_ss)
	{
		current_time.time_sec--;
		temp_time.time_ss = current_time.time_ss + 0x8000 - recv_time.time_ss;
	}
	else
	{
		temp_time.time_ss = current_time.time_ss - recv_time.time_ss;
	}
	temp_time.time_ss = short_time + temp_time.time_ss + 80;
	
	temp_time.time_sec = current_time.time_sec - recv_time.time_sec + long_time;
	if(temp_time.time_ss >= config_interval)
	{
		temp_time.time_ss -= config_interval;
		temp_time.time_sec++;
	}
	

	long_counter = temp_time.time_sec;
	SI32_RTC_B_write_setcap(SI32_RTC_0, temp_time.time_ss);
	
	LEAVE_CRITICAL_REGION();
}

*/
uint32_t alarm1_cnt = 0;
uint32_t alarm0_cnt = 0;
uint32_t alarm2_cnt = 0;
void RTC0ALRM_IRQHandler(void)
{
	if (SI32_RTC_B_is_alarm0_interrupt_pending(SI32_RTC_0) & SI32_RTC_B_is_alarm0_enabled(SI32_RTC_0))     
	{
		SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);
		long_counter++;	
		alarm0_cnt++;
	}
	if (SI32_RTC_B_is_alarm1_interrupt_pending(SI32_RTC_0) & SI32_RTC_B_is_alarm1_enabled(SI32_RTC_0))     
	{
		SI32_RTC_B_clear_alarm1_interrupt(SI32_RTC_0);
		
		SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3,0x0002);
		SI32_PBSTD_A_write_pins_high(SI32_PBSTD_3,0x0002);
//		rtc_alarm_update_item();
		
		inter++;
		time_waking = 1;
		alarm1_cnt++;
	}
	if (SI32_RTC_B_is_alarm2_interrupt_pending(SI32_RTC_0) & SI32_RTC_B_is_alarm2_enabled(SI32_RTC_0))     
	{
		SI32_RTC_B_clear_alarm2_interrupt(SI32_RTC_0);
		rtc_alarm_relative_update_item();
		alarm2_cnt++;
	}	
//	led_on();
}

void RTC0FAIL_IRQHandler(void)
{
	while(1);
}


void time_synchronizing(enum card_state state)
{
	
	uint64_t relative_time;
	uint32_t cpu_sr = cpu_sr;

	ENTER_CRITICAL_REGION();
	if(state == synchronizing){
		if(pkt_time_struct.time > 0x30000000)
			{
				flag = 1;
				pkt_sub_time = pkt_time_struct.subtime ;
				pkt_time_long = pkt_time_struct.time;
				time = pkt_time_long;
				time += 0x100000000;
				time <<= 8;
				time += pkt_sub_time;				
				time <<= 5;
				cur_time = get_current_time();
				relative_time = cur_time - pkt_time_struct.recv_time;
				time = time + relative_time;
				set_current_time(time+COMPENSATE_TIME);	

				//set alarm
				pkt_next_time_long = pkt_time_struct.nextWTime;
				
				next_wakeup_time = pkt_next_time_long + 0x100000000;
				
				next_wakeup_time <<= 13;
				
				cccc_time = next_wakeup_time;
				caca = get_current_time();
				rtc_alarm_enable((uint32_t) (next_wakeup_time));
				read_setcap = get_rtc_setcap();
				read_alarm1 = SI32_RTC_B_read_alarm1(SI32_RTC_0);
				cur_time = get_current_time();
				send_test_pkg_c();
				__NOP;
			}else
			{
				pkt_sub_time = pkt_time_struct.subtime ;
				pkt_time_long = pkt_time_struct.time;
				time = pkt_time_long;
			
				time <<= 8;
				time += pkt_sub_time;				
				time <<= 5;
				cur_time = get_current_time();
				relative_time = cur_time - pkt_time_struct.recv_time;
				set_current_time(time + relative_time +COMPENSATE_TIME);	

				//set alarm
				pkt_next_time_long = pkt_time_struct.nextWTime;
				next_wakeup_time = pkt_next_time_long;				
				next_wakeup_time <<= 13;
				next_temp_time = next_wakeup_time;
				cur_time = get_current_time();
				cccc_time = cur_time;
				caca = get_current_time();
				rtc_alarm_enable((uint32_t) next_wakeup_time);
				read_setcap = get_rtc_setcap();
				read_alarm1 = SI32_RTC_B_read_alarm1(SI32_RTC_0);
			}
//		rtc_alarm_relative_add_item(pkt_next_time_long,&time_waking);
		
		LEAVE_CRITICAL_REGION();		
		send_syn_ack(pkt_time_struct.seq, pkt_time_struct.panID, pkt_time_struct.receiverID);
		
		period = pkt_time_struct.period >> 2;
		}
								
	if(state == pending || state == in_cave){
		if(pkt_cave_struct.time > 0x30000000)
		{
			pkt_sub_time = pkt_cave_struct.subtime ;
			pkt_time_long = pkt_cave_struct.time;
			time = pkt_time_long;
			time += 0x100000000;
			time <<= 8;
			time += pkt_sub_time;				
			time <<= 5;
			cur_time = get_current_time();
			relative_time = cur_time - pkt_cave_struct.recv_time;
			set_current_time(time + relative_time +COMPENSATE_TIME);	

			//set alarm
			pkt_next_time_long = pkt_cave_struct.nextWTime;
				
			next_wakeup_time = pkt_next_time_long + 0x100000000;
				
			next_wakeup_time <<= 13;
			rtc_alarm_enable((uint32_t) next_wakeup_time);	
			cur_time = get_current_time();
			send_test_pkg_d();
			__NOP;
		}else
		{
			pkt_sub_time = pkt_cave_struct.subtime ;
			pkt_time_long = pkt_cave_struct.time;
			time = pkt_time_long;

			time <<= 8;
			time += pkt_sub_time;				
			time <<= 5;
			cur_time = get_current_time();
			relative_time = cur_time - pkt_cave_struct.recv_time;
			set_current_time(time + relative_time +COMPENSATE_TIME);	

			//set alarm
			pkt_next_time_long = pkt_cave_struct.nextWTime;
				next_wakeup_time = pkt_next_time_long;				
				next_wakeup_time <<= 13;
			rtc_alarm_enable((uint32_t) next_wakeup_time);	
		}
			
		
		if(state == in_cave){
			LEAVE_CRITICAL_REGION();
			send_cave_pkt(pkt_cave_struct.seq, pkt_cave_struct.panID, pkt_cave_struct.receiverID);
		}
		period = pkt_cave_struct.period >> 2;	
	}	
	LEAVE_CRITICAL_REGION();
}


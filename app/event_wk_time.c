#include "SI32_RTC_B_Type.h"
#include "rf2xx_tat.h"
#include "timestamp.h"
#include "RTC0.h"
#include "rf2xx_send.h"
#include "systick.h"
#include "crc.h"
#include "configuration.h"
#include "rf2xx_include.h"
#include "event_wk_time.h"
#include "workstate.h"

uint32_t wkp_sub_time;
uint8_t secOverflow, minOverflow;
tm_new temp_date_time;
uint32_t temp_time_long;
uint32_t temp_alarm_time_long;
uint16_t temp_pan_id;
extern uint8_t rssi1, rssi2;
extern uint16_t battery;
extern uint8_t fLength, frame[127];
uint8_t rssi;
extern uint8_t is_sent;
extern uint32_t sub_append;
extern uint8_t HnoSignal;
extern enum card_state c_state;
extern uint16_t lastEID;
			
extern int ms_offset, ms_offset_payoff, enCount, exCount;
extern tm_new temp_date_time;
extern uint32_t temp_sub_time, temp_alarm_sub_time;
extern tm_new temp_date_time;
extern tm_new temp_alarm_date_time;
extern uint32_t temp_alarm_time_long;
extern uint32_t next_wakeup_time;
extern uint8_t frame[127] ;
extern uint32_t alarm1;
uint32_t recv_next_wakeup_time;
uint32_t long_count;
//uint64_t current_time;

void set_next_wake_time(uint8_t period)
{
	alarm1++;
	uint32_t c_period;
	c_period = period << 15;
	next_wakeup_time += c_period ;
	rtc_alarm_enable((uint32_t )next_wakeup_time);
//	SI32_PMU_A_enable_rtc0_alarm_wake_event(SI32_PMU_0);
}
extern uint8_t PconSignal, PnoSignal, CnoSignal;		//分别代表：在pending状态下已连续收到包的周期个数；在pending状态下已连续没收到包的周期个剩，
extern uint16_t lastRID;							//记录上次收到哪个发来的包，基本没用到
extern uint16_t exciterID;
//extern uint8_t exRSSI1, exRSSI2;					//两个激励天线的ed值

void send_cave_pkt(uint8_t seq, uint16_t dest_panID, uint16_t dest_addr)
{
	uint8_t send_count = 0;
	uint32_t current_quater_seconds;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = 0x01;
		temp_pan_id = rf2xx_tat_get_short_address();
		frame[1] = temp_pan_id & 0xff;
		frame[2] = (temp_pan_id >> 8) & 0xff;
	//   RTC_GetTime(RTC_Format_BIN, &temp_time);
	//	RTC_GetDate(RTC_Format_BIN, &temp_date);
	//	get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
	//	temp_time_long = mktime_new(&temp_date_time);
	//	temp_time_long = temp_time_long << 2;
		current_quater_seconds = get_quater_seconds();
	
		frame[3] = current_quater_seconds& 0xff;
		frame[4] = (current_quater_seconds >> 8) & 0xff;
		frame[5] = (current_quater_seconds >> 16) & 0xff;
		frame[6] = (current_quater_seconds >> 24) & 0xff;
		frame[7] = rssi;
		
//		battery_sample();
		battery = battery >> 2;
		frame[8] = battery & 0xff;
		frame[9] = (battery >> 8) & 0xff;
		
	//	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
	//	get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime );
	//	temp_alarm_time_long = mktime_new(&temp_alarm_date_time);
// 		temp_alarm_time_long = temp_alarm_time_long << 2;
// 		temp_alarm_sub_time = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 		temp_alarm_time_long = temp_alarm_time_long & ((temp_alarm_sub_time >> 8) | 0xfffffffc);
// 		tx_frame[10] = temp_alarm_sub_time;
// 		tx_frame[11] = temp_alarm_sub_time >> 8;
// 		tx_frame[12] = temp_alarm_time_long & 0xff;
// 		tx_frame[13] = (temp_alarm_time_long >> 8) & 0xff;
// 		tx_frame[14] = (temp_alarm_time_long >> 16) & 0xff;
// 		tx_frame[15] = (temp_alarm_time_long >> 24) & 0xff;

		frame[16] = lastEID;
		frame[17] = lastEID >> 8;
		frame[18] = lastRID;
		frame[19] = lastRID >> 8;
		frame[20] = exciterID;
		frame[21] = exciterID >> 8;
		
		frame[22] = rssi1;
		frame[23] = rssi2;
		
		frame[24] = PconSignal;
		frame[25] = PnoSignal;
		frame[26] = CnoSignal;

		frame[27] = seq;
		frame[28] = 11;
		frame[29] = 0x30;
		
		rfTxInfo.destPanId = dest_panID;
		rfTxInfo.destAddr = dest_addr;
		rfTxInfo.length = 30;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
// 		is_sent = basicRfSendPacketTest(&rfTxInfo);

		is_sent = -1;
		while(is_sent != 0){
			is_sent =	mac_send_packet_extend(&rfTxInfo,5000);
			if (send_count++ >=2) {
				delay_ms(2);
				break;
			}
		}
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

void send_en_pkt() 
{
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = c_state;
		frame[1] = ms_offset;
		frame[2] = ms_offset_payoff;
		frame[3] = enCount;
		frame[4] = 0x40;
		
					//		RTC_GetTime(RTC_Format_BIN, &temp_time);
					//		RTC_GetDate(RTC_Format_BIN, &temp_date);
					//		get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
					//		temp_time_long = mktime_new(&temp_date_time);
					//		temp_time_long = temp_time_long << 2;
					//		temp_sub_time = RTC_GetSubSecond();
// 							current_time = get_real_time();
// 							current_time.time_sec = current_time.time_sec << 2;
// 							tx_frame[5] = current_time.time_ss;
// 							tx_frame[6] = current_time.time_ss >> 8;
// 							tx_frame[7] = temp_time.RTC_Seconds;
// 							tx_frame[8] = temp_time.RTC_Minutes;
// 							tx_frame[9] = temp_time.RTC_Hours;
// 							RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
// 							get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime);
// 							temp_alarm_time_long = mktime_new(&temp_alarm_date_time);
// 							temp_alarm_time_long = temp_alarm_time_long << 2;
// 							temp_alarm_sub_time = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 							temp_alarm_time_long = temp_alarm_time_long & ((temp_alarm_sub_time >> 8) | 0xfffffffc);
// 							tx_frame[10] = temp_alarm_sub_time;
// 							tx_frame[11] = temp_alarm_sub_time >> 8;
// 							tx_frame[12] = temp_alarm.RTC_AlarmTime.RTC_Seconds;
// 							tx_frame[13] = temp_alarm.RTC_AlarmTime.RTC_Minutes;
// 							tx_frame[14] = temp_alarm.RTC_AlarmTime.RTC_Hours;
// 							tx_frame[15] = lastEID;
// 							tx_frame[16] = lastEID >> 8;
							
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.length = 17;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
		is_sent =	mac_send_packet_basic(&rfTxInfo,5000);
// 		while(is_sent == 0){
// 			delay(2);
// 			is_sent =	basicRfSendPacket(&rfTxInfo);
// 		}
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

void send_hrt_pkt()
{
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = c_state;
		frame[1] = HnoSignal;
					//		RTC_GetTime(RTC_Format_BIN, &temp_time);
					//		RTC_GetDate(RTC_Format_BIN, &temp_date);
					//		get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
					//		temp_time_long = mktime_new(&temp_date_time);
					//		temp_time_long = temp_time_long << 2;
					//		temp_sub_time = RTC_GetSubSecond();
// 							current_time = get_real_time();
// 							current_time.time_sec = current_time.time_sec << 2;
// 							tx_frame[2] = current_time.time_ss;
// 							tx_frame[3] = current_time.time_ss >> 8;
// 							tx_frame[4] = temp_time.RTC_Seconds;
// 							tx_frame[5] = temp_time.RTC_Minutes;
// 							tx_frame[6] = temp_time.RTC_Hours;
// 							RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
// 							get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime);
// 							temp_alarm_time_long = mktime_new(&temp_alarm_date_time);
// 							temp_alarm_time_long = temp_alarm_time_long << 2;
// 							temp_alarm_sub_time = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 							temp_alarm_time_long = temp_alarm_time_long & ((temp_alarm_sub_time >> 8) | 0xfffffffc);
// 							tx_frame[7] = temp_alarm_sub_time;
// 							tx_frame[8] = temp_alarm_sub_time >> 8;
// 							tx_frame[9] = temp_alarm.RTC_AlarmTime.RTC_Seconds;
// 							tx_frame[10] = temp_alarm.RTC_AlarmTime.RTC_Minutes;
// 							tx_frame[11] = temp_alarm.RTC_AlarmTime.RTC_Hours;
// 							tx_frame[12] = lastEID;
// 							tx_frame[13] = lastEID >> 8;		
							
		frame[14] = 0x50;
		
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.length = 15;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
		is_sent =	mac_send_packet_basic(&rfTxInfo,5000);
// 		while(is_sent == 0){
// 			delay(2);
// 			is_sent =	basicRfSendPacket(&rfTxInfo);
// 		}
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

void send_ex_pkt()
{
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = c_state;
		frame[1] = ms_offset;
		frame[2] = ms_offset_payoff;
		frame[3] = exCount;
		frame[4] = 0x60;
		
				//			RTC_GetTime(RTC_Format_BIN, &temp_time);
				//			RTC_GetDate(RTC_Format_BIN, &temp_date);
				//			get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
				//			temp_time_long = mktime_new(&temp_date_time);
				//			temp_time_long = temp_time_long << 2;
				//			temp_sub_time = RTC_GetSubSecond();
// 							current_time = get_real_time();
// 							current_time.time_sec = current_time.time_sec << 2;
// 							tx_frame[5] = current_time.time_ss;
// 							tx_frame[6] = current_time.time_ss >> 8;
// 							tx_frame[7] = temp_time.RTC_Seconds;
// 							tx_frame[8] = temp_time.RTC_Minutes;
// 							tx_frame[9] = temp_time.RTC_Hours;
// 							RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
// 							get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime);
// 							temp_alarm_time_long = mktime_new(&temp_alarm_date_time);
// 							temp_alarm_time_long = temp_alarm_time_long << 2;
// 							temp_alarm_sub_time = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 							temp_alarm_time_long = temp_alarm_time_long & ((temp_alarm_sub_time >> 8) | 0xfffffffc);
// 							tx_frame[10] = temp_alarm_sub_time;
// 							tx_frame[11] = temp_alarm_sub_time >> 8;
// 							tx_frame[12] = temp_alarm.RTC_AlarmTime.RTC_Seconds;
// 							tx_frame[13] = temp_alarm.RTC_AlarmTime.RTC_Minutes;
// 							tx_frame[14] = temp_alarm.RTC_AlarmTime.RTC_Hours;		
// 							tx_frame[15] = lastEID;
// 							tx_frame[16] = lastEID >> 8;
							
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.length = 17;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
		is_sent =	mac_send_packet_basic(&rfTxInfo,5000);
// 		while(is_sent == 0){
// 			delay(2);
// 			is_sent =	basicRfSendPacket(&rfTxInfo);
// 		}
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

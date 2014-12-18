#include "arch_include.h"
#include "rf2xx_include.h"
#include "workstate.h"
#include "as3933_include.h"
#include "flash_operation.h"
#include "configuration.h"
#include "config.h"
#include "RTC0.h"
#include "systick.h"
#include "event_receive_pkt.h"
#include "event_wk_time.h"
#include "event_exciter.h"
#include "rf2xx_version.h"
#include "rf2xx_recv.h"
#include "rtc_alarm_relative.h"
#define DEFAULT_PANID   (4)	
uint8_t test_num = 0;
uint32_t alarm1 = 0;
uint32_t inter = 0;
uint8_t num = 0;
uint8_t num2 = 0;
uint32_t num3 = 0;
uint32_t num4 = 0;
uint8_t wake, time_waking = 0;		//��־λ���ж�����Ϊ1������CPU����������������һ��Ϊ1
uint16_t exciterID;			//exciterID�Ǳ�����ʱ�����ļ�����IDֵ
uint8_t exciterType;					//�жϳ���������ʲô���ͣ�����ͨ�����á�������
uint16_t lastRID;							//��¼�ϴ��յ��ĸ������İ�������û�õ�
uint8_t period;								//������붴�����������ڵ�ͨ�ŵ����ڣ�Ĭ��Ϊ5s�����ղ�������ʱ���������

uint64_t temp_time = 0;

//uint8_t event_flag = 0;
enum card_state c_state = synchronizing;	//Ĭ��Ϊ����ͬ��״̬����δͬ��������˯�ߵ�
uint8_t enCount, exCount;				//�����ڵ�r_entrance��r_exit״̬�£��Ѿ����������˵İ�����
uint8_t PconSignal, PnoSignal, CnoSignal;		//�ֱ������pending״̬���������յ��������ڸ�������pending״̬��������û�յ��������ڸ�ʣ��
//��in_cave״̬��������û�յ��������ڸ���
int ms_offset;									//�����ڵ�r_entrance��r_exit״̬�£����ͼ��������ӳ٣���Χ��0-99ms
int ms_offset_payoff;						//�����ڵ�r_entrance��r_exit״̬�£��¸�����Ҫ���ص��ϸ�������ɵ�ƫ�ֵΪms_offset-50������Ϊ��ֵ
uint16_t battery;								//��ȡ���Ŀ��ĵ�ǰ�������ڸñ�����
uint8_t HnoSignal;							//����״̬�£���־һ����������û���յ����������յ���Ϊ0��û�յ�Ϊ1
uint8_t pending_received_flag, in_cave_received_flag;		//��������������ʷ�������ݣ���ʱ���ڲ��Բ�˯��״̬���ܲ��ܻ��ѣ��緢�ֲ���Ҫ��ɾ
int tDelay, tDelayValid;	//�յ���ʱ��rx_pkt��1��������һ����rx_pkt��1��tDelayValid�ڲ���Ҫ��Systickʱ��Ҫ��֤��Ϊ0
uint8_t cnt;			//�����ӵ���Ƶ״̬�л��������������3�ζ�û�й������˳�ѭ������ֹ����һֱ�в���������ѭ��
//uint16_t sys_panID;
extern struct time_info pkt_time_struct, pkt_cave_struct;		//�����յ��İ�Ҳ������ṹ�壬��ʱreceiverID��ʾ�������ڵ�ID��exciterID��Ч
extern uint16_t  id, panid;
extern uint8_t channel_2_4G;
extern uint32_t packet_received;
extern CFG_PARAS cfg_paras;
extern BASIC_RF_RX_INFO rfRxInfo;
extern uint16_t lastEID;
extern uint8_t rssi;					//��Ƶ���ź�ǿ��ֵ
extern uint8_t rssi1, rssi2;					//�����������ߵ�edֵ
extern uint16_t ex_id;
extern uint16_t ex_crc;
extern uint8_t false_wake_up;		//������ʱ����⵽������ID������2λcrcУ�鲻ƥ��ʱ����λ��1
extern uint16_t config_item[CONFIG_PARA_NUMBER];				//����ʱ���ݴ����ò���������
extern uint64_t cur_time;
extern uint64_t next_wakeup_time;
extern uint32_t read_alarm1;
extern uint32_t read_setcap;

uint64_t idle_cnt = 0;

void send_test_pkg(void);
void send_test_pkg_a(void) ;
void send_test_pkg_b(void) ;
void send_test_pkg_c(void) ;
void send_test_pkg_d(void) ;
extern uint8_t frame[127];

int main(void)
{
	int i;
	system_init_from_reset();
	test_num = 0;
	test_num = SI32_RSTSRC_A_get_last_reset_source(SI32_RSTSRC_0);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3,0x0002);
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3,0x0002);
	led_start();
	read_parameter_from_flash(config_item);
	if(config_item[CONFIG_PARA_NUMBER - 1] == SYS_PARA_MAGIC_NUM)		//ħ���ж����ò����Ƿ��Ѿ���flash����
			get_cfg_paras(config_item); 	//����Ѿ����ˣ���ȡ
		else{
			get_cfg_default(config_item);		//���û�У�дĬ��ֵ��flash�У�����������ħ��
			write_sys_info_to_flash(config_item);
		}	
	pmu_init();
	//timeout_init();
	
	channel_2_4G = cfg_paras.cfg_chl;
	channel_2_4G = 12;
	panid = DEFAULT_PANID;
	id = cfg_paras.cfg_tgid;
	mac_init();
	send_test_pkg();
	
//	mac_test();
	as3933_init();
//	label_test();


//	rtc_init();
	pmu_enter_sleep();
		
	while (1)
	{
		idle_cnt++;
		if(false_wake_up == 1){			//�ٻ��ѣ���ID��crc��ƥ��
				num = 1;
				num2 = 1;
			false_wake_up = 0;
			// 	send_test_pkg_before_sleep(2);
			pmu_enter_sleep();
			continue;
/******************************* 1 �յ������ź� **********************************/			
		}	
		if(wake==1) {					//�滽�ѣ�����ȷ�ļ�����ID���������е��Զ��庯������event_exciter.c��
			num = 2;
			
			wake = 0; 
			exciterID = ex_id & 0xffff;
			exciterType = exciterTypeAnalyze(exciterID);		//���ݼ�����ID�ж���ʲô���͵�
			switch(exciterType){
				case EX_TYPE_GENERAL:		//��ͨ
					if(exciterID != lastEID){		//���������ID���Ǳ����϶��ģ����ս���������⣬�����쳣������綴�߿����л���
					//	sys_panID = DEFAULT_PANID;
						channel_2_4G = cfg_paras.cfg_chl;
						panid = DEFAULT_PANID;
						id = cfg_paras.cfg_tgid;
						mac_init();
//						basicRF231Init(cfg_paras.cfg_chl, sys_panID, cfg_paras.cfg_tgid);								
//						basicProtocolInit(rx_buffer, cfg_paras.cfg_chl, sys_panID, cfg_paras.cfg_tgid);
						rssi1 = as3933_hal_register_read(0x0a) & 0x1f;
						rssi2 = as3933_hal_register_read(0x0b) & 0x1f;
						
						cnt = 0;
						while(1) {
							rf2xx_tat_set_trx_state(RX_ON);
							if (rf2xx_tat_get_trx_state() == RX_ON)
									break;
							else if (cnt++ >= 3)
									break;
						}
						switch(c_state){		//����״̬
							case synchronizing: 		//��һ���붴�����
								lastEID = exciterID;					//����ID����֤�´��ٱ������������ٵ�����
								tDelay = cfg_paras.cfg_wtim;	//�������Լ�������ͬ������β��Ϊ20
								tDelayValid = 1;							//��Ҫ��Systick��ʱ
								num2 = 21;
								break;
							case pending: 
								if(lastEID != exciterID){		//�쳣��������߼�����л�
									lastEID = exciterID;
									tDelay = cfg_paras.cfg_wtim;
									tDelayValid = 1;
									c_state = synchronizing;	//״̬�ص�δͬ��
								}
								num2 = 22;
								break;
							case in_cave: 
								if(lastEID != exciterID){		//�����쳣��������߼�������л�
									lastEID = exciterID;
									tDelay = cfg_paras.cfg_wtim;
									tDelayValid = 1;
									c_state = synchronizing;
								}
								num2 = 23;
								break;
							default: 
								num2 = 24;
								break;
							}						
						}
						else{
							num2 = 25;
						if(tDelayValid == 0){
// 							RTC_GetTime(RTC_Format_BIN, &temp_time);
// 							RTC_GetDate(RTC_Format_BIN, &temp_date);
// 							get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
// 							temp_time_long = mktime_new(&temp_date_time);
// 							temp_time_long = temp_time_long << 2;
// 							temp_time_long = temp_time_long & ((temp_sub_time >> 8) | 0xfffffffc);
// 							
// 							RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
// 							get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime);
// 							temp_alarm_time_long = mktime_new(&temp_alarm_date_time);
// 							temp_alarm_time_long = temp_alarm_time_long << 2;
// 							temp_alarm_sub_time = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 							temp_alarm_time_long = temp_alarm_time_long & ((temp_alarm_sub_time >> 8) | 0xfffffffc);
							
// 							if(temp_alarm_time_long < temp_time_long)			//����һ���쳣�����ǵ�ǰʱ���������RTC����ʱ�䣬��ֱ������
// 								NVIC_SystemReset();
// 							LED2_Close();
// 	send_test_pkg_before_sleep(3);
							Systick_off();
							pmu_enter_sleep();
						}else
						{
							num2 = 26;
						}
					}
					break;
				case EX_TYPE_CONFIG: 		//����������Ϊ����
					wdtimer_stop();
					channel_2_4G = 12;
					panid = DEFAULT_PANID;
					id = cfg_paras.cfg_tgid;					
					mac_init();
					
//					basicRF231Init(12, DEFAULT_PANID, cfg_paras.cfg_tgid);
//					basicProtocolInit(rx_buffer, 12, DEFAULT_PANID, cfg_paras.cfg_tgid);
//					enable_rf_interrupt();
					while(1) {
						rf2xx_tat_set_trx_state(RX_ON);
						if (rf2xx_tat_get_trx_state() == RX_ON)
								break;
						else if (cnt++ >= 3)
								break;
					}
//					send_start_config_flag_pkt();
					tDelay = SYN_CONFIG_TIME;
					tDelayValid = 1;
					num2 = 29;
					c_state = config;
					break;
				case EX_TYPE_TEST: 
					i = 10;
					channel_2_4G = 12;
					panid = DEFAULT_PANID;
					id = cfg_paras.cfg_tgid;
					mac_init();
//					basicRF231Init(12, DEFAULT_PANID, cfg_paras.cfg_tgid);
//					basicProtocolInit(rx_buffer, 12, DEFAULT_PANID, cfg_paras.cfg_tgid);
					while(i--) {
						send_test_pkt(exciterID);
					}
// 	send_test_pkg_before_sleep(4);
					pmu_enter_sleep();
					break;
				case EX_TYPE_EN_EX:
					if(exciterID != lastEID){		//��֤���ᱻͬһ������һֱ����
						lastEID = exciterID;
						enCount = 0;							//��ʼ��������
						c_state = r_entrance;
						ms_offset = rand() % 100;		//������ֵ
						rtc_alarm_relative_add_item(cfg_paras.cfg_edpt + 50 - ms_offset,&time_waking);
//						en_ex_alarm_set(cfg_paras.cfg_edpt + 50 - ms_offset);	// 200 + 50 - (0~100)		//�������ӣ�Ϊ��ǰʱ����������ڵ�ֵ
//						en_ex_alarm_set(200);	// 200 + 50 - (0~100)
						ms_offset_payoff = ms_offset - 50;	//�����´β�����ֵ����ΧΪ-50~50
						channel_2_4G = cfg_paras.cfg_chl;
						panid = DEFAULT_PANID;
						id = cfg_paras.cfg_tgid;
						mac_init();
//						basicRF231Init(cfg_paras.cfg_chl, DEFAULT_PANID, cfg_paras.cfg_tgid);								
//						basicProtocolInit(rx_buffer, cfg_paras.cfg_chl, DEFAULT_PANID, cfg_paras.cfg_tgid);
						send_en_pkt();			//���ͽ�������Ŀǰ����һЩ������Ϣ���Ժ���Э�鰴��Э����
						enCount++;
// 	send_test_pkg_before_sleep(5);
						pmu_enter_sleep();
					} else {
						HnoSignal = 0;			//��ͬһ���������������ñ�־���㣬ֻ��hrt״̬���õ���
//						LED2_Close();
// 	send_test_pkg_before_sleep(6);
						pmu_enter_sleep();
					}
					break;
				default: 
//					if(tDelayValid == 0){
//					LED2_Close();
// 	send_test_pkg_before_sleep(7);
					pmu_enter_sleep();
//					}
					break;
					}
				}
/******************************* 2  ��������ʱ **********************************/
// 		#if 0
		if(tDelay < 0 && tDelayValid==1){			//Systick��ʱ���������յİ�û�յ�
			//����Ӧ״̬��ʱ��ִ�����в���
			num = 3;
			
			switch(c_state){
				case synchronizing:		//�յ��˼����źţ���û�յ�����������ͬ����
					lastEID = 0;	//NULL
					tDelayValid = 0;
//					LED1_Close();
// 	send_test_pkg_before_sleep(8);		
					num2 = 31;				
					pmu_enter_sleep();
					break;
				case pending:					//û�յ��������ڵ�İ�����������β��ΪBB�Ĳ��԰�
					num2 = 32;
					if(PnoSignal < cfg_paras.cfg_xcnt) {
						if(pending_received_flag == 1) {
							pending_received_flag = 0;
						}	else {
							PconSignal = 0;
							PnoSignal++;
//							alarm_shift();	//�����ǰ��´λ���ʱ����ǰ5ms�������յ����ĸ���
						}
					} else {
						lastEID = 0;			//PnoSignal�Ѵ���ֵ���ص�δͬ��״̬
						lastRID = 0;
						c_state = synchronizing;
					}
//test
					read_setcap = get_rtc_setcap();
					read_alarm1 = SI32_RTC_B_read_alarm1(SI32_RTC_0);
					cur_time = get_current_time();
					send_test_pkg_a();

					tDelayValid = 0;
//					LED1_Close();
// 	send_test_pkg_before_sleep(9);
// 					while(1)
// 					{
 						num3 = 1;
						num4 = 1;
						pmu_enter_sleep();
						num4 = 0;
//  					}
					
					num2 = num2;
					break;
				case in_cave:
					num2 = 33;
					if(CnoSignal < cfg_paras.cfg_ocnt) {
						if(in_cave_received_flag == 1) {
							in_cave_received_flag = 0;
						}	else {
							CnoSignal++;
//							alarm_shift();
						}
					}	else {
						lastEID = 0;
						lastRID = 0;
						CnoSignal = 0;
						c_state = synchronizing;
					}
//test
//					send_test_pkg_bb();					
					tDelayValid = 0;
//					LED1_Close();
// 	send_test_pkg_before_sleep(10);
					pmu_enter_sleep();
					break;
				case config:
					num2 = 34;
					tDelayValid = 0;
					//�˳�����ģʽ����������ԭ
					NVIC_SystemReset();
					break;
				default: 
					num2 = 35;
					break;
			}
		}
/******************************* 3 ������Ĵ��� **********************************/
		if(packet_received>0){	//receive a packet
			num = 4;
			
//			read_group(&rfRxInfo);
			mac_parze_packet(&rfRxInfo);
			switch(c_state){
				case synchronizing:				//�յ��˼����źź��ͬ������β��Ϊ20��������
					num2 = 41;
					if(parse_rx_syn_pkt() == PKT_MATCH) {
						if (exciterID == pkt_time_struct.receiverID) {
							panid = pkt_time_struct.panID;
							rf2xx_tat_set_pan_id(panid);	//����panid����֤�������յ��������ߵİ�
						} else
							break;
						time_synchronizing(c_state);				//ʱ��ͬ������		
						temp_time = get_current_time();
						c_state = pending;			//�л�״̬
						PconSignal = 0;					//��ʼ����������
						PnoSignal = 0;
						tDelayValid = 0;
//test
//						send_test_pkg_cc();

//						LED1_Close();
// 	send_test_pkg_before_sleep(11);
						pmu_enter_sleep();			
					}							
					break;
				case pending:		//���ڽ�������������
					num2 = 42;
					if(rfRxInfo.ed > cfg_paras.cfg_trssi) {				//pending��in_cave״̬�¶�Ҫ���ж��յ�����rssiֵ�Ƿ�������Ҫ��ֵ��Ŀǰ�������Ϊ0
						if(PconSignal < cfg_paras.cfg_icnt) {		//�����յ����ĸ�����û�ﵽ��ֵ
							if(parse_rx_cave_pkt() == PKT_MATCH) {						//����β��Ϊ30�İ��������������ڵ�
								lastRID = pkt_cave_struct.receiverID;
								time_synchronizing(c_state);				//���������ڵ�����ʱ��ͬ��
								temp_time = get_current_time();
								PnoSignal = 0;											//�������δ�յ����ı�־
								PconSignal++; 											//�����յ���������1
								pending_received_flag = 1;
								tDelayValid = 0;
//test
//								send_test_pkg_cc();

//								LED1_Close();
// 	send_test_pkg_before_sleep(12);
								pmu_enter_sleep();
							}
						}
						
					}
					else
					{
						num2 = 46;
						pmu_enter_sleep();
						
					}
					break;
				case in_cave:			//�����ˣ����������ڵ���ͨ��
					num2 = 43;
					if(rfRxInfo.ed > cfg_paras.cfg_trssi) {
						if(parse_rx_cave_pkt() == PKT_MATCH) {
							if(lastRID == pkt_cave_struct.receiverID) {
//test
// 						temp_sub_time_pre = RTC_GetSubSecond(); 
// 						RTC_GetTime(RTC_Format_BIN, &temp_time);
// 						RTC_GetDate(RTC_Format_BIN, &temp_date);
// 						get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
// 						temp_time_long_pre = mktime_new(&temp_date_time);
// 						temp_time_long_pre = temp_time_long_pre << 2;
// 								
// 						temp_alarm_sub_time = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 						RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
// 						get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime);
// 						temp_alarm_time_long = mktime_new(&temp_alarm_date_time);
// 						temp_alarm_time_long = temp_alarm_time_long << 2;
// 						temp_alarm_time_long = temp_alarm_time_long & ((temp_alarm_sub_time >> 8) | 0xfffffffc);

//end test	
						time_synchronizing(c_state);	//���������ڵ�ʱ��ͬ��

// 						temp_sub_time_cur = RTC_GetSubSecond(); 
// 						RTC_GetTime(RTC_Format_BIN, &temp_time);
// 						RTC_GetDate(RTC_Format_BIN, &temp_date);
// 						get_current_date_time_reverse(&temp_date_time, &temp_date, &temp_time);
// 						temp_time_long_cur = mktime_new(&temp_date_time);
// 						temp_time_long_cur = temp_time_long_cur << 2;

// 						temp_alarm_sub_time_cur = RTC_GetAlarmSubSecond(RTC_Alarm_A);
// 						RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &temp_alarm);
// 						get_current_date_time_reverse(&temp_alarm_date_time, &temp_date, &temp_alarm.RTC_AlarmTime);
// 						temp_alarm_time_long_cur = mktime_new(&temp_alarm_date_time);
// 						temp_alarm_time_long_cur = temp_alarm_time_long_cur << 2;
// 						temp_alarm_time_long_cur = temp_alarm_time_long_cur & ((temp_alarm_sub_time_cur >> 8) | 0xfffffffc);

// 								if ( abs((int32_t)temp_time_long_cur - (int32_t)temp_time_long_pre) > 80 )
// 									__NOP;
// 								if ( abs((int32_t)temp_alarm_time_long_cur - (int32_t)temp_time_long_cur) > 21 )
// 									__NOP;
								
								CnoSignal = 0;								//�������δ�յ����ı�־
								in_cave_received_flag = 1;
								tDelayValid = 0;
//test
// 								send_test_pkg_cc();
//							 LED1_Close();
// 	send_test_pkg_before_sleep(13);
								pmu_enter_sleep();
							}else
							{
								num2 = 49;
								pmu_enter_sleep();
							}
						}
						else
						{
							num2 = 48;
							pmu_enter_sleep();
							}
					}else 
					{
						num2 = 47;
						pmu_enter_sleep();
					}
					break;
				case config:
					num2 = 44;
					if(parse_rx_update_code_pkt() == QUIT_CONFIG_MODE) {		//�����־�������ý����������˳�����ģʽ
						NVIC_SystemReset();
					}
					
					break;
				default:
					num2 = 45;
					break;
			}
			
		}
		/******************************* 4 ʱ��У׼ **********************************/
		if(time_waking == 1) {
			num = 5;
			
			if(c_state == config) {		//����ģʽ�²�������
				time_waking = 0;
				continue;
			}
//			SystemInit();									
//			Systick_Init();		
//		SysTick_ITConfig(ENABLE);		
			time_waking = 0;					//��־���������壬��Ҫ�ȴ����������壬�����wakeҲ��һ��
			channel_2_4G = cfg_paras.cfg_chl;
			panid = DEFAULT_PANID;
			id = cfg_paras.cfg_tgid;
			mac_init();
			read_setcap = get_rtc_setcap();
			read_alarm1 = SI32_RTC_B_read_alarm1(SI32_RTC_0);
			cur_time = get_current_time();
			send_test_pkg_b();
//			basicRF231Init(cfg_paras.cfg_chl, DEFAULT_PANID, cfg_paras.cfg_tgid);
//			basicProtocolInit(rx_buffer, cfg_paras.cfg_chl, DEFAULT_PANID, cfg_paras.cfg_tgid);
//test
//			send_test_pkg();
						
			cnt = 0;
			while(1) {
				rf2xx_tat_set_trx_state(RX_ON);
				if (rf2xx_tat_get_trx_state() == RX_ON)
						break;
				else if (cnt++ >= 3)
						break;
			}
			switch(c_state) {
				case pending:
					num2 = 51;
					set_next_wake_time(period);	//	�������õ�ʱ�䶼������ʱ�䣬���õ�ʱ��ֻ����û���յ�30��β��ͬ����ʱ�Ż�������
					if(PconSignal < cfg_paras.cfg_icnt) {
						num2 = 57;
						tDelay = cfg_paras.cfg_ltim;
						tDelayValid = 1;
						
					}	else {
						num2 = 58;
						c_state = in_cave;
						tDelay = cfg_paras.cfg_ltim;
						tDelayValid = 1;
						CnoSignal = 0;
					}
					break;
				case in_cave:
					num2 = 52;
					set_next_wake_time(period);	//
					tDelay = cfg_paras.cfg_ltim;
					tDelayValid = 1;					
					break;
				case r_entrance:
					num2 = 53;
					if(enCount < cfg_paras.cfg_edpc) {
						ms_offset = rand() % 100;
//test
//						en_ex_alarm_set(200);
						rtc_alarm_relative_add_item(cfg_paras.cfg_edpt + 50 - ms_offset,&time_waking);
//						en_ex_alarm_set(ms_offset_payoff + cfg_paras.cfg_edpt + 50 - ms_offset);	// 200 + 50 - (0~100)		//����������õ����´λ��ѵĺ���ʱ����
						ms_offset_payoff = ms_offset - 50;	//��cfg_paras.cfg_edptΪ��׼���ϸ��������ֵ�����ٲ�
						send_en_pkt();		//��5���ֽ���40Ϊ��־
						enCount++;
//					send_test_pkg_before_sleep(14);
						pmu_enter_sleep();
					}	else {
						c_state = en_heart_ex;
						set_next_wake_time(cfg_paras.cfg_hpt);	//���������͵�ʱ�����Ƚϳ�������������λ
						HnoSignal = 1;
					}
					break;
				case en_heart_ex:
					num2 = 54;
					if(HnoSignal == 0) {		//received exciting signal in last 5 seconds
						set_next_wake_time(cfg_paras.cfg_hpt);
						HnoSignal = 1;
						send_hrt_pkt();		//β����50
	//send_test_pkg_before_sleep(15);
						pmu_enter_sleep();
					} else {
						exCount = 0;
						c_state = r_exit;
						ms_offset = rand() % 100;
						rtc_alarm_relative_add_item(cfg_paras.cfg_edpt + 50 - ms_offset,&time_waking);
//						en_ex_alarm_set(cfg_paras.cfg_edpt + 50 - ms_offset);	// 200 + 50 - (0~100)
//test
//						en_ex_alarm_set(200);
						
						ms_offset_payoff = ms_offset - 50;
						send_ex_pkt();		//��5���ֽ���60Ϊ��־
						exCount++;
//	send_test_pkg_before_sleep(16);
						pmu_enter_sleep();
					}
					break;
				case r_exit:
					num2 = 55;
					if(exCount < cfg_paras.cfg_edpc) {
						ms_offset = rand() % 100;
//test
//						en_ex_alarm_set(200);
						rtc_alarm_relative_add_item(cfg_paras.cfg_edpt + 50 - ms_offset,&time_waking);
//						en_ex_alarm_set(ms_offset_payoff + cfg_paras.cfg_edpt + 50 - ms_offset);	// 200 + 50 - (0~100)
						ms_offset_payoff = ms_offset - 50;
						send_ex_pkt();
						exCount++;
//	send_test_pkg_before_sleep(17);
						pmu_enter_sleep();						
					} else {
						lastEID = 0;
						c_state = synchronizing;
						send_ex_pkt();
//	send_test_pkg_before_sleep(18);
						pmu_enter_sleep();						
					}
					break;
				default: 
					num2 = 56;
					c_state = synchronizing;
					tDelay = 0;
					tDelayValid = 1;
					break;
			}
		}		

	}
}


void send_test_pkg(void) {
// 	uint8_t send_count = 0;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){		
		frame[0] = c_state;

	

		frame[1] = lastEID;
		frame[2] = lastEID >> 8;
		frame[3] = lastRID;
		frame[4] = lastRID >> 8;
		frame[5] = PconSignal;
		frame[6] = PnoSignal;
		frame[7] = CnoSignal;
		frame[8] = pending_received_flag;
		frame[9] = rssi1;
		frame[10] = rssi2;
		frame[11] = exciterID;
		frame[12] = exciterID >> 8;
		frame[13] = test_num;

		
		frame[15] = 0xbb;
		
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.srcAddr = id;
		rfTxInfo.length = 30;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);		//����ʱ���������Test�ŵ��ǲ���CSMA�����ŵ��Ǽ����˵ģ�Ŀǰ�汾�����˵Ŀ����д���
// 		is_sent = 0;
// 		while(is_sent == 0){
// 			is_sent =	basicRfSendPacket(&txInfo);
// 			if (send_count++ >=2) {
// 				delay_ms(2);
// 				break;
// 			}
// 		}

		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}


void send_test_pkg_a(void) {
// 	uint8_t send_count = 0;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){		
		frame[0] = c_state;
		
		frame[1] = cur_time & 0xff;
		frame[2] = (cur_time >>8) & 0xff;
		frame[3] = (cur_time >>16) & 0xff;
		frame[4] = (cur_time >>24)& 0xff;
		frame[5] = (cur_time >>32)& 0xff;
		frame[6] = (cur_time >>40)& 0xff;
		frame[7] = (cur_time >>48)& 0xff;
		frame[8] = (cur_time >>56)& 0xff;
		frame[9] = next_wakeup_time & 0xff;
		frame[10] = (next_wakeup_time >> 8) & 0xff;
		frame[11] = (next_wakeup_time >> 16) & 0xff;
		frame[12] = (next_wakeup_time >> 24) & 0xff;
		frame[13] = (next_wakeup_time >> 32) & 0xff;
		frame[14] = (next_wakeup_time >> 40) & 0xff;
		frame[15] = (next_wakeup_time >> 48) & 0xff;
		frame[16] = (next_wakeup_time >> 56) & 0xff;
		frame[17] = read_setcap & 0xff;
		frame[18] = (read_setcap >> 8)& 0xff;
		frame[19] = (read_setcap >> 16)& 0xff;
		frame[20] = (read_setcap >> 24)& 0xff;
		frame[21] = read_alarm1 & 0xff;
		frame[22] = (read_alarm1 >> 8)& 0xff;
		frame[23] = (read_alarm1 >> 16)& 0xff;
		frame[24] = (read_alarm1 >> 24)& 0xff;
		
		frame[25] = 0xaa;
		
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.srcAddr = id;
		rfTxInfo.length = 30;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);		//����ʱ���������Test�ŵ��ǲ���CSMA�����ŵ��Ǽ����˵ģ�Ŀǰ�汾�����˵Ŀ����д���
// 		is_sent = 0;
// 		while(is_sent == 0){
// 			is_sent =	basicRfSendPacket(&txInfo);
// 			if (send_count++ >=2) {
// 				delay_ms(2);
// 				break;
// 			}
// 		}

		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}


void send_test_pkg_b(void) {
// 	uint8_t send_count = 0;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){		
		frame[0] = c_state;
		
		frame[1] = cur_time & 0xff;
		frame[2] = (cur_time >>8) & 0xff;
		frame[3] = (cur_time >>16) & 0xff;
		frame[4] = (cur_time >>24)& 0xff;
		frame[5] = (cur_time >>32)& 0xff;
		frame[6] = (cur_time >>40)& 0xff;
		frame[7] = (cur_time >>48)& 0xff;
		frame[8] = (cur_time >>56)& 0xff;
		frame[9] = next_wakeup_time & 0xff;
		frame[10] = (next_wakeup_time >> 8) & 0xff;
		frame[11] = (next_wakeup_time >> 16) & 0xff;
		frame[12] = (next_wakeup_time >> 24) & 0xff;
		frame[13] = (next_wakeup_time >> 32) & 0xff;
		frame[14] = (next_wakeup_time >> 40) & 0xff;
		frame[15] = (next_wakeup_time >> 48) & 0xff;
		frame[16] = (next_wakeup_time >> 56) & 0xff;
		frame[17] = read_setcap & 0xff;
		frame[18] = (read_setcap >> 8)& 0xff;
		frame[19] = (read_setcap >> 16)& 0xff;
		frame[20] = (read_setcap >> 24)& 0xff;
		frame[21] = read_alarm1 & 0xff;
		frame[22] = (read_alarm1 >> 8)& 0xff;
		frame[23] = (read_alarm1 >> 16)& 0xff;
		frame[24] = (read_alarm1 >> 24)& 0xff;
		
		frame[25] = 0xbb;
		
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.srcAddr = id;
		rfTxInfo.length = 30;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);		//����ʱ���������Test�ŵ��ǲ���CSMA�����ŵ��Ǽ����˵ģ�Ŀǰ�汾�����˵Ŀ����д���
// 		is_sent = 0;
// 		while(is_sent == 0){
// 			is_sent =	basicRfSendPacket(&txInfo);
// 			if (send_count++ >=2) {
// 				delay_ms(2);
// 				break;
// 			}
// 		}

		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

void send_test_pkg_c(void) {
// 	uint8_t send_count = 0;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){		
		frame[0] = c_state;
		
		frame[1] = cur_time & 0xff;
		frame[2] = (cur_time >>8) & 0xff;
		frame[3] = (cur_time >>16) & 0xff;
		frame[4] = (cur_time >>24)& 0xff;
		frame[5] = (cur_time >>32)& 0xff;
		frame[6] = (cur_time >>40)& 0xff;
		frame[7] = (cur_time >>48)& 0xff;
		frame[8] = (cur_time >>56)& 0xff;
		frame[9] = next_wakeup_time & 0xff;
		frame[10] = (next_wakeup_time >> 8) & 0xff;
		frame[11] = (next_wakeup_time >> 16) & 0xff;
		frame[12] = (next_wakeup_time >> 24) & 0xff;
		frame[13] = (next_wakeup_time >> 32) & 0xff;
		frame[14] = (next_wakeup_time >> 40) & 0xff;
		frame[15] = (next_wakeup_time >> 48) & 0xff;
		frame[16] = (next_wakeup_time >> 56) & 0xff;
		frame[17] = read_setcap & 0xff;
		frame[18] = (read_setcap >> 8)& 0xff;
		frame[19] = (read_setcap >> 16)& 0xff;
		frame[20] = (read_setcap >> 24)& 0xff;
		frame[21] = read_alarm1 & 0xff;
		frame[22] = (read_alarm1 >> 8)& 0xff;
		frame[23] = (read_alarm1 >> 16)& 0xff;
		frame[24] = (read_alarm1 >> 24)& 0xff;

		
		frame[25] = 0xcc;
		
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.srcAddr = id;
		rfTxInfo.length = 30;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);		//����ʱ���������Test�ŵ��ǲ���CSMA�����ŵ��Ǽ����˵ģ�Ŀǰ�汾�����˵Ŀ����д���
// 		is_sent = 0;
// 		while(is_sent == 0){
// 			is_sent =	basicRfSendPacket(&txInfo);
// 			if (send_count++ >=2) {
// 				delay_ms(2);
// 				break;
// 			}
// 		}

		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

void send_test_pkg_d(void) {
// 	uint8_t send_count = 0;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){		
		frame[0] = c_state;
		
		frame[1] = cur_time & 0xff;
		frame[2] = (cur_time >>8) & 0xff;
		frame[3] = (cur_time >>16) & 0xff;
		frame[4] = (cur_time >>24)& 0xff;
		frame[5] = (cur_time >>32)& 0xff;
		frame[6] = (cur_time >>40)& 0xff;
		frame[7] = (cur_time >>48)& 0xff;
		frame[8] = (cur_time >>56)& 0xff;
		frame[9] = next_wakeup_time & 0xff;
		frame[10] = (next_wakeup_time >> 8) & 0xff;
		frame[11] = (next_wakeup_time >> 16) & 0xff;
		frame[12] = (next_wakeup_time >> 24) & 0xff;
		frame[13] = (next_wakeup_time >> 32) & 0xff;
		frame[14] = (next_wakeup_time >> 40) & 0xff;
		frame[15] = (next_wakeup_time >> 48) & 0xff;
		frame[16] = (next_wakeup_time >> 56) & 0xff;
		frame[17] = read_setcap & 0xff;
		frame[18] = (read_setcap >> 8)& 0xff;
		frame[19] = (read_setcap >> 16)& 0xff;
		frame[20] = (read_setcap >> 24)& 0xff;
		frame[21] = read_alarm1 & 0xff;
		frame[22] = (read_alarm1 >> 8)& 0xff;
		frame[23] = (read_alarm1 >> 16)& 0xff;
		frame[24] = (read_alarm1 >> 24)& 0xff;
		
		frame[25] = 0xdd;
		
		rfTxInfo.destPanId = 0xffff;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.srcAddr = id;
		rfTxInfo.length = 30;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);		//����ʱ���������Test�ŵ��ǲ���CSMA�����ŵ��Ǽ����˵ģ�Ŀǰ�汾�����˵Ŀ����д���
// 		is_sent = 0;
// 		while(is_sent == 0){
// 			is_sent =	basicRfSendPacket(&txInfo);
// 			if (send_count++ >=2) {
// 				delay_ms(2);
// 				break;
// 			}
// 		}

		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

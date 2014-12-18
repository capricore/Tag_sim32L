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
uint8_t wake, time_waking = 0;		//标志位，中断中置为1，唤醒CPU的条件是两个中有一个为1
uint16_t exciterID;			//exciterID是被激励时读出的激励器ID值
uint8_t exciterType;					//判断出激励器是什么类型，如普通、配置、进出口
uint16_t lastRID;							//记录上次收到哪个发来的包，基本没用到
uint8_t period;								//这个是入洞后与人流量节点通信的周期，默认为5s，在收不到包的时候才起作用

uint64_t temp_time = 0;

//uint8_t event_flag = 0;
enum card_state c_state = synchronizing;	//默认为正在同步状态，即未同步，卡是睡眠的
uint8_t enCount, exCount;				//进出口的r_entrance和r_exit状态下，已经连续发送了的包个数
uint8_t PconSignal, PnoSignal, CnoSignal;		//分别代表：在pending状态下已连续收到包的周期个数；在pending状态下已连续没收到包的周期个剩，
//在in_cave状态下已连续没收到包的周期个数
int ms_offset;									//进出口的r_entrance和r_exit状态下，发送间隔的随机延迟，范围是0-99ms
int ms_offset_payoff;						//进出口的r_entrance和r_exit状态下，下个周期要补回的上个周期造成的偏差，值为ms_offset-50，可以为负值
uint16_t battery;								//获取到的卡的当前电量存在该变量中
uint8_t HnoSignal;							//心跳状态下，标志一个周期内有没有收到过激励，收到过为0，没收到为1
uint8_t pending_received_flag, in_cave_received_flag;		//这两个变量是历史遗留数据，当时用于测试不睡眠状态下能不能唤醒，如发现不需要可删
int tDelay, tDelayValid;	//收到包时，rx_pkt加1，处理完一个，rx_pkt减1；tDelayValid在不需要用Systick时，要保证设为0
uint8_t cnt;			//徐宏添加的射频状态切换次数计数，如果3次都没切过来，退出循环，防止出现一直切不过来的死循环
//uint16_t sys_panID;
extern struct time_info pkt_time_struct, pkt_cave_struct;		//洞内收到的包也用这个结构体，此时receiverID表示人流量节点ID，exciterID无效
extern uint16_t  id, panid;
extern uint8_t channel_2_4G;
extern uint32_t packet_received;
extern CFG_PARAS cfg_paras;
extern BASIC_RF_RX_INFO rfRxInfo;
extern uint16_t lastEID;
extern uint8_t rssi;					//射频的信号强度值
extern uint8_t rssi1, rssi2;					//两个激励天线的ed值
extern uint16_t ex_id;
extern uint16_t ex_crc;
extern uint8_t false_wake_up;		//被唤醒时，检测到激励器ID与它的2位crc校验不匹配时，该位置1
extern uint16_t config_item[CONFIG_PARA_NUMBER];				//配置时，暂存配置参数的数组
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
	if(config_item[CONFIG_PARA_NUMBER - 1] == SYS_PARA_MAGIC_NUM)		//魔数判断配置参数是否已经在flash中了
			get_cfg_paras(config_item); 	//如果已经有了，读取
		else{
			get_cfg_default(config_item);		//如果没有，写默认值到flash中，包括了设置魔数
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
		if(false_wake_up == 1){			//假唤醒，即ID与crc不匹配
				num = 1;
				num2 = 1;
			false_wake_up = 0;
			// 	send_test_pkg_before_sleep(2);
			pmu_enter_sleep();
			continue;
/******************************* 1 收到激励信号 **********************************/			
		}	
		if(wake==1) {					//真唤醒，被正确的激励器ID激励，其中的自定义函数多在event_exciter.c中
			num = 2;
			
			wake = 0; 
			exciterID = ex_id & 0xffff;
			exciterType = exciterTypeAnalyze(exciterID);		//根据激励器ID判断是什么类型的
			switch(exciterType){
				case EX_TYPE_GENERAL:		//普通
					if(exciterID != lastEID){		//如果激励器ID不是本来认定的，除刚进洞的情况外，都是异常情况（如洞窟快速切换）
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
						switch(c_state){		//三种状态
							case synchronizing: 		//第一次入洞的情况
								lastEID = exciterID;					//设置ID，保证下次再被激励到不会再到这里
								tDelay = cfg_paras.cfg_wtim;	//监听来自激励器的同步包，尾号为20
								tDelayValid = 1;							//需要用Systick计时
								num2 = 21;
								break;
							case pending: 
								if(lastEID != exciterID){		//异常情况，洞窟间快速切换
									lastEID = exciterID;
									tDelay = cfg_paras.cfg_wtim;
									tDelayValid = 1;
									c_state = synchronizing;	//状态回到未同步
								}
								num2 = 22;
								break;
							case in_cave: 
								if(lastEID != exciterID){		//更加异常情况，洞窟间更快速切换
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
							
// 							if(temp_alarm_time_long < temp_time_long)			//处理一个异常，就是当前时间如果超过RTC闹钟时间，就直接重启
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
				case EX_TYPE_CONFIG: 		//配置器类型为配置
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
					if(exciterID != lastEID){		//保证不会被同一激励器一直激励
						lastEID = exciterID;
						enCount = 0;							//初始化发包数
						c_state = r_entrance;
						ms_offset = rand() % 100;		//获得随机值
						rtc_alarm_relative_add_item(cfg_paras.cfg_edpt + 50 - ms_offset,&time_waking);
//						en_ex_alarm_set(cfg_paras.cfg_edpt + 50 - ms_offset);	// 200 + 50 - (0~100)		//设置闹钟，为当前时间加上括号内的值
//						en_ex_alarm_set(200);	// 200 + 50 - (0~100)
						ms_offset_payoff = ms_offset - 50;	//用于下次补偿的值，范围为-50~50
						channel_2_4G = cfg_paras.cfg_chl;
						panid = DEFAULT_PANID;
						id = cfg_paras.cfg_tgid;
						mac_init();
//						basicRF231Init(cfg_paras.cfg_chl, DEFAULT_PANID, cfg_paras.cfg_tgid);								
//						basicProtocolInit(rx_buffer, cfg_paras.cfg_chl, DEFAULT_PANID, cfg_paras.cfg_tgid);
						send_en_pkt();			//发送进洞包，目前包含一些测试信息，以后定了协议按照协议来
						enCount++;
// 	send_test_pkg_before_sleep(5);
						pmu_enter_sleep();
					} else {
						HnoSignal = 0;			//被同一激励器激励，将该标志清零，只在hrt状态下用到它
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
/******************************* 2  激励后处理超时 **********************************/
// 		#if 0
		if(tDelay < 0 && tDelayValid==1){			//Systick计时到，即想收的包没收到
			//当相应状态超时后，执行下列操作
			num = 3;
			
			switch(c_state){
				case synchronizing:		//收到了激励信号，但没收到激励器发的同步包
					lastEID = 0;	//NULL
					tDelayValid = 0;
//					LED1_Close();
// 	send_test_pkg_before_sleep(8);		
					num2 = 31;				
					pmu_enter_sleep();
					break;
				case pending:					//没收到人流量节点的包，发送下面尾号为BB的测试包
					num2 = 32;
					if(PnoSignal < cfg_paras.cfg_xcnt) {
						if(pending_received_flag == 1) {
							pending_received_flag = 0;
						}	else {
							PconSignal = 0;
							PnoSignal++;
//							alarm_shift();	//功能是把下次唤醒时间提前5ms，增加收到包的概率
						}
					} else {
						lastEID = 0;			//PnoSignal已达限值，回到未同步状态
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
					//退出配置模式并将参数还原
					NVIC_SystemReset();
					break;
				default: 
					num2 = 35;
					break;
			}
		}
/******************************* 3 激励后的处理 **********************************/
		if(packet_received>0){	//receive a packet
			num = 4;
			
//			read_group(&rfRxInfo);
			mac_parze_packet(&rfRxInfo);
			switch(c_state){
				case synchronizing:				//收到了激励信号后的同步包（尾号为20），解析
					num2 = 41;
					if(parse_rx_syn_pkt() == PKT_MATCH) {
						if (exciterID == pkt_time_struct.receiverID) {
							panid = pkt_time_struct.panID;
							rf2xx_tat_set_pan_id(panid);	//设置panid，保证不会再收到其它洞窟的包
						} else
							break;
						time_synchronizing(c_state);				//时间同步函数		
						temp_time = get_current_time();
						c_state = pending;			//切换状态
						PconSignal = 0;					//初始化两个参数
						PnoSignal = 0;
						tDelayValid = 0;
//test
//						send_test_pkg_cc();

//						LED1_Close();
// 	send_test_pkg_before_sleep(11);
						pmu_enter_sleep();			
					}							
					break;
				case pending:		//正在进洞的正常过程
					num2 = 42;
					if(rfRxInfo.ed > cfg_paras.cfg_trssi) {				//pending和in_cave状态下都要先判断收到包的rssi值是否大于最低要求值，目前最低设置为0
						if(PconSignal < cfg_paras.cfg_icnt) {		//连续收到包的个数还没达到限值
							if(parse_rx_cave_pkt() == PKT_MATCH) {						//解析尾号为30的包，来自人流量节点
								lastRID = pkt_cave_struct.receiverID;
								time_synchronizing(c_state);				//与人流量节点再做时间同步
								temp_time = get_current_time();
								PnoSignal = 0;											//清除连续未收到包的标志
								PconSignal++; 											//连续收到包个数加1
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
				case in_cave:			//进洞了，与人流量节点做通信
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
						time_synchronizing(c_state);	//与人流量节点时间同步

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
								
								CnoSignal = 0;								//清除连续未收到包的标志
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
					if(parse_rx_update_code_pkt() == QUIT_CONFIG_MODE) {		//这个标志表明配置结束，主动退出配置模式
						NVIC_SystemReset();
					}
					
					break;
				default:
					num2 = 45;
					break;
			}
			
		}
		/******************************* 4 时间校准 **********************************/
		if(time_waking == 1) {
			num = 5;
			
			if(c_state == config) {		//配置模式下不做事情
				time_waking = 0;
				continue;
			}
//			SystemInit();									
//			Systick_Init();		
//		SysTick_ITConfig(ENABLE);		
			time_waking = 0;					//标志必须马上清，不要等处理完了再清，上面的wake也是一样
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
					set_next_wake_time(period);	//	这里设置的时间都是整秒时间，设置的时间只有在没有收到30结尾的同步包时才会有作用
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
//						en_ex_alarm_set(ms_offset_payoff + cfg_paras.cfg_edpt + 50 - ms_offset);	// 200 + 50 - (0~100)		//这个函数设置的是下次唤醒的毫秒时间间隔
						ms_offset_payoff = ms_offset - 50;	//以cfg_paras.cfg_edpt为基准，上个周期随机值多退少补
						send_en_pkt();		//第5个字节是40为标志
						enCount++;
//					send_test_pkg_before_sleep(14);
						pmu_enter_sleep();
					}	else {
						c_state = en_heart_ex;
						set_next_wake_time(cfg_paras.cfg_hpt);	//心跳包发送的时间间隔比较长，用整秒作单位
						HnoSignal = 1;
					}
					break;
				case en_heart_ex:
					num2 = 54;
					if(HnoSignal == 0) {		//received exciting signal in last 5 seconds
						set_next_wake_time(cfg_paras.cfg_hpt);
						HnoSignal = 1;
						send_hrt_pkt();		//尾号是50
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
						send_ex_pkt();		//第5个字节是60为标志
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
		mac_send_packet_basic(&rfTxInfo,5000);		//发送时，后面带着Test着的是不加CSMA，带着的是加上了的，目前版本加上了的可能有错误
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
		mac_send_packet_basic(&rfTxInfo,5000);		//发送时，后面带着Test着的是不加CSMA，带着的是加上了的，目前版本加上了的可能有错误
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
		mac_send_packet_basic(&rfTxInfo,5000);		//发送时，后面带着Test着的是不加CSMA，带着的是加上了的，目前版本加上了的可能有错误
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
		mac_send_packet_basic(&rfTxInfo,5000);		//发送时，后面带着Test着的是不加CSMA，带着的是加上了的，目前版本加上了的可能有错误
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
		mac_send_packet_basic(&rfTxInfo,5000);		//发送时，后面带着Test着的是不加CSMA，带着的是加上了的，目前版本加上了的可能有错误
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

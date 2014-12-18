#include "crc.h"
#include "configuration.h"
#include "rf2xx_include.h"
#include "event_receive_pkt.h"
#include "rf2xx_protoc.h"
#include "rf2xx_send.h"
#include "systick.h"
#include "as3933_include.h"
#include "SI32_RTC_B_Type.h"
#include "string.h"
#include "FLASHCTRL.h"
#define __at(_addr) __attribute__ ((at(_addr)))

#define USER_APP_START_ADDR 0X1400
#define USER_CODE_STORE_ADDR	0X8400
#define USER_BOOT_INFO_ADDR		0Xf400

struct time_info pkt_time_struct, pkt_cave_struct;

//const uint16_t current_software_version __at(0x0000fe00) =  SOFTWARE_VERSION;
uint16_t current_software_version = 0x1105;

uint8_t  write_flash_ok[350] = {1,0,0,};
uint32_t error_piece_num = 4;
uint32_t write_flash_time_out;
int is_sent;
uint8_t frame[127] ;
#define __at(_addr) __attribute__ ((at(_addr)))
uint16_t reboot_flag;
uint32_t boot_len;

// static uint8_t	recv_updata_req = 0;//代码更新求情标志位
// static uint16_t seq = 1;//包序号
// static uint16_t file_crc = INITIAL_VALUE;//要自己累加的文件crc
// static uint16_t recv_file_crc = 0;//接收到的文件crc
// static uint16_t current_piece = 0;//当前片数，（第几片代码数据）
// static uint16_t total_segments = 0;//代码总片数
// static uint32_t current_store_addr = 0;//flash当前存储位置（页数）

extern BASIC_RF_RX_INFO rfRxInfo;
extern BASIC_RF_TX_INFO rfTxInfo;
extern uint8_t frame[127];
extern uint8_t fLength;
// extern int is_sent;
extern uint32_t next_wakeup_time;
extern uint32_t long_count;

// void my_sync(uint32_t long_time, uint32_t short_time)
// {
// //	real_time temp_time;
// 	uint8_t temp_5s;
// 	uint8_t temp_1_4s;
// 	uint32_t temp_long_time;
// 	uint32_t rtc;
// //	real_time current_time = get_real_time();
// 	ENTER_CRITICAL_REGION();
// 	temp_1_4s = long_time%4;
// 	temp_long_time = long_time - temp_1_4s;
// 	temp_long_time >>= 2;
// 	temp_long_time += 0x40000000;
// 	temp_5s = temp_long_time%5;
// 	long_count = long_time - temp_5s;
// 	rtc = temp_5s<<2 + temp_1_4s;
// 	rtc <<= 13;
// 	SI32_RTC_B_start_timer_set(SI32_RTC_0);
// 	SI32_RTC_B_write_setcap(SI32_RTC_0, rtc);
// 	while(SI32_RTC_B_is_timer_set_in_progress(SI32_RTC_0));
// 	LEAVE_CRITICAL_REGION();
// }
uint64_t next_wakeup_time_temp = 0;
uint8_t parse_rx_syn_pkt()
{
	uint8_t return_value = PKT_MATCH;
	if(rfRxInfo.pPayload[rfRxInfo.length-1] == 0x20 && rfRxInfo.pPayload[0] == 0x00){	//syn packet
		pkt_time_struct.receiverID = rfRxInfo.srcAddr;
		pkt_time_struct.panID = 0;
		pkt_time_struct.panID |= rfRxInfo.pPayload[2];
		pkt_time_struct.panID = pkt_time_struct.panID << 8;
		pkt_time_struct.panID |= rfRxInfo.pPayload[1];		
		pkt_time_struct.time = 0;
		pkt_time_struct.time |= rfRxInfo.pPayload[6];
		pkt_time_struct.time = pkt_time_struct.time << 8;
		pkt_time_struct.time |= rfRxInfo.pPayload[5];
		pkt_time_struct.time = pkt_time_struct.time << 8;
		pkt_time_struct.time |= rfRxInfo.pPayload[4];
		pkt_time_struct.time = pkt_time_struct.time << 8;
		pkt_time_struct.time |= rfRxInfo.pPayload[3];
		pkt_time_struct.subtime = rfRxInfo.pPayload[7];
		pkt_time_struct.nextWTime = 0;
		pkt_time_struct.nextWTime |= rfRxInfo.pPayload[11];
		pkt_time_struct.nextWTime = pkt_time_struct.nextWTime << 8;
		pkt_time_struct.nextWTime |= rfRxInfo.pPayload[10];
		pkt_time_struct.nextWTime = pkt_time_struct.nextWTime << 8;
		pkt_time_struct.nextWTime |= rfRxInfo.pPayload[9];
		pkt_time_struct.nextWTime = pkt_time_struct.nextWTime << 8;
		pkt_time_struct.nextWTime |= rfRxInfo.pPayload[8];
		next_wakeup_time = pkt_time_struct.nextWTime;
		next_wakeup_time_temp = pkt_time_struct.nextWTime;
		pkt_time_struct.period = 0;
		pkt_time_struct.period |= rfRxInfo.pPayload[13];
		pkt_time_struct.period = pkt_time_struct.period << 8;
		pkt_time_struct.period |= rfRxInfo.pPayload[12];
		pkt_time_struct.seq = rfRxInfo.pPayload[14];
		pkt_time_struct.recv_time = rfRxInfo.recv_time;
//		my_sync(pkt_time_struct.time, pkt_time_struct.subtime);
		if (abs((int32_t)pkt_time_struct.nextWTime - (int32_t)pkt_time_struct.time) > 6*4) {
			return_value = TYPE_ERROR;
		}
		
	}
	else
		return_value = TYPE_ERROR;
	return return_value;
}

void send_syn_ack(uint8_t seq, uint16_t dest_panID, uint16_t dest_addr)
{
	int send_count = 0;
	
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = 0x01;
		frame[1] = rf2xx_tat_get_short_address() & 0xff;
		frame[2] = (rf2xx_tat_get_short_address() >> 8) & 0xff;
		frame[3] = seq;
		frame[4] = 4;
		frame[5] = 0x20;
		rfTxInfo.destPanId = dest_panID;
		rfTxInfo.destAddr = dest_addr;
		rfTxInfo.srcAddr = id;
		rfTxInfo.length = 6;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
//test NO retry
// 		basicRfSendPacketTest(&txInfo);
		is_sent = -1;
		while(is_sent != 0){
			is_sent =	mac_send_packet_basic(&rfTxInfo,5000);
			if (send_count++ >=2) {
				delay_ms(2);
				break;
			}
		}

		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}

uint8_t parse_rx_cave_pkt()
{
	uint8_t return_value = PKT_MATCH;
	if(rfRxInfo.pPayload[rfRxInfo.length-1] == 0x30 && rfRxInfo.pPayload[0] == 0x00){	//cave packe		pkt_cave_struct.receiverID = rfRxInfo.srcAddr;
		pkt_cave_struct.receiverID = rfRxInfo.srcAddr;
		pkt_cave_struct.panID = 0;
		pkt_cave_struct.panID |= rfRxInfo.pPayload[2];
		pkt_cave_struct.panID = pkt_cave_struct.panID << 8;
		pkt_cave_struct.panID |= rfRxInfo.pPayload[1];		
		pkt_cave_struct.time = 0;
		pkt_cave_struct.time |= rfRxInfo.pPayload[6];
		pkt_cave_struct.time = pkt_cave_struct.time << 8;
		pkt_cave_struct.time |= rfRxInfo.pPayload[5];
		pkt_cave_struct.time = pkt_cave_struct.time << 8;
		pkt_cave_struct.time |= rfRxInfo.pPayload[4];
		pkt_cave_struct.time = pkt_cave_struct.time << 8;
		pkt_cave_struct.time |= rfRxInfo.pPayload[3];
		pkt_cave_struct.subtime = rfRxInfo.pPayload[7];
		pkt_cave_struct.nextWTime = 0;
		pkt_cave_struct.nextWTime |= rfRxInfo.pPayload[11];
		pkt_cave_struct.nextWTime = pkt_cave_struct.nextWTime << 8;
		pkt_cave_struct.nextWTime |= rfRxInfo.pPayload[10];
		pkt_cave_struct.nextWTime = pkt_cave_struct.nextWTime << 8;
		pkt_cave_struct.nextWTime |= rfRxInfo.pPayload[9];
		pkt_cave_struct.nextWTime = pkt_cave_struct.nextWTime << 8;
		pkt_cave_struct.nextWTime |= rfRxInfo.pPayload[8];
		next_wakeup_time = pkt_time_struct.nextWTime;
		pkt_cave_struct.period = 0;
		pkt_cave_struct.period |= rfRxInfo.pPayload[13];
		pkt_cave_struct.period = pkt_cave_struct.period << 8;
		pkt_cave_struct.period |= rfRxInfo.pPayload[12];
		pkt_cave_struct.seq = rfRxInfo.pPayload[14];
		pkt_cave_struct.recv_time = rfRxInfo.recv_time;
		if (abs((int32_t)pkt_time_struct.nextWTime - (int32_t)pkt_time_struct.time) > 6*4) {
			return_value = TYPE_ERROR;
		}
		
	}
	else
		return_value = TYPE_ERROR;
	return return_value;
}
//end Cong

//chai add code start
//发送代码配置包的ack函数
//seq：包序号
//pkt_type: 包类型，代码更新请求包、代码更新数据包
//report：报告，成功、包序号错、crc错等
int send_cf_ack(uint16_t seq, uint8_t pkt_type, uint8_t report)
{
	int ret = -7;
	uint16_t ack_pkt_crc = INITIAL_VALUE;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0]  = 0x74;
		frame[1]  = 0x39;
		frame[2]  = ACK_LENGTH;
		frame[3]  = DEST_ADDR & 0xff;
		frame[4]  = (DEST_ADDR>>8) & 0xff;
		frame[5]  = rf2xx_tat_get_short_address() & 0xff;
		frame[6]  = (rf2xx_tat_get_short_address() >> 8) & 0xff;
		frame[7]  = seq & 0xff;
		frame[8]  = (seq>>8) & 0xff;
		frame[9]  = pkt_type;
		frame[10] = report;

		ack_pkt_crc = crc_calculate(ack_pkt_crc, &frame[3], ACK_LENGTH-3);
		ack_pkt_crc = crc_finish(ack_pkt_crc);
		frame[11] = ack_pkt_crc & 0xff;
		frame[12] = (ack_pkt_crc>>8) & 0xff;
		frame[13] = PROTOCOL_NUM;

		rfTxInfo.destPanId = DEST_PANID;
		rfTxInfo.destAddr = DEST_ADDR;
		rfTxInfo.length = 14;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		ret = mac_send_packet_basic(&rfTxInfo,5000);
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
	return ret;
}


//发送测试包函数
bool send_test_pkt(uint16_t exciterID) {
	int i;
	bool ret = false;

	uint8_t ch1_rssi = as3933_hal_register_read(0x0a) & 0x1F;
	uint8_t ch2_rssi = as3933_hal_register_read(0x0c) & 0x1F;
	uint8_t ch3_rssi = as3933_hal_register_read(0x0b) & 0x1F;

	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0]  = 0x74;//前导符
		frame[1]  = 0x39;
		frame[2]  = 0x0F+0x5A;//长度
		frame[3]  = DEST_ADDR & 0xff;//目标地址
		frame[4]  = (DEST_ADDR>>8) & 0xff;
		frame[5]  = rf2xx_tat_get_short_address() & 0xff;//源地址
		frame[6]  = (rf2xx_tat_get_short_address() >> 8) & 0xff;
		frame[7]  = current_software_version & 0xff;//子版本号
		frame[8]  = (current_software_version >> 8) & 0xff;//主版本号
		frame[9]  = exciterID & 0xff;//激励器ID
		frame[10]  = (exciterID>>8) & 0xff;
		frame[11] = ch1_rssi;//通道1 强度
		frame[12] = ch2_rssi;//通道2 强度
		frame[13] = ch3_rssi;//通道3 强度
		frame[14] = 0xff;
		for (i=0; i<0x5A; i++) {
			frame[15+i] = 0xff;
		}
		rfTxInfo.destPanId = DEST_PANID;
		rfTxInfo.destAddr = DEST_ADDR;
		rfTxInfo.length = 15+0x5A;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;

//		ret = basicRfSendPacket(&txInfo);
		ret = mac_send_packet_basic(&rfTxInfo,5000);
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
	return ret;
}
static uint8_t	recv_updata_req = 0;//代码更新求情标志位
static uint16_t seq = 1;//包序号
static uint16_t file_crc = INITIAL_VALUE;//要自己累加的文件crc
static uint16_t recv_file_crc = 0;//接收到的文件crc
static uint16_t current_piece = 0;//当前片数，（第几片代码数据）
static uint16_t total_segments = 0;//代码总片数
static uint32_t current_store_addr = 0;//flash当前存储位置（页数）

//参数配置及代码更新函数
int parse_rx_update_code_pkt()
{
	
	int i;
	uint16_t pkt_crc = INITIAL_VALUE;//包crc

//	static uint32_t ptr = PAGE_SIZE;//一页flash的大小
	
	//当前帧的crc
	pkt_crc = crc_calculate(pkt_crc, PKT_CRC_START_ADDR, CF_PKT_LEN-3);
	pkt_crc = crc_finish(pkt_crc);
	
	//判断是否是合法的数据帧
	if (CF_PKT_PRE != 0x3974|| CF_PKT_CRC != pkt_crc || CF_PKT_PROTOCOL_NUM != PROTOCOL_NUM)
	{
		while(2);
		return -1;

	}
		

	switch (CF_DATA_AREA_TYPE) {
		case 0:	//配置系统参数
			if(parse_rx_config_pkt() == QUIT_CONFIG_MODE)//配置参数
				return QUIT_CONFIG_MODE;
			break;

		case 1: //代码更新请求包
			//参数初始化
			file_crc = INITIAL_VALUE;
			recv_file_crc = CF_DATA_AREA_FILE_CRC;
			total_segments =  CF_DATA_AREA_TOTAL_SEQ;
// 			write_image_info_to_flash(total_segments, recv_file_crc);//从烧写时要根据image大小计算crc并与文件crc对比
			current_store_addr = USER_CODE_STORE_ADDR;
			seq = 1;
			current_piece = 1;
//			ptr = PAGE_SIZE;
			recv_updata_req = 1; //
			write_flash_ok[0] = 1;//对应每片代码数据的ack情况
			for(i=1; i<350; i++) {
				write_flash_ok[i] = 0;
			}
			if (CF_DATA_AREA_VERSION < current_software_version) {//判断软件版本号是否合法
				send_cf_ack(seq, UPDATE_ASK_PKT_REPLY_TYPE, UPDATE_ASK_SOFTWARE_VERSION_ERR);
				seq = 0;
				return -1;
			}
			if (CF_DATA_AREA_TOTAL_SEQ*PIECE_SIZE > FLASH_STORE_CODE_SIZE) {//判断总片数是否合法
				send_cf_ack(seq, UPDATE_ASK_PKT_REPLY_TYPE, UPDATE_ASK_TOTAL_SEGMENTS_ERR);
				seq = 0;
				recv_updata_req = 0;
				return -1;
			}
			send_cf_ack(seq++, UPDATE_ASK_PKT_REPLY_TYPE, UPDATE_ASK_OK);
			break;

		case 2: //代码更新代码包
			if (recv_updata_req != 1)//收到代码更新请求才进行更新代码
				return -1;
			
			if (CF_DATA_AREA_TOTAL_SEQ != total_segments) {//判断总片数是否合法
				recv_updata_req = 0;
				send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_TOTAL_SEGMENTS_ERR);
				seq = 0;
				return -1;		
			}

 			if (CF_DATA_AREA_CURRENT_PIECE != current_piece) {//判断当前片数是否合法
				seq = CF_DATA_AREA_CURRENT_PIECE+1;
				if ((CF_DATA_AREA_CURRENT_PIECE == current_piece-1)) {//若代码片数是当前片数-1，则认为是重发的包
					if (!send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_OK)) {
						seq++;
					}
					return 0;
				} else {
					send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_CURRENT_SEQMENT_ERR);
					recv_updata_req = 0;
					return -1;
				}
			}
			
			//test start;
			if (current_piece == error_piece_num) {
				current_piece++;
				current_piece--;
			}
			//test end;
			
			if (current_piece == total_segments) {//如果接收到最后一个代码包，对crc进行判断后执行从烧写
				if (!send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_OK)) {
					seq++;
//					file_crc = crc_calculate(file_crc, CF_DATA_AREA_CODE_ADDR, PIECE_SIZE);
//					file_crc = crc_finish(file_crc);
					file_crc = crc_compute(CF_DATA_AREA_CODE_ADDR, PIECE_SIZE, file_crc, 1);

					if (file_crc != recv_file_crc) {
						send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_FILE_CRC_ERR);
						seq = 0;
						recv_updata_req = 0;
						return -1;
					} else {
						send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_OK);
						write_image_info_to_flash(total_segments, recv_file_crc);//从烧写时要根据image大小计算crc并与文件crc对比
						code_data_handle(CF_DATA_AREA_CODE_ADDR, &current_store_addr);
						reboot_flag = 1;
						boot_len = 96*total_segments;
						erase_flash(USER_BOOT_INFO_ADDR, 1, 0xff);
						write_to_flash_from_buff(USER_BOOT_INFO_ADDR,(uint8_t *)(&reboot_flag),2);
						write_to_flash_from_buff(USER_BOOT_INFO_ADDR+2,(uint8_t *)(&boot_len),4);
						SI32_RSTSRC_A_generate_software_reset(SI32_RSTSRC_0);
//						copy_boot_code_to_sram();
//						jump_to_boot();
					}
				}
			} else {//如果不是最后一个代码数据包，则进行crc累加，并将代码数据写入flash
 				if (write_flash_ok[current_piece] == 0 && write_flash_ok[current_piece-1] == 1) {
					if (send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_OK)) {
 						if (send_cf_ack(seq, UPDATE_DATA_PKT_REPLY_TYPE, UPDATE_DATA_OK))
							return -1;
					} else {
//						file_crc = crc_calculate(file_crc, CF_DATA_AREA_CODE_ADDR, PIECE_SIZE);
						file_crc = crc_compute(CF_DATA_AREA_CODE_ADDR, PIECE_SIZE, file_crc,0);
						code_data_handle(CF_DATA_AREA_CODE_ADDR, &current_store_addr);
	//					memset(rfRxInfo.pPayload, 0x00, CF_PKT_LEN+2);
						write_flash_ok[current_piece] = 1;
						current_piece++;
						seq++;
					}
 				} else {

				}
			}
			break;

		default:
			break;
	}
	return 0;
}

//每片代码的处理函数
//ptr：每个字段的代码在flash页中的偏移量，当该偏移量达到页最大容量时，对下一页进行擦写操作
//page_addr：存放代码的flash地址，以指针的方式带入函数
//返回ptr的值
uint32_t test_code_addr;
void code_data_handle(uint8_t *buffer, uint32_t * page_addr) {
	
	uint16_t len  = PIECE_SIZE;
	test_code_addr = *page_addr;
	
		write_data_to_flash(*page_addr,  buffer, len);
	
		*page_addr += len;

	
/*	write_flash_time_out = 0;
	for(i=0; i<=PIECE_SIZE-4; i+=4) {
		if(ptr == PAGE_SIZE) {
			ptr = 0;
			while(1) {
				if (write_flash_time_out >= ERASE_TIMEOUT_MS) {
					return -1;
				}
				if (FLASH_ErasePage(*page_addr) == FLASH_COMPLETE) {
					break;
				}
			}
		}
		write_flash_time_out = 0;
		while(1) {
			if (write_flash_time_out >= PROGRAM_WORD_TIMEOUT_MS) {
				return -1;
			}
			a = rfRxInfo.pPayload[14+3+i];
			b = rfRxInfo.pPayload[14+2+i];
			c = rfRxInfo.pPayload[14+1+i];
			d = rfRxInfo.pPayload[14+0+i];
			
			data32 = d | c<<8 | b<<16 | a<<24;
			
			if (FLASH_ProgramWord(*page_addr, data32) == FLASH_COMPLETE) {
				*page_addr += 4;
				ptr += 4;
				break;
			}
		}
	}

	FLASH_Lock();*/
	//return ptr;
}

// void flash_write_one_page(uint32_t page_addr) {
// 	uint32_t i;
// 	FLASH_Unlock();
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

// 	while(1) {
// 		if (write_flash_time_out >= ERASE_TIMEOUT_MS) {
// 			return ;
// 		}
// 		if (FLASH_ErasePage(page_addr) == FLASH_COMPLETE) {
// 			break;
// 		}
// 	}

// 	for(i=0; i<PAGE_SIZE; i+=4) {
// 		write_flash_time_out = 0;
// 		while(1) {
// 			if (write_flash_time_out >= PROGRAM_WORD_TIMEOUT_MS) {
// 				return ;
// 			}
// 			if (FLASH_ProgramWord(page_addr+i, (*(uint32_t*)(code_buf+i))) == FLASH_COMPLETE) {
// 				break;
// 			}
// 		}
// 	}
// 	FLASH_Lock();
// }

//将image的大小以及文件crc值写入flash的指定位置
void write_image_info_to_flash(uint16_t total_segments, uint16_t fille_crc) {
	uint16_t img_size = 0;
	uint16_t *buffer;
	img_size = total_segments*PIECE_SIZE; //bytes
	buffer = &img_size;
	write_data_to_flash(IMG_SIZE_ADDR,  (uint8_t *)buffer, 2);
	buffer = &fille_crc;
	write_data_to_flash(IMG_SIZE_ADDR+2,  (uint8_t *)buffer, 2);
	
/*	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

	write_flash_time_out = 0;
	while(1) {
		if (write_flash_time_out >= ERASE_TIMEOUT_MS) {
			return ;
		}
		if (FLASH_ErasePage(IMG_INFO_PAGE_ADDR) == FLASH_COMPLETE) {
			break;
		}
	}
	
	write_flash_time_out = 0;
	while(1) {
		if (write_flash_time_out >= PROGRAM_WORD_TIMEOUT_MS) {
			return ;
		}
		if (FLASH_ProgramWord(IMG_SIZE_ADDR, img_size) == FLASH_COMPLETE) {
			break;
		}
	}

	write_flash_time_out = 0;
	while(1) {
		if (write_flash_time_out >= PROGRAM_WORD_TIMEOUT_MS) {
			return ;
		}
		if (FLASH_ProgramHalfWord(IMG_CRC_ADDR, file_crc) == FLASH_COMPLETE) {
			break;
		}
	}

	FLASH_Lock();*/
}

//chai add code end

// #define SYS_INFO_SIZE  				((uint32_t)40)   // bytes
// #define SYS_INFO_START_ADDR			((uint32_t)0x0800f800)
// #define ERASE_TIMEOUT_MS			((uint32_t)5000) //ms
// #define PROGRAM_WORD_TIMEOUT_MS		((uint32_t)1000) //ms

// void write_sys_info_to_flash(uint8_t * sys_info_buf) {
// 	uint32_t i;
// 	uint32_t sys_info_size = SYS_INFO_SIZE;
// 	uint32_t sys_info_start_addr = SYS_INFO_START_ADDR;
// 	uint16_t sys_info_data_temp = 0;

// 	FLASH_Unlock();
// 	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

// 	write_flash_time_out = 0;
// 	while(1) {
// 		if (write_flash_time_out >= ERASE_TIMEOUT_MS) {
// 			return ;
// 		}
// 		if (FLASH_ErasePage(sys_info_start_addr) == FLASH_COMPLETE) {
// 			break;
// 		}
// 	}
// 	
// 	write_flash_time_out = 0;

// 	for(i=0; i<sys_info_size; i+=2) {
// 		sys_info_data_temp = *(uint16_t*)(sys_info_buf+i);
// 		while(1) {
// 			if (write_flash_time_out >= PROGRAM_WORD_TIMEOUT_MS) {
// 				return ;
// 			}
// 			if (FLASH_ProgramHalfWord(sys_info_start_addr+i, sys_info_data_temp) == FLASH_COMPLETE) {
// 				break;
// 			}
// 		}
// 	}

// 	FLASH_Lock();
// 	return ;
// }

bool send_start_config_flag_pkt(void) {
	bool ret = false;
	uint8_t start_config_flag_group[] = "******** Enter config mode, 5 Min Timeout! ********";
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		memcpy(frame, start_config_flag_group, sizeof(start_config_flag_group));
		rfTxInfo.destPanId = DEST_PANID;
		rfTxInfo.destAddr = DEST_ADDR;
		rfTxInfo.length = sizeof(start_config_flag_group);
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;

//		ret = basicRfSendPacket(&txInfo);
		ret = mac_send_packet_basic(&rfTxInfo,5000);
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
	return ret;
}

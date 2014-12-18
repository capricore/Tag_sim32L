#include "rf2xx_include.h"
#include "configuration.h"
#include "config.h"
#include "rf2xx_send.h"
#include "event_receive_pkt.h"
#include "flash_operation.h"

struct config_info pkt_config_struct;
uint16_t 	config_item[CONFIG_PARA_NUMBER];
uint8_t	 	config_item_flag[CONFIG_PARA_NUMBER];
uint16_t 	config_item_test[CONFIG_PARA_NUMBER];
uint16_t 	pkt_config_value;
uint8_t 	*data_pointer;
CFG_PARAS cfg_paras;

uint8_t compare_result;
uint8_t pkt_config_num;
uint8_t flash_write_return;
uint8_t *conf_cmd;



extern BASIC_RF_TX_INFO rfTxInfo;
extern BASIC_RF_RX_INFO rfRxInfo;
extern uint8_t fLength, frame[127];
extern uint16_t calcout_crc;
extern uint16_t battery;
extern const uint16_t current_software_version;

uint8_t ATC_CFG[] = "AT+CFG?\r";
uint8_t ATC_TGID[] = "AT+TGID=";
uint8_t ATC_CMSK[] = "AT+CMSK=";
uint8_t ATC_CHL[] = "AT+CHL=";
uint8_t ATC_EMSK[] = "AT+EMSK=";
uint8_t ATC_EDPT[] = "AT+EDPT=";
uint8_t ATC_HPT[] = "AT+HPT=";
uint8_t ATC_HPTO[] = "AT+HPTO=";
uint8_t ATC_EDPC[] = "AT+EDPC=";
uint8_t ATC_GMSK[] = "AT+GMSK=";
uint8_t ATC_WTIM[] = "AT+WTIM=";
uint8_t ATC_RSSI[] = "AT+RSSI=";
uint8_t ATC_SDPC[] = "AT+SDPC=";
uint8_t ATC_PTIM[] = "AT+PTIM=";
uint8_t ATC_LTIM[] = "AT+LTIM=";
uint8_t ATC_XCNT[] = "AT+XCNT=";
uint8_t ATC_ICNT[] = "AT+ICNT=";
uint8_t ATC_OCNT[] = "AT+OCNT=";
uint8_t ATC_TRSSI[] = "AT+TRSSI=";
uint8_t ATC_QUIT[] = "AT+QUIT";


void get_cfg_default(uint16_t* config_item)
{
	cfg_paras.cfg_tgid = config_item[0] = 0xee02;
	cfg_paras.cfg_cmsk = config_item[1] = 0x9013;
	cfg_paras.cfg_chl = config_item[2] = 22;
	cfg_paras.cfg_emsk = config_item[3] = 0x8013;
	cfg_paras.cfg_edpt = config_item[4] = 200;
	cfg_paras.cfg_hpt = config_item[5] = 5;
	cfg_paras.cfg_hpto = config_item[6] = 2;
	cfg_paras.cfg_edpc = config_item[7] = 10;
	cfg_paras.cfg_gmsk = config_item[8] = 0x4000;
	cfg_paras.cfg_wtim = config_item[9] = 50;
	cfg_paras.cfg_rssi = config_item[10] = 0;
	cfg_paras.cfg_sdpc = config_item[11] = 1;
	cfg_paras.cfg_ptim = config_item[12] = 0x7013;
	cfg_paras.cfg_ltim = config_item[13] = 50;
	cfg_paras.cfg_xcnt = config_item[14] = 3;
	cfg_paras.cfg_icnt = config_item[15] = 5;
	cfg_paras.cfg_ocnt = config_item[16] = 4;
	cfg_paras.cfg_trssi = config_item[17] = 0;
	config_item[CONFIG_PARA_NUMBER - 1] = SYS_PARA_MAGIC_NUM;
}
void get_cfg_paras(uint16_t* config_item)
{
	cfg_paras.cfg_tgid = config_item[0];
	cfg_paras.cfg_cmsk = config_item[1];
	cfg_paras.cfg_chl = config_item[2];
	cfg_paras.cfg_emsk = config_item[3];
	cfg_paras.cfg_edpt = config_item[4];
	cfg_paras.cfg_hpt = config_item[5];
	cfg_paras.cfg_hpto = config_item[6];
	cfg_paras.cfg_edpc = config_item[7];
	cfg_paras.cfg_gmsk = config_item[8];
	cfg_paras.cfg_wtim = config_item[9];
	cfg_paras.cfg_rssi = config_item[10];
	cfg_paras.cfg_sdpc = config_item[11];
	cfg_paras.cfg_ptim = config_item[12];
	cfg_paras.cfg_ltim = config_item[13];
	cfg_paras.cfg_xcnt = config_item[14];
	cfg_paras.cfg_icnt = config_item[15];
	cfg_paras.cfg_ocnt = config_item[16];
	cfg_paras.cfg_trssi = config_item[17];
}

uint8_t str_compare(uint8_t *s1, uint8_t *s2, uint8_t length)
{
	int i;
	for(i = 0; i < length; i++){
		if(*(s1+i) != *(s2+i))
			return 1;
	}
	return 0;
}

void send_config_cnt_ack(uint16_t seq, uint16_t dest_panID, uint16_t dest_addr)
{
	int i;
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = 0x74;
		frame[1] = 0x39;
		frame[3] = 0xff;
		frame[4] = 0xff;
		frame[5] = rf2xx_tat_get_short_address();
		frame[6] = rf2xx_tat_get_short_address() >> 8;
		frame[7] = seq;
		frame[8] = seq >> 8;		
		frame[9] = 0x80;
		frame[10] = CONFIG_PARA_NUMBER;
		read_parameter_from_flash(config_item);
		for(i = 0; i < CONFIG_PARA_NUMBER - 2; i++){
			frame[2*i+11] = config_item[i];
			frame[2*i+12] = config_item[i] >> 8;
		}
		i = 2 * i + 11;
		frame[i++] = battery;
		frame[i++] = battery >> 8;
		frame[i++] = current_software_version;
		frame[i++] = current_software_version >> 8;
		calcout_crc = crc_finish(crc_calculate(0xffff, &frame[3], i - 3));

		frame[i++] = calcout_crc;	//crc低位
		frame[i++] = calcout_crc >> 8;	//crc高位
		frame[i++] = 0x61;
		frame[2] = i - 3;
		rfTxInfo.destPanId = dest_panID;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.length = i;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
// 		basicRfSendPacket(&txInfo);
		mac_send_packet_basic(&rfTxInfo,5000);
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}
void send_config_cmd_ack(uint8_t seq, uint16_t dest_panID, uint16_t dest_addr, uint8_t length, uint8_t *frame)
{
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		rfTxInfo.destPanId = dest_panID;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.length = length;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}
}
void send_config_quit_ack(uint16_t seq, uint16_t dest_panID, uint16_t dest_addr)
{
	
	if(rf2xx_tat_set_trx_state(PLL_ON) == TAT_SUCCESS){
		frame[0] = 0x74;
		frame[1] = 0x39;
		frame[2] = 13;
		frame[3] = 0xff;
		frame[4] = 0xff;
		frame[5] = rf2xx_tat_get_short_address();
		frame[6] = rf2xx_tat_get_short_address() >> 8;
		frame[7] = seq;
		frame[8] = seq >> 8;		
		frame[9] = 0x80;
		frame[10] = 1;
		frame[11] = CMD_QUIT;
		frame[12] = 0;
		calcout_crc = crc_finish(crc_calculate(0xffff, &frame[3], 10));
		frame[13] = calcout_crc;
		frame[14] = calcout_crc >> 8;
		frame[15] = 0x61;
		rfTxInfo.destPanId = dest_panID;
		rfTxInfo.destAddr = 0xffff;
		rfTxInfo.length = 16;
		rfTxInfo.pPayload = frame;
		rfTxInfo.ackRequest = 0;
	//	while(1)
		mac_send_packet_basic(&rfTxInfo,5000);
		if(rf2xx_tat_set_trx_state(RX_ON) == TAT_SUCCESS){}
	}	
}

uint8_t parse_rx_config_pkt()
{
	uint8_t return_value = PKT_MATCH;
	int i, j, l;
	if(rfRxInfo.pPayload[rfRxInfo.length-1] == 0x61 && rfRxInfo.pPayload[0] == 0x74 && rfRxInfo.pPayload[1] == 0x39){	//config packet
		pkt_config_struct.pkt_len = rfRxInfo.pPayload[2];
		pkt_config_struct.destAddr = 0;
		pkt_config_struct.destAddr |= rfRxInfo.pPayload[4];
		pkt_config_struct.destAddr = pkt_config_struct.destAddr << 8;
		pkt_config_struct.destAddr |= rfRxInfo.pPayload[3];
		pkt_config_struct.srcAddr = 0;
		pkt_config_struct.srcAddr |= rfRxInfo.pPayload[6];
		pkt_config_struct.srcAddr = pkt_config_struct.srcAddr << 8;
		pkt_config_struct.srcAddr |= rfRxInfo.pPayload[5];
		pkt_config_struct.seq = 0;
		pkt_config_struct.seq |= rfRxInfo.pPayload[8];
		pkt_config_struct.seq = pkt_config_struct.seq << 8;
		pkt_config_struct.seq |= rfRxInfo.pPayload[7];
		data_pointer = rfRxInfo.pPayload + 9;
		switch(*data_pointer){
			case 0:
				if(data_pointer[1] == 1 && data_pointer[2] == CMD_CFG){
					if(str_compare(data_pointer+3, ATC_CFG, sizeof(ATC_CFG) - 1)==0){
				//test
//						for(i = 0; i < CONFIG_PARA_NUMBER; i++)
//							config_item[i] = i;
						send_config_cnt_ack(pkt_config_struct.seq, 0xffff, pkt_config_struct.exciterID);
					}
				}
				if(data_pointer[1] == 1 && data_pointer[2] == CMD_QUIT){
					if(str_compare(data_pointer+3, ATC_QUIT, sizeof(ATC_QUIT) - 1) == 0){
						send_config_quit_ack(pkt_config_struct.seq, 0xffff, pkt_config_struct.exciterID);
						return_value = QUIT_CONFIG_MODE;
						return return_value;
					}
				}
				
				if(data_pointer[2] != CMD_CFG){
					frame[0] = 0x74;
					frame[1] = 0x39;
//					frame[2] = 11 + CONFIG_PARA_NUMBER * 2;
					frame[3] = 0xff;
					frame[4] = 0xff;
					frame[5] = rf2xx_tat_get_short_address();
					frame[6] = rf2xx_tat_get_short_address() >> 8;
					frame[7] = pkt_config_struct.seq;
					frame[8] = pkt_config_struct.seq >> 8;						
					frame[9] = 0x80;
					pkt_config_num = data_pointer[1];
					frame[10] = data_pointer[1];
					data_pointer += 2;
					j = 11;
					for(i = 0; i < CONFIG_PARA_NUMBER; i++)
						config_item_flag[i] = 0;
					read_parameter_from_flash(config_item);
					for(i = 0; i < pkt_config_num; i++){
						conf_cmd = data_pointer;
						data_pointer++;
						l = 0;
						while(*data_pointer != '=' && l < 100){		//AT+ID=ee10
							data_pointer++;
							l++;
						}
						data_pointer++;
						pkt_config_value = data_pointer[1];
						pkt_config_value = pkt_config_value << 8;
						pkt_config_value |= data_pointer[0];
						switch(*conf_cmd){
// uint8_t ATC_CFG[] = "AT+CFG?\r";
// uint8_t ATC_TGID[] = "AT+ID=";
// uint8_t ATC_CMSK[] = "AT+CMSK=";
// uint8_t ATC_CHL[] = "AT+CHL=";
// uint8_t ATC_EMSK[] = "AT+EMSK=";
// uint8_t ATC_EDPT[] = "AT+EDPT=";
// uint8_t ATC_HPT[] = "AT+HPT=";
// uint8_t ATC_HPTO[] = "AT+HPTO=";
// uint8_t ATC_EDPC[] = "AT+EDPC=";
// uint8_t ATC_GMSK[] = "AT+GMSK=";
// uint8_t ATC_WTIM[] = "AT+WTIM=";
// uint8_t ATC_RSSI[] = "AT+RSSI=";
// uint8_t ATC_SDPC[] = "AT+SDPC=";
// uint8_t ATC_PTIM[] = "AT+PTIM=";
// uint8_t ATC_LTIM[] = "AT+LTIM=";
// uint8_t ATC_XCNT[] = "AT+XCNT=";
// uint8_t ATC_ID[] = "AT+ICNT=";
// uint8_t ATC_ID[] = "AT+OCNT=";
// uint8_t ATC_ID[] = "AT+TRSSI=";							
							case CMD_TGID:
								if(str_compare(conf_cmd + 1, ATC_TGID, sizeof(ATC_TGID) - 1)==0){
									config_item[0] = pkt_config_value;
									config_item_flag[0] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_CMSK:
								if(str_compare(conf_cmd + 1, ATC_CMSK, sizeof(ATC_CMSK) - 1)==0){
									config_item[1] = pkt_config_value;
									config_item_flag[1] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_CHL:
								if(str_compare(conf_cmd + 1, ATC_CHL, sizeof(ATC_CHL) - 1)==0){
									config_item[2] = pkt_config_value;
									config_item_flag[2] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_EMSK:
								if(str_compare(conf_cmd + 1, ATC_EMSK, sizeof(ATC_EMSK) - 1)==0){
									config_item[3] = pkt_config_value;
									config_item_flag[3] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_EDPT:
								if(str_compare(conf_cmd + 1, ATC_EDPT, sizeof(ATC_EDPT) - 1)==0){
									config_item[4] = pkt_config_value;
									config_item_flag[4] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_HPT:
								if(str_compare(conf_cmd + 1, ATC_HPT, sizeof(ATC_HPT) - 1)==0){
									config_item[5] = pkt_config_value;
									config_item_flag[5] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_HPTO:
								if(str_compare(conf_cmd + 1, ATC_HPTO, sizeof(ATC_HPTO) - 1)==0){
									config_item[6] = pkt_config_value;
									config_item_flag[6] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_EDPC:
								if(str_compare(conf_cmd + 1, ATC_EDPC, sizeof(ATC_EDPC) - 1)==0){
									config_item[7] = pkt_config_value;
									config_item_flag[7] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_GMSK:
								if(str_compare(conf_cmd + 1, ATC_GMSK, sizeof(ATC_GMSK) - 1)==0){
									config_item[8] = pkt_config_value;
									config_item_flag[8] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_WTIM:
								if(str_compare(conf_cmd + 1, ATC_WTIM, sizeof(ATC_WTIM) - 1)==0){
									config_item[9] = pkt_config_value;
									config_item_flag[9] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_RSSI:
								if(str_compare(conf_cmd + 1, ATC_RSSI, sizeof(ATC_RSSI) - 1)==0){
									config_item[10] = pkt_config_value;
									config_item_flag[10] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_SDPC:
								if(str_compare(conf_cmd + 1, ATC_SDPC, sizeof(ATC_SDPC) - 1)==0){
									config_item[11] = pkt_config_value;
									config_item_flag[11] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_PTIM:
								if(str_compare(conf_cmd + 1, ATC_PTIM, sizeof(ATC_PTIM) - 1)==0){
									config_item[12] = pkt_config_value;
									config_item_flag[12] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_LTIM:
								if(str_compare(conf_cmd + 1, ATC_LTIM, sizeof(ATC_LTIM) - 1)==0){
									config_item[13] = pkt_config_value;
									config_item_flag[13] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_XCNT:
								if(str_compare(conf_cmd + 1, ATC_XCNT, sizeof(ATC_XCNT) - 1)==0){
									config_item[14] = pkt_config_value;
									config_item_flag[14] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_ICNT:
								if(str_compare(conf_cmd + 1, ATC_ICNT, sizeof(ATC_ICNT) - 1)==0){
									config_item[15] = pkt_config_value;
									config_item_flag[15] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_OCNT:
								if(str_compare(conf_cmd + 1, ATC_OCNT, sizeof(ATC_OCNT) - 1)==0){
									config_item[16] = pkt_config_value;
									config_item_flag[16] = 1;
									data_pointer += 3;
								}
								break;
							case CMD_TRSSI:
								if(str_compare(conf_cmd + 1, ATC_TRSSI, sizeof(ATC_TRSSI) - 1)==0){
									config_item[17] = pkt_config_value;
									config_item_flag[17] = 1;
									data_pointer += 3;
								}
								break;
							default:
								break;
						}
					}
					write_sys_info_to_flash(config_item);
					read_parameter_from_flash(config_item_test);
					for(i = 0; i < CONFIG_PARA_NUMBER; i++){
						if(config_item_flag[i] == 1){
							frame[j++] = i + CMD_TGID;
							if(config_item[i] == config_item_test[i]){
								frame[j++] = 0;
							}
							else{
								frame[j++] = 1;
							}
						}
					}
					calcout_crc = crc_finish(crc_calculate(0xffff, &frame[3], j - 3));

					frame[j++] = calcout_crc;	//crc低位
					frame[j++] = calcout_crc >> 8;	//crc高位
					frame[j++] = 0x61;
					
					frame[2] = j - 3;
					send_config_cmd_ack(pkt_config_struct.seq, 0xffff, pkt_config_struct.exciterID, j, frame);
				}
				break;
			default:
				break;
		}
		pkt_config_struct.crc = 0;
		pkt_config_struct.crc |= rfRxInfo.pPayload[rfRxInfo.length - 2];
		pkt_config_struct.crc = pkt_config_struct.crc << 8;
		pkt_config_struct.crc |= rfRxInfo.pPayload[rfRxInfo.length - 3];

	}
	return return_value;
}

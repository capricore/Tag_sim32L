#ifndef __CONFIGURATION_H
#define __CONFIGURATION_H

#ifdef  __cplusplus
	extern "C" {
#endif

#include <stdint.h>
		
#define CONFIG_PARA_NUMBER 20
#define MAX_DATA_LEN 127

#define CMD_CFG 50
#define CMD_TGID	54
#define CMD_CMSK 55
#define CMD_CHL	56
#define CMD_EMSK	57
#define CMD_EDPT	58
#define CMD_HPT	59
#define CMD_HPTO	60
#define	CMD_EDPC	61
#define CMD_GMSK	62
#define CMD_WTIM	63
#define	CMD_RSSI	64
#define	CMD_SDPC	65
#define CMD_PTIM	66
#define CMD_LTIM	67
#define CMD_XCNT	68
#define CMD_ICNT	69
#define CMD_OCNT	70
#define CMD_TRSSI	71
#define CMD_QUIT	72

struct config_info
{
	uint16_t exciterID;
	uint8_t pkt_len;
	uint16_t destAddr;
	uint16_t srcAddr;
	uint16_t seq;
	uint16_t crc;
};

typedef struct {
	uint16_t cfg_tgid;		//标签卡ID
	uint16_t cfg_cmsk;		//配置激励ID掩码
	uint16_t cfg_chl;			//采用的通信信道
	uint16_t cfg_emsk;		//进出口激励ID掩码
	uint16_t cfg_edpt;		//发送进场包和出场包的时间间隔
	uint16_t cfg_hpt;			//发送心跳包的时间间隔
	uint16_t cfg_hpto;		//心跳包超时时间
	uint16_t cfg_edpc;		//发送进场包和离场包的数量
	uint16_t cfg_gmsk;		//普通激励器ID掩码
	uint16_t cfg_wtim;		//等待同步时的唤醒时间长度
	uint16_t cfg_rssi;		//激励器信号强度阀值（RSSI）		
	uint16_t cfg_sdpc;		//确定同步时需要的同步包个数
	uint16_t cfg_ptim;		//测试激励器ID掩码
	uint16_t cfg_ltim;		//每个周期唤醒时的监听时间长度
	uint16_t cfg_xcnt;		//判断尚未进洞需要的周期数
	uint16_t cfg_icnt;		//判断已进洞需要的周期数
	uint16_t cfg_ocnt;		//判断已离洞需要的周期数
	uint16_t cfg_trssi;		//判断进洞需要的RSSI阀值
	uint32_t magic_num;		//判断是否已配置过的标志位，数值应为SYS_PARA_MAGIC_NUM
}CFG_PARAS;

void get_cfg_default(uint16_t* config_item);
void get_cfg_paras(uint16_t* config_item);
uint8_t str_compare(uint8_t *s1, uint8_t *s2, uint8_t length);
void send_config_cnt_ack(uint16_t seq, uint16_t dest_panID, uint16_t dest_addr);
void send_config_cmd_ack(uint8_t seq, uint16_t dest_panID, uint16_t dest_addr, uint8_t length, uint8_t *frame);
void send_config_quit_ack(uint16_t seq, uint16_t dest_panID, uint16_t dest_addr);
uint8_t parse_rx_config_pkt(void);

#ifdef  __cplusplus
	extern }
#endif

#endif //__CONFIGURATION_H

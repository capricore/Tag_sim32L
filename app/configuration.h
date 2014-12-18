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
	uint16_t cfg_tgid;		//��ǩ��ID
	uint16_t cfg_cmsk;		//���ü���ID����
	uint16_t cfg_chl;			//���õ�ͨ���ŵ�
	uint16_t cfg_emsk;		//�����ڼ���ID����
	uint16_t cfg_edpt;		//���ͽ������ͳ�������ʱ����
	uint16_t cfg_hpt;			//������������ʱ����
	uint16_t cfg_hpto;		//��������ʱʱ��
	uint16_t cfg_edpc;		//���ͽ��������볡��������
	uint16_t cfg_gmsk;		//��ͨ������ID����
	uint16_t cfg_wtim;		//�ȴ�ͬ��ʱ�Ļ���ʱ�䳤��
	uint16_t cfg_rssi;		//�������ź�ǿ�ȷ�ֵ��RSSI��		
	uint16_t cfg_sdpc;		//ȷ��ͬ��ʱ��Ҫ��ͬ��������
	uint16_t cfg_ptim;		//���Լ�����ID����
	uint16_t cfg_ltim;		//ÿ�����ڻ���ʱ�ļ���ʱ�䳤��
	uint16_t cfg_xcnt;		//�ж���δ������Ҫ��������
	uint16_t cfg_icnt;		//�ж��ѽ�����Ҫ��������
	uint16_t cfg_ocnt;		//�ж����붴��Ҫ��������
	uint16_t cfg_trssi;		//�жϽ�����Ҫ��RSSI��ֵ
	uint32_t magic_num;		//�ж��Ƿ������ù��ı�־λ����ֵӦΪSYS_PARA_MAGIC_NUM
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

#ifndef __EVENT_RECEIVE_PKT_H
#define __EVENT_RECEIVE_PKT_H

#include <stdint.h>
#include "rf2xx_protoc.h"
#define PKT_MATCH 0
#define TYPE_ERROR 1
#define QUIT_CONFIG_MODE 2

#define DEST_PANID		0xffff //��!
#define DEST_ADDR		0xffff //��!
#define ACK_LENGTH		(0x0B)	//ack������ 11�ֽڣ�����ǰ�����ͳ��ȵ������ֽڣ�
extern BASIC_RF_RX_INFO rfRxInfo;
#define PROTOCOL_NUM	(0x61)
//ǰ����	2B
#define CF_PKT_PRE					((uint16_t)(((uint16_t)rfRxInfo.pPayload[0]) + (((uint16_t)rfRxInfo.pPayload[1])<<8)))
//����		1B��������ǰ�����ͳ��ȱ��� �����ֽ�
#define CF_PKT_LEN					(rfRxInfo.pPayload[2])
//��CRC		2B
#define CF_PKT_CRC					(((uint16_t)rfRxInfo.pPayload[CF_PKT_LEN]) + (((uint16_t)rfRxInfo.pPayload[CF_PKT_LEN+1])<<8))
//Э���
#define CF_PKT_PROTOCOL_NUM			(rfRxInfo.pPayload[CF_PKT_LEN+2])

//������ ������������ 1B
#define CF_DATA_AREA_TYPE			(rfRxInfo.pPayload[9])
//��Ƭ��  2B
#define CF_DATA_AREA_TOTAL_SEQ		(((uint16_t)rfRxInfo.pPayload[12]) + (((uint16_t)rfRxInfo.pPayload[13])<<8))

//�����
//����汾��  2B
#define CF_DATA_AREA_VERSION		(((uint16_t)rfRxInfo.pPayload[10]) + (((uint16_t)rfRxInfo.pPayload[11])<<8))
//�ļ�CRC  2B
#define CF_DATA_AREA_FILE_CRC		(((uint16_t)rfRxInfo.pPayload[14]) + (((uint16_t)rfRxInfo.pPayload[15])<<8))

//�����
//��ǰƬ�� 2B
#define CF_DATA_AREA_CURRENT_PIECE	(((uint16_t)rfRxInfo.pPayload[10]) + (((uint16_t)rfRxInfo.pPayload[11])<<8))
//���������׵�ַ  uint8_t*	
#define CF_DATA_AREA_CODE_ADDR		((uint8_t*)&(rfRxInfo.pPayload[14]))

#define PIECE_SIZE					(96)			// ÿƬ�����С
#define PAGE_SIZE					((uint32_t)0x400)
#define FLASH_STORE_CODE_SIZE		((uint32_t)(30*PAGE_SIZE))	// ��������ſռ�
#define FLASH_STORE_CODE_START_ADDR	(0x00007800)

#define PKT_CRC_START_ADDR			(&rfRxInfo.pPayload[3])

#define IMG_INFO_PAGE_ADDR			(0x0000FC00)
#define IMG_SIZE_ADDR				(IMG_INFO_PAGE_ADDR + 0)
#define IMG_CRC_ADDR				(IMG_INFO_PAGE_ADDR + 4)

#define SOFTWARE_VERSION			((uint16_t)0x1105)

typedef enum {
	UPDATE_ASK_OK = 1,
	UPDATE_ASK_SOFTWARE_VERSION_ERR,
	UPDATE_ASK_TOTAL_SEGMENTS_ERR,

	UPDATE_ASK_PKT_REPLY_TYPE = 0x81,
} CODE_UPDATE_ASK_REPORT;
#include "rf2xx_include.h"

typedef enum {
	UPDATE_DATA_OK = 1,
	UPDATE_DATA_CURRENT_SEQMENT_ERR,
	UPDATE_DATA_TOTAL_SEGMENTS_ERR,
	UPDATE_DATA_FILE_CRC_ERR,

	UPDATE_DATA_PKT_REPLY_TYPE = 0x82
} CODE_UPDATE_DATA_REPORT;

typedef enum {
	IS_LAST_PIECE = 1,
	NOT_LAST_PIECE,
} IS_LAST_PIECE_;

struct time_info
{
	uint16_t exciterID;
	uint16_t receiverID;
	uint16_t panID;
	uint32_t time;
	uint8_t subtime;
	uint32_t nextWTime;
	uint16_t period;
	uint8_t seq;
	uint64_t recv_time;
};

void write_image_info_to_flash(uint16_t total_segments, uint16_t fille_crc);
uint8_t parse_rx_syn_pkt(void);
void send_syn_ack(uint8_t seq, uint16_t dest_panID, uint16_t dest_addr);
uint8_t parse_rx_cave_pkt(void);
int send_cf_ack(uint16_t seq, uint8_t pkt_type, uint8_t report);
bool send_test_pkt(uint16_t exciterID);
int parse_rx_update_code_pkt(void);
void code_data_handle(uint8_t *buffer, uint32_t * page_addr);
void write_image_info_to_flash(uint16_t total_segments, uint16_t fille_crc) ;
bool send_start_config_flag_pkt(void);

#endif //__EVENT_RECEIVE_PKT_H

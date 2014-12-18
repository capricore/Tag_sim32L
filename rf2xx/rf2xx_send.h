#ifndef RF2XX_SEND_H_
#define RF2XX_SEND_H_

#include <stdint.h>
extern uint8_t tx_frame[ 127 ];
typedef uint8_t BOOL;
typedef uint8_t BYTE;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
//typedef bool BOOL;
typedef UINT16 WORD;
typedef struct {
	WORD destPanId;		//目标PanId
	WORD destAddr;		//目标地址
	WORD srcAddr;
	uint8_t length;		//包长度
	uint8_t *pPayload;	//指向有效载荷的指针????
	BOOL ackRequest;	//是否需要应答
	//UINT8 lqi;
} BASIC_RF_TX_INFO;


extern BASIC_RF_TX_INFO rfTxInfo;

int mac_assemble_packet(BASIC_RF_TX_INFO *pRTI);

int RfSend_Test_Mode(void);

int mac_send_packet_basic(BASIC_RF_TX_INFO *pRTI,  uint16_t timeout);

int mac_send_packet_extend(BASIC_RF_TX_INFO *pRTI, uint16_t timeout );

int mac_csma_send_packet(BASIC_RF_TX_INFO *pRTI, uint16_t timeout);

#endif


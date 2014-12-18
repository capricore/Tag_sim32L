#ifndef RF2XX_PROTOC_H
#define RF2XX_PROTOC_H

#define BASIC_RF_PACKET_FOOTER_SIZE 2
#define HAL_MIN_FRAME_LENGTH   ( 0x03 ) //!< A frame should be at least 3 bytes.
#define HAL_MAX_FRAME_LENGTH   ( 0x7F ) //!< A frame should no more than 127 bytes.
#define BASIC_RF_PACKET_FOOTER_SIZE 2
#define BASIC_RF_ACK_PACKET_SIZE		5


#define BASIC_RF_FCF_NOACK			0x8841
#define BASIC_RF_ACK_FCF				0x0002		//��־ACK��
#define BASIC_RF_FCF_ACK_BM             	0x0020
#define BASIC_RF_FCF_BM				(~BASIC_RF_FCF_ACK_BM)

// Footer
#define BASIC_RF_CRC_OK_BM              0x80
#define LQI_MASK                        0x7F

#define BASIC_RF_MAX_PAYLOAD_SIZE		(127 - BASIC_RF_PACKET_OVERHEAD_SIZE)
#include <stdint.h>
typedef uint8_t BOOL;
typedef uint8_t BYTE;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
//typedef bool BOOL;
typedef UINT16 WORD;

typedef struct {
	uint8_t seqNumber;	//���к�
	WORD srcAddr;		//Դ��ַ
	WORD srcPanId;		//ԴPanID
	uint8_t length;		//������
	uint8_t *pPayload;	//��Ч�غ�ָ��????������Ϣ�ṹ������ݻ�����ָ��????
//Added by Cong
	uint8_t payloadLength;	//�յ������ݲ��ֵĳ���

	BOOL ackRequest;	//�Ƿ�Ҫ��Ӧ��
	uint64_t recv_time;
	UINT8 ed;		//monkey change it 2010,10,9
} BASIC_RF_RX_INFO;


typedef struct BASIC_RF_SETTINGS_t{
    BASIC_RF_RX_INFO *pRxInfo;		//ָ�������Ϣ�ṹ���ָ��
    uint8_t txSeqNumber;				//�������к�
    volatile uint8_t ackReceived;			//Ӧ���Ƿ���յ�
    uint16_t panId;					//������ID
    uint16_t myAddr;					//��ַ
    uint8_t receiveOn;					//�ڽ��յ�ACK�������������????
    uint8_t rfChannel;					//��Ƶchannel
} BASIC_RF_SETTINGS;

void rf2xx_protoc_init(void);

#endif


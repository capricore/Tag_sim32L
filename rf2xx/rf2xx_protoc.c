#include "rf2xx_include.h"

volatile BASIC_RF_SETTINGS rfSettings = {};
uint8_t pTxBuffer[BASIC_RF_MAX_PAYLOAD_SIZE];
uint8_t pRxBuffer[BASIC_RF_MAX_PAYLOAD_SIZE];
extern BASIC_RF_RX_INFO rfRx;
	

void rf2xx_protoc_init(void)
{

	rfSettings.pRxInfo = &rfRx; 		
	rfSettings.panId = panid;
	rfSettings.myAddr = id;
	rfSettings.txSeqNumber = 0;
	rfSettings.receiveOn = false;

	rfRxInfo.pPayload = pRxBuffer;
	rfTxInfo.pPayload = pTxBuffer;
	packet_received = 0;
	received_packet_pool.header = MAX_PACKET_BUFFER_SIZE - 1;
	received_packet_pool.tail = 0;
	received_packet_pool.size = MAX_PACKET_BUFFER_SIZE; 

}

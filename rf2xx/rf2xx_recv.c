#include "rf2xx_include.h"
#include "RTC0.h"


#define APP_PROTO_ID 0X77

uint8_t recv_packet_lock = 1;
BASIC_RF_RX_INFO rfRxInfo;
BASIC_RF_RX_INFO rfRx;

hal_rx_frame_t  rx_frame;
uint8_t pReceivedBuffer[BASIC_RF_MAX_PAYLOAD_SIZE];
BASIC_RF_RX_INFO   received_pkt =
{
	0,0,0,0,pReceivedBuffer,0,0
};  

uint64_t received_time;
packet_pool received_packet_pool;
uint32_t packet_received;
extern volatile BASIC_RF_SETTINGS rfSettings;
extern uint8_t recv_cfg_event;
extern uint8_t recv_gw_event;
int rf2xx_recv_on(void) 
{
	int ret = -1;
	uint8_t trx_state;
	uint8_t retry = 3;
	while(retry--)
	{
		rf2xx_tat_set_trx_state( RX_AACK_ON ) ;
		trx_state = rf2xx_tat_get_trx_state();
		if(trx_state == RX_AACK_ON)
		{
			ret = 0;
			break;
		}
	}
	return ret;
}

void mac_parze_packet(BASIC_RF_RX_INFO *pRRI)
{
//	uint64_t time;
//	time = get_current_time();
//	p_recv_packet_lock();
	uint32_t cpu_sr = cpu_sr;

	ENTER_CRITICAL_REGION();

	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->recv_time), 8);
	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->seqNumber), 1);
	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->srcAddr), 2);
	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->srcPanId), 2);
	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->length), 1);
	READ_PACKET_POOL(received_packet_pool, (pRRI->pPayload), (pRRI->length));
	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->ackRequest), 1);
	READ_PACKET_POOL(received_packet_pool, (uint8_t*) &(pRRI->ed), 1);
	packet_received--;
	LEAVE_CRITICAL_REGION();
}
uint64_t r_time;
void rf2xx_trx_recv_packet(void)
{
    if (rf_if_rx()) 
	{
		uint16_t frameControlField;
		uint8_t pFooter[2];
		uint8_t length;
		uint16_t capacity;
    	uint8_t ed_value_test;
		received_time = get_current_time();
		r_time = get_current_time();
		ed_value_test = rf2xx_tat_get_ed_value();

	    rf2xx_hal_frame_read( &rx_frame );
		length = rx_frame.length;
		length &= HAL_MAX_FRAME_LENGTH; // Ignore MSB
		rfSettings.pRxInfo->recv_time = received_time;
		frameControlField = *(uint16_t *)&(rx_frame.data[0]);	
		(rfSettings.pRxInfo)->seqNumber= rx_frame.data[2];
		rfSettings.pRxInfo->ed = ed_value_test; 
		if (rx_frame.crc != true) 
		{
		} 
		else 
		{
			if ( length < BASIC_RF_ACK_PACKET_SIZE) 
			{
			} 
			else 
			{
				rfSettings.pRxInfo->length = \
					length - BASIC_RF_PACKET_OVERHEAD_SIZE - BASIC_RF_PACKET_FOOTER_SIZE;

				if ((length == BASIC_RF_ACK_PACKET_SIZE)&&(frameControlField == BASIC_RF_ACK_FCF ) && \
						(rfSettings.pRxInfo->seqNumber == rfSettings.txSeqNumber) )
				{
					pFooter[0] = rx_frame.data[3];
					pFooter[1] = rx_frame.data[4];
				
					if (pFooter[1] & BASIC_RF_CRC_OK_BM)	
						rfSettings.ackReceived = true;
				} 
				else if (length < BASIC_RF_PACKET_OVERHEAD_SIZE) 
				{				
				} 
				else 
				{
					rfSettings.pRxInfo->srcAddr = *(uint16_t *)&(rx_frame.data[7]);

					rfSettings.pRxInfo->pPayload = &(rx_frame.data[9]);

					pFooter[ 0 ] = rx_frame.data[ length - 2 ];
					pFooter[ 1 ] = rx_frame.data[ length - 1 ];
					if (((frameControlField & (BASIC_RF_FCF_BM)) == BASIC_RF_FCF_NOACK)) 
					{
						CHECK_POOL(received_packet_pool,capacity);
						if (rfSettings.pRxInfo->length + 16 < capacity)								
						{
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->recv_time, 8);
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->seqNumber, 1);
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->srcAddr, 2);
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->srcPanId, 2);
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->length, 1);
							WRITE_PACKET_POOL(received_packet_pool, rfSettings.pRxInfo->pPayload, rfSettings.pRxInfo->length);
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->ackRequest, 1);
							WRITE_PACKET_POOL(received_packet_pool, (uint8_t*) &rfSettings.pRxInfo->ed, 1);
							packet_received++;
						}
					}
				}
			}
		}
    } 
}

inline void p_recv_packet_lock(void)
{
	recv_packet_lock = 0;
}

inline void v_recv_packet_lock(void)
{
	recv_packet_lock = 1;
}

inline uint8_t get_recv_packet_lock(void)
{
	return recv_packet_lock;
}


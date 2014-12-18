#ifndef RF2XX_RECV_H
#define RF2XX_RECV_H

#define MAX_PACKET_BUFFER_SIZE 512
#include "rf2xx_protoc.h"
typedef struct packet_pool_t
{
	uint8_t packet_buffer[MAX_PACKET_BUFFER_SIZE];
	uint16_t tail; // the next byte to write
	uint16_t header; // the previous byte read
	uint16_t size;
}packet_pool;


#define WRITE_PACKET_POOL(b, p, c) \
	do { \
		for (uint8_t spiCnt = 0; spiCnt < (c); spiCnt++) { \
            		b.packet_buffer[b.tail] = (((uint8_t*)(p))[spiCnt]); \
            		b.tail++; \
            		if (b.tail >= ((packet_pool)b).size) \
			{ \
				b.tail=0; \
			} \
		} \
	} while(0)

#define READ_PACKET_POOL(b, p, c) \
	do { \
		for (uint8_t spiCnt = 0; spiCnt < (c); spiCnt++) { \
			b.header++; \
			if (b.header >= b.size) \
			{ \
				b.header = 0; \
			} \
           	((uint8_t*)(p))[spiCnt] = b.packet_buffer[b.header]; \
		} \
	} while(0)
	
#define CHECK_POOL(b,s) \
			do { \
				if( b.tail <= b.header ) \
				{ \
					s = b.header - b.tail; \
				} \
				else \
				{ \
					s = b.header + b.size - b.tail; \
				} \
			} while(0)


extern BASIC_RF_RX_INFO rfRxInfo;
extern uint32_t packet_received;
extern packet_pool received_packet_pool;
void rf2xx_trx_recv_packet(void);

int rf2xx_recv_on(void) ;

void mac_parze_packet(BASIC_RF_RX_INFO *pRRI);

inline void p_recv_packet_lock(void);

inline void v_recv_packet_lock(void);

inline uint8_t get_recv_packet_lock(void);

#endif

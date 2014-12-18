#include "rf2xx_include.h"

BASIC_RF_TX_INFO rfTxInfo;


//BASIC_RF_RX_INFO rfRxInfo;

uint8_t tx_frame[127];//< Buffer used to build TX frames. (Size must be max PSDU length.)
uint8_t txSeqNumber;
uint8_t tx_frame_current_position;

int mac_assemble_packet(BASIC_RF_TX_INFO *pRTI)
{
	uint8_t length_of_tx_data_without_head;
	int success = 0;
	uint8_t *pData;
	tx_frame_current_position = BASIC_RF_PACKET_OVERHEAD_SIZE;

	if( pRTI->ackRequest == 0 ) 
	{
		tx_frame[ 0 ] = 0x41; 
	} 
	else 
	{
		tx_frame[ 0 ] = 0x61; 
	}
	tx_frame[ 1 ] = 0x88;               
	tx_frame[ 2 ] = txSeqNumber; 
	tx_frame[ 3 ] = pRTI->destPanId & 0xFF; 
	tx_frame[ 4 ] = ( pRTI->destPanId >> 8 ) & 0xFF; 
	tx_frame[ 5 ] = pRTI->destAddr & 0xFF; 
	tx_frame[ 6 ] = ( pRTI->destAddr >> 8 ) & 0xFF; 
	tx_frame[ 7 ] = pRTI->srcAddr & 0xFF; 
	tx_frame[ 8 ] = (pRTI->srcAddr >> 8 ) & 0xFF; 

	pData = pRTI->pPayload; 
	length_of_tx_data_without_head = pRTI->length + BASIC_RF_PACKET_FOOTER_SIZE;


	do {
	tx_frame[ tx_frame_current_position++ ] = *pData++;
	} while (--length_of_tx_data_without_head > 0);
	success = 0;
	txSeqNumber++;
	return success;

}

int mac_send_packet_extend(BASIC_RF_TX_INFO *pRTI, uint16_t timeout )
{
	int ret = 0;
	tat_status_t tat_status;
	uint8_t trx_state;
	rf2xx_tat_set_trx_state(TX_ARET_ON);
	trx_state = rf2xx_tat_get_trx_state();
	if(trx_state == TX_ARET_ON)
	{
		mac_assemble_packet(pRTI);
		rf_tx_on();
		tat_status = rf2xx_tat_send_packet(tx_frame_current_position, tx_frame, timeout);
		if(tat_status == TAT_SUCCESS)
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		rf_rx_on();
		if(rf2xx_recv_on())
		{
			ret = -2;
		}
	}
	else
	{
		if(rf2xx_recv_on())
		{
			ret	= -3;
		}
		else
		{
			ret = -4;
		}
	}
	return ret;

}

int mac_send_packet_basic(BASIC_RF_TX_INFO *pRTI,  uint16_t timeout)
{
	int ret = 0;
	tat_status_t tat_status;
	uint8_t trx_state;
	rf2xx_tat_set_trx_state(PLL_ON);
	trx_state = rf2xx_tat_get_trx_state();
	if(trx_state == PLL_ON)
	{
		mac_assemble_packet(pRTI);
		rf_tx_on();
		tat_status = rf2xx_tat_send_packet(tx_frame_current_position, tx_frame, timeout);
		if(tat_status == TAT_SUCCESS)
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		rf_rx_on();
		if(rf2xx_recv_on())
		{
			ret = -2;
		}
	}
	else
	{
		if(rf2xx_recv_on())
		{
			ret	= -3;
		}
		else
		{
			ret = -4;
		}
	}
	return ret;
}

// int mac_csma_send_packet(BASIC_RF_TX_INFO *pRTI,  uint16_t timeout)
// {
// 	int ret = -1;
// 	tat_status_t cur_status;
// 	tat_status_t trx_status;
// 	rf2xx_tat_set_trx_state(RX_ON);
// 	cur_status = rf2xx_tat_do_cca();
// 	do{
// 		if( cur_status == TAT_CCA_IDLE )
// 		{
// 			trx_status = rf2xx_tat_set_trx_state(PLL_ON);
// 			if(trx_status == TAT_SUCCESS)
// 			{
// 				ret = mac_send_packet_basic(pRTI, timeout);
// 				break;
// 			}
// 			else if(trx_status == TAT_BUSY_STATE)
// 			{

// 			}
// 			else if(trx_status == TAT_TIMED_OUT)
// 			{

// 			}

// 		}
// 		else if((cur_status == TAT_CCA_BUSY))
// 		{

// 		}
// 		else
// 		{
// 			
// 		}
// 	}while(!manual_csma());

// 	return ret;
// }


int RfSend_Test_Mode(void)
{	
	uint8_t tx_frame[ 127 ];
	uint8_t i = 0;
	uint8_t tat_state ;
	rf2xx_tat_set_trx_state(PLL_ON);
	tat_state = rf2xx_tat_get_trx_state();
	if(tat_state == PLL_ON)
	{
		rf2xx_hal_subregister_write(RF231_RF212_SR_TX_AUTO_CRC_ON, 0);		
		for(i = 0; i<127; i++)
				tx_frame[i] = 0xff;
		rf2xx_hal_frame_write(tx_frame, i); 
		rf2xx_hal_register_write(0x36,0x0f);
		rf2xx_hal_register_write(0x1c, 84);
		rf2xx_hal_register_write(0x1c, 70);
		rf2xx_hal_register_write(0x02, 0x02);
	}
   return 0;
}



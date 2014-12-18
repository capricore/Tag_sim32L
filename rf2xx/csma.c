#include "rf2xx_include.h"
#include "arch_include.h"

uint8_t mac_csma_timeout_flag;
uint8_t csma_bexp ;
uint8_t csma_num;

void csma_init(void)
{
	csma_bexp = CSMA_BE;
	csma_num = 1;
}


int manual_csma(void)
{
	uint16_t base_value = BM(csma_bexp)-1;
	uint16_t slot_length = 400;
	uint16_t rand_wait_time = (rand() & base_value);
	if(csma_num > CSMA_NB)
	{
		return -1;
	}
	
	rand_wait_time *= slot_length;
	rand_wait_time += slot_length;
	csma_num++;
//	timeout_add_item(rand_wait_time, &mac_csma_timeout_flag, TIMEOUT_US);
//	timeout_add_item(8000, &mac_csma_event, TIMEOUT_US);
	return 0;
}


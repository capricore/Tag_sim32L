#include "exciter_include.h"

uint8_t exciter_state = EXCITER_IDLE;

uint8_t exciter_initial_event;
uint8_t exciter_over_event;
uint8_t exciter_data_buff[EXCITER_DATA_BUFF_LEN];
uint8_t exciter_data_crc_buff[EXCITER_DATA_CRC_BUFF_LEN];
uint8_t exciter_man_data_buff[EXCITER_MAN_DATA_BUFF_LEN];

exciter_data_t exciter_data;
exciter_man_data_t exciter_man_data;

uint8_t preamble_cnt = 0;

uint16_t pattern_data = 0xcdab;
uint16_t pattern_temp;
uint8_t burst_cnt = 0;
uint8_t pattern_cnt = 0;
uint8_t exciter_data_cnt = 0;
uint8_t exciter_data_len;
uint8_t exciter_send_temp;
uint8_t exciter_send_index;
uint8_t *p_exciter_send_buff;

void exciter_test(void)
{
	uint8_t buff[2] = {0x44,0xff};
	exciter_initial_event = 1;
	exciter_state = EXCITER_IDLE;
	set_exciter_data(buff,2);
	set_exciter_man_data(1);
}

void exciter_init(void)
{
	wave_125k_init();
	wave_4k_init();
	wave_4k_stop();
	wave_125k_stop();
	exciter_data_init();
}


void exciter_data_init(void)
{
	exciter_data.data_len = 0;
	exciter_data.crc_len = 0;
	exciter_data.data = exciter_data_buff;
	exciter_data.crc = exciter_data_crc_buff;

	exciter_man_data.data_len = 0;
	exciter_man_data.data = exciter_man_data_buff;
}

void set_exciter_data(uint8_t *data, uint8_t data_len)
{
	uint8_t *p_temp;
	uint16_t crc;
	uint8_t *p_crc = (uint8_t *)(&crc);
	uint8_t crc_len = EXCITER_DATA_CRC_BUFF_LEN;

	exciter_data.data_len = data_len;
	exciter_data.crc_len = crc_len;
	p_temp = exciter_data.data;
	while(data_len--)
		*p_temp++ = *data++;
	crc = crc_compute(data, data_len, INITIAL_VALUE, 1);
	p_temp = exciter_data.crc;
	while(crc_len--)
		*p_temp++ = *p_crc++;
}

void set_exciter_man_data(uint8_t if_crc)
{
	uint8_t *p_man = exciter_man_data.data;
	uint8_t *p_data = exciter_data.data;
	uint8_t *p_crc = exciter_data.crc;
	uint8_t crc_len = exciter_data.crc_len;
	uint8_t data_len = exciter_data.data_len;
	uint8_t temp;
	uint8_t i = 0;
	exciter_man_data.data_len = (exciter_data.data_len)<<1;
	while(data_len--)
	{
		temp = *p_data++;
		for(i=0; i<8; i++)
		{
			*p_man <<= 2;
			if(temp&EXCITER_MASK)
			{
				*p_man += 2;
			}
			else
			{
				*p_man += 1;
			}
			if(i == 3)
			{
				p_man++;
			}
			temp <<= 1;
		}
		p_man++;
	}

	if(if_crc)
	{
		exciter_man_data.data_len += (exciter_data.crc_len)<<1;
		while(crc_len--)
		{
			temp = *p_crc++;
			for(i=0; i<8; i++)
			{
				*p_man <<= 2;
				if(temp&EXCITER_MASK)
				{
					*p_man += 2;
				}
				else
				{
					*p_man += 1;
				}
				if(i == 3)
				{
					p_man++;
				}
				temp <<= 1;
			}
			p_man++;
		}
	}
	
}

int exciter_initial_handler(void)
{
	int ret = 0;
	switch(exciter_state)
	{
		case EXCITER_IDLE:
			exciter_state = EXCITER_BURST;
			wave_125k_start();
			wave_4k_start();
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}

int exciter_over_handler(void)
{
	int ret = 0;
	switch(exciter_state)
	{
		case EXCITER_IDLE:
			delay_us(40000);
			exciter_initial_event = 1;
			break;
		default:
			ret = -1;
			break;
	}
	return ret;
}


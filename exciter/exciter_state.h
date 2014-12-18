#ifndef EXCITER_STATE_H
#define EXCITER_STATE_H

#define EXCITER_IDLE		1
#define EXCITER_PREAMBLE	2
#define EXCITER_PATTERN		3
#define EXCITER_DATA		4
#define EXCITER_BURST		5

#define EXCITER_DATA_BUFF_LEN 4
#define EXCITER_DATA_CRC_BUFF_LEN 2
#define EXCITER_MAN_DATA_BUFF_LEN 20

#define CARRIER_BURST_LEN	45
#define PREAMBLE_LEN 6
#define PATTERN_LEN 16

#define EXCITER_MASK 0X80

typedef struct exciter_data_tt
{
	uint8_t data_len;
	uint8_t crc_len;
	uint8_t *data;
	uint8_t *crc;
	
}exciter_data_t;

typedef struct exciter_man_data_tt
{
	uint8_t data_len;
	uint8_t *data;
}exciter_man_data_t;

extern uint8_t exciter_initial_event;
extern uint8_t exciter_over_event;

extern uint8_t exciter_state;
extern uint8_t preamble_cnt;
extern uint8_t burst_cnt;
extern uint8_t pattern_cnt;
extern uint8_t exciter_data_cnt;
extern uint16_t pattern_data;
extern uint16_t pattern_temp;

extern uint8_t exciter_data_len;
extern uint8_t exciter_send_temp;
extern uint8_t exciter_send_index;
extern uint8_t *p_exciter_send_buff;
extern exciter_man_data_t exciter_man_data;

void exciter_init(void);

void exciter_test(void);

void exciter_data_init(void);

void set_exciter_data(uint8_t *data, uint8_t data_len);

void set_exciter_man_data(uint8_t if_crc);

int exciter_initial_handler(void);

int exciter_over_handler(void);

#endif

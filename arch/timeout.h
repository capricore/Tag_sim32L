#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <si32_device.h>
#include <SI32_TIMER_A_Type.h>


#define US_TIMEOUT_ARRAY_LEN 5
#define MS_TIMEOUT_ARRAY_LEN 10

#define TIMEOUT_MS	1
#define TIMEOUT_US	2

typedef struct timeout_item_t
{
	int time;
	int flag;
	void *pEvent;
}timeout_item;

void timeout_init(void);

void us_timer_init(void);

void ms_timer_init(void);

void timeout_item_init(void);

uint16_t get_timeout_time(uint8_t us_ms);

void set_timeout_time(uint16_t time, uint8_t us_ms);

void timeout_add_item(int time, void *pEvent, uint8_t us_ms);

void timeout_remove_item(uint8_t *p_event, uint8_t us_ms);

void timeout_updata_item(uint8_t us_ms);

#endif


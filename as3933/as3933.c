#include "as3933_include.h"
#include "crc.h"
#include "systick.h"

uint8_t rValue[20];

extern uint8_t wake;
uint32_t dat = 0;
uint8_t k_3933 = 0;
uint16_t ex_id = 0;
uint16_t ex_crc = 0;
uint8_t rssi1 = 0;
uint8_t rssi2 = 0;
uint16_t calcout_crc=0;
uint16_t lastEID;
//extern uint32_t DataTxTime;
uint32_t as_timeout = 0;
void as3933_port_init(void)
{
	as3933_spi_port_init();
	AS3933_PINDAT_CLK_IN();
	AS3933_PINDAT_DAT_IN();
}


void as3933_init(void)
{
	
	as3933_port_init();
	as3933_spi_init();
	spi0_as_register_direct_command(0x04);	 //reset registers to default
	spi0_as_register_direct_command(0x03);	 //clear false wake-up
	spi0_as_register_direct_command(0x00);	 //clear wake-up state to listening mode
	spi0_as_register_direct_command(0x01);	 //clear RSSI
	spi0_as_register_direct_command(0x02);	 //trim_osc

	as3933_hal_register_write(0x00, 0x4A); 
	as3933_hal_register_write(0x01, 0x0A);		 
	as3933_hal_register_write(0x02, 0xA0);		 
	as3933_hal_register_write(0x03, 0x61);		 
	as3933_hal_register_write(0x04, 0x10);		 
	as3933_hal_register_write(0x05, 0xCD);//abcd		 
	as3933_hal_register_write(0x06, 0xAB);		 
	as3933_hal_register_write(0x07, 0x07);		 
	as3933_hal_register_write(0x11, 0x02);
	as3933_hal_register_write(0x12, 0x02);
	
	rValue[0] = as3933_hal_register_read(0x0);
	rValue[1] = as3933_hal_register_read(0x01);
	rValue[2] = as3933_hal_register_read(0x02);
	rValue[3] = as3933_hal_register_read(0x03);
	rValue[4] = as3933_hal_register_read(0x04);
	rValue[5] = as3933_hal_register_read(0x05);
	rValue[6] = as3933_hal_register_read(0x06);
	rValue[7] = as3933_hal_register_read(0x07);
	rValue[8] = as3933_hal_register_read(0x08);
	rValue[9] = as3933_hal_register_read(0x09);
	rValue[10] = as3933_hal_register_read(0x0a);
	rValue[11] = as3933_hal_register_read(0x0b);
	rValue[12] = as3933_hal_register_read(0x0c);
	rValue[13] = as3933_hal_register_read(0x0d);
	rValue[14] = as3933_hal_register_read(0x0e);
	rValue[15] = as3933_hal_register_read(0x0f);
	rValue[16] = as3933_hal_register_read(0x10);
	rValue[17] = as3933_hal_register_read(0x11);
	rValue[18] = as3933_hal_register_read(0x12);
	rValue[19] = as3933_hal_register_read(0x13);
//	spi0_as_register_direct_command(0x00);
	
	/*	
	delay_us(20);
	delay_interval++;
	if(!(delay_interval&0x0003ff))
	{
		led_ctr++;
		if((led_ctr&0x03)==0)
			led3_on();
		if((led_ctr&0x03)==1)
			led3_off();
		if((led_ctr&0x03)==2)
			led2_on();
		if((led_ctr&0x03)==3)
			led2_off();
	}
	*/
}

uint32_t excite_right_cnt = 0;
uint32_t excite_false_cnt = 0;
uint8_t id_buff[2];
uint8_t countBits = 0;
//uint8_t wake = 0;

int as3933_data_read(void) 
{
	int ret = 0;
	uint8_t cl_dat = 0;
	uint8_t last_cl_dat = 0;
	
	dat = 0;
	countBits = 0;
//	set_systick_delay(50);
//	systick_init();
//	Systick_Init();
//	DataTxTime = 40;
	as_timeout = 0;
	while((countBits < 32)&&(as_timeout < 12000))
	{
		as_timeout++;
		cl_dat = as3933_pindat_read_clk();
		
		if((cl_dat == 1) && (last_cl_dat == 0)) 
		{
			dat |= as3933_pindat_read_dat();
			countBits++;
				if (countBits < 32)
				{	
					dat <<= 1;
				}
		} 
		last_cl_dat = cl_dat;
		
	}
//	systick_off();
//	Systick_off();
//	delay_init(20);

	ex_id  = (uint16_t)(dat >> 16);
	ex_crc = (uint16_t)(dat);
	
	id_buff[0] = (uint8_t)(ex_id >> 8);
	id_buff[1] = (uint8_t)(ex_id);
	
	rssi1 = as3933_hal_register_read(0x0a) & 0x1f;
	rssi2 = as3933_hal_register_read(0x0b) & 0x1f;
	
	calcout_crc = crc_calculate(0xFFFF, id_buff, 2);
	calcout_crc = crc_finish(calcout_crc);
//	if((id_buff[0] == 0X44)&& id_buff[1] == 0Xff)
//	{
//		excite_right_cnt++;
//	}
	if(calcout_crc == ex_crc)
	{
		ret = 0;
		excite_right_cnt++;
//		wake = 1;
	}
	else
	{
		ret = -1;
	}
	excite_false_cnt++;
	spi0_as_register_direct_command(0x00);	 //clear wake-up state to listening mode
	return ret;
}

void as3933_wake_int_init(void)
{
	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_0, 0x0200);
	SI32_PBCFG_A_select_interrupt_0_pin(SI32_PBCFG_0, 9);	
	SI32_PBCFG_A_select_interrupt_0_rising_edge_mode(SI32_PBCFG_0);	
	SI32_PBCFG_A_enable_interrupt_0(SI32_PBCFG_0);	
//	NVIC_SetPriority(PBEXT0_IRQn,52);
	NVIC_ClearPendingIRQ(PBEXT0_IRQn);	
	NVIC_EnableIRQ(PBEXT0_IRQn);	
	NVIC_SetPriority(PBEXT0_IRQn,50);
}

void PBEXT0_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(PBEXT0_IRQn);	
	NVIC_DisableIRQ( PBEXT0_IRQn  );
	
	NVIC_ClearPendingIRQ(PBEXT0_IRQn);	
	NVIC_EnableIRQ( PBEXT0_IRQn  );
}


uint32_t pmatch_cnt = 0;
void PMATCH_IRQHandler(void)
{
	led_on();
//	NVIC_DisableIRQ(PMATCH_IRQn);
	NVIC_ClearPendingIRQ(PMATCH_IRQn);
	SI32_PMU_A_clear_wakeup_flags(SI32_PMU_0);
	NVIC_DisableIRQ(PMATCH_IRQn);
	wake = 1;
//	as3933_data_read();

// 	spi0_as_register_direct_command(0x00);	 //clear wake-up state to listening mode

//	NVIC_ClearPendingIRQ(PMATCH_IRQn);	
//	NVIC_EnableIRQ( PMATCH_IRQn  );	
	
} 


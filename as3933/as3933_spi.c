#include "as3933_include.h"


void as3933_spi_port_init(void)
{

//	SI32_PBSTD_A_write_pbskipen(SI32_PBSTD_0,0x01);
	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_0, 0x0080);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0140);	

	SI32_PBCFG_A_enable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_SPI0EN);
	AS3933_SPI_CS_OUT();
	AS3933_SPI_CS_DISABLE();	
}

void as3933_spi_port_off(void)
{
	SI32_PBCFG_A_disable_xbar0_peripherals(SI32_PBCFG_0, SI32_PBCFG_A_XBAR0_SPI0EN);	
// 	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_0, 0x0100);
// 	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_0, 0x0080);
// 	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_0, 0x0040);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0100);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0080);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0040);	
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_0, 0x0100);
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_0, 0x0040);
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x0008);
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x0010);
}
void as3933_spi_init(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_SPI0);	
                                         
	SI32_SPI_B_select_master_mode(SI32_SPI_0);                   //set master mode
	SI32_SPI_B_select_3wire_master_mode(SI32_SPI_0);
	SI32_SPI_B_write_clkrate(SI32_SPI_0, 0x00000013);            //set 500Khz
	SI32_SPI_B_set_data_length(SI32_SPI_0, 8);                   //set data_length 8bits

	SI32_SPI_B_select_clock_idle_high(SI32_SPI_0);               
	SI32_SPI_B_select_data_change_second_edge(SI32_SPI_0);
	SI32_SPI_B_select_direction_msb_first(SI32_SPI_0);           //set MSB mode

	SI32_SPI_B_flush_rx_fifo(SI32_SPI_0);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI_0);

	SI32_SPI_B_clear_all_interrupts(SI32_SPI_0);

	SI32_SPI_B_enable_module(SI32_SPI_0);	
}

uint8_t SPI0_ReadWrite(uint8_t data)
{
	uint8_t val;
//	uint32_t i=0;
	
	SI32_SPI_B_clear_all_interrupts(SI32_SPI_0);

	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI_0, data);
	
	while(0 == SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI_0));
	
	SI32_SPI_B_clear_underrun_interrupt(SI32_SPI_0);
	
 	val = SI32_SPI_0->CONTROL.RFCNT;
	
 	while(val==0)
 	{
 		val = SI32_SPI_0->CONTROL.RFCNT;
/*		
		i++;
		if(i==40000)
		{
			i++;
//			time_out_flag=1;
			return 0;
		}
*/		
 	}
	val = SI32_SPI_0->DATA.U8;
	
	SI32_SPI_B_flush_rx_fifo(SI32_SPI_0); 
	
	return val;
}


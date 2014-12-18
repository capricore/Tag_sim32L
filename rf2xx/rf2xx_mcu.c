#include "rf2xx_include.h"
#include "mcu.h"
#define RF2XX_SPI_MAX_CLK 8000000

uint8_t rf2xx_interrupt_source;

void (*trx_end_callback)(void);


uint32_t get_rfspi_clkdiv(void)
{
	uint32_t clkdiv = 0;
	uint32_t fsck =  get_periph_clock();
	while(clkdiv < 16)
	{
		fsck >>= 1;
		fsck /= (clkdiv+1);
		if(fsck < RF2XX_SPI_MAX_CLK)
			break;
		clkdiv++;
	}
	return clkdiv;
//	return 2;
}

void rf2xx_spi_init(void)
{
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	uint32_t clkdiv = get_rfspi_clkdiv(); 
	uint32_t SI32_CLKCTRL_A_APBCLKG0_SPI = SI32_CLKCTRL_A_APBCLKG0_SPI1;
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x000000d0);//PB2.4 2.6 2.7
	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2, 0x0000020);
	
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_SPI|SI32_CLKCTRL_A_APBCLKG0_FLASHCTRL0);
	SI32_PBCFG_A_enable_spi1_on_crossbar(SI32_PBCFG_0); 	

	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
	SI32_SPI_B_select_nss_data_count(SI32_SPI, 0);	
	SI32_SPI_B_set_data_length(SI32_SPI, 8);
	
	SI32_SPI_B_write_clkrate(SI32_SPI, clkdiv);
	SI32_SPI_B_select_clock_idle_low(SI32_SPI);
	SI32_SPI_B_select_data_change_second_edge(SI32_SPI);
	SI32_SPI_B_select_direction_msb_first(SI32_SPI);
	
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI);
	SI32_SPI_B_clear_all_interrupts(SI32_SPI);
	
	SI32_SPI_B_select_operation_mode_normal(SI32_SPI);

	SI32_SPI_B_enable_module(SI32_SPI);
}



void rf2xx_port_init(void)
{
	PORT_CLK_ENABLE();	
	RF_RST_OUT();
	RF_SLP_OUT();
	RF_IRQ_ENABLE();
}

void rf2xx_port_off(void)
{
	SI32_PBCFG_A_disable_spi1_on_crossbar(SI32_PBCFG_0);
	SI32_PBSTD_A_disable_pullup_resistors(SI32_PBSTD_3);
	SI32_PBSTD_A_disable_pullup_resistors(SI32_PBSTD_2);
	SI32_PBSTD_A_disable_pullup_resistors(SI32_PBSTD_0);
// 	RF_SLP_LOW();
//	RF_RST_LOW();
//	RF_RST_IN();
 	RF_IRQ_DISABLE();
	RF_IRQ_LOW();
 	SPI1_MISO_HIGH();
 	SPI1_MOSI_HIGH();
 	SPI1_NSS_HIGH();
 	SPI1_SCK_HIGH();
}

void rf2xx_trx_reset(void)
{
	RF_RST_LOW();
	RF_SLP_LOW();
	delay_us(TIME_RESET);
	RF_RST_HIGH();
}

uint32_t succ_num = 0;
void PBEXT1_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(PBEXT1_IRQn);
	
	rf2xx_hal_register_read( RG_IRQ_STATUS, &rf2xx_interrupt_source);
	if ((rf2xx_interrupt_source & HAL_RX_START_MASK))
	{
		
	}
	else if (rf2xx_interrupt_source & HAL_TRX_END_MASK)
	{
		rf2xx_set_trx_end_flag();				
		if(rf_if_rx())
		{
			rf2xx_trx_recv_packet();
		}
		else
		{

			rf_rx_on();
			succ_num++;
		}
		
	}
	else if (rf2xx_interrupt_source & HAL_TRX_UR_MASK)
	{

	}
	else if (rf2xx_interrupt_source & HAL_PLL_UNLOCK_MASK)
	{

	}
	else if (rf2xx_interrupt_source & HAL_PLL_LOCK_MASK)
	{
	
	}
	else if (rf2xx_interrupt_source & HAL_BAT_LOW_MASK)
	{

	}
	else
	{

	}
	
	
}



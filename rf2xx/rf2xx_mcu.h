#ifndef RF2XX_MCU_H_
#define RF2XX_MCU_H_

#include "sim3l1xx.h"
#include "SI32_PBSTD_A_Type.h"
#include "SI32_CLKCTRL_A_Type.h"
#include "SI32_PBCFG_A_Type.h"

#define YINAO
#if defined GUANGDAI

#define SPI1_NSS_LOW()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000080);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000080);}
#define SPI1_MOSI_LOW() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000040);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000040);}
#define SPI1_MISO_LOW() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000020);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000020);}
#define SPI1_SCK_LOW() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000010);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000010);}

#define RF_RST_OUT()	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000004)
#define RF_SLP_OUT()	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000008)

#define RF_RST_HIGH()	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000004)

//#if 0
#define RF_RST_LOW()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000004);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000004);}
//#endif
//#define RF_RST_LOW()	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000002)


#define RF_SLP_HIGH()	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000008)

//#if 0
#define RF_SLP_LOW()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000008);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000008);}
//#endif
//#define RF_SLP_LOW()	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000004)

#define RF_IRQ_DISABLE()	SI32_PBCFG_A_disable_interrupt_1(SI32_PBCFG_0)
#define RF_IRQ_LOW() 	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x00000001);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x00000001);}

#define RF_GET_SLP()	SI32_PBSTD_A_read_pin(SI32_PBSTD_2, 3)

#define PORT_CLK_ENABLE() SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0)

//		SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2, 1);\

// //SI32_PBSTD_A_write_pbskipen()
#define RF_IRQ_ENABLE()	\
	do{\
		SI32_PBSTD_A_write_pbskipen(SI32_PBSTD_3, 0x0001);	\
		SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_3, 0x0001);\
		SI32_PBCFG_A_select_interrupt_1_pin(SI32_PBCFG_0, 8);	\
		SI32_PBCFG_A_select_interrupt_1_rising_edge_mode(SI32_PBCFG_0);	\
		SI32_PBCFG_A_enable_interrupt_1(SI32_PBCFG_0);	\
		NVIC_ClearPendingIRQ(PBEXT1_IRQn);	\
		NVIC_EnableIRQ(PBEXT1_IRQn);	\
	}while(0);

#elif defined YINAO
#define SPI1_NSS_LOW()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000080);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000080);}
#define SPI1_NSS_HIGH()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000080);\
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000080);}
#define SPI1_MOSI_HIGH() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000040);\
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000040);}
#define SPI1_MISO_LOW() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000020);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000020);}
#define SPI1_MISO_HIGH() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000020);\
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000020);}
#define SPI1_SCK_LOW() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000010);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000010);}
#define SPI1_SCK_HIGH() {SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000010);\
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000010);}

#define RF_RST_OUT()	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x00000004)
#define RF_SLP_OUT()	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000008)

#define RF_RST_HIGH()	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_3, 0x00000004)

//#if 0
#define RF_RST_LOW()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x00000004);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x00000004);}
//#endif
//#define RF_RST_LOW()	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000002)
#define RF_RST_IN()   SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_3, 0x0004);

#define RF_SLP_HIGH()	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_2, 0x00000008)

//#if 0
#define RF_SLP_LOW()	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000008);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000008);}
//#endif
//#define RF_SLP_LOW()	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000004)

#define RF_IRQ_DISABLE()	SI32_PBCFG_A_disable_interrupt_1(SI32_PBCFG_0)
#define RF_IRQ_LOW() 	{SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x00000001);\
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x00000001);}

#define RF_GET_SLP()	SI32_PBSTD_A_read_pin(SI32_PBSTD_2, 3)

#define PORT_CLK_ENABLE() SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0)

//		SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2, 1);\

// //SI32_PBSTD_A_write_pbskipen()
#define RF_IRQ_ENABLE()	\
	do{\
		SI32_PBSTD_A_write_pbskipen(SI32_PBSTD_2, 0x0000);	\
		SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2, 0x0001);\
		SI32_PBCFG_A_select_interrupt_1_pin(SI32_PBCFG_0, 0);	\
		SI32_PBCFG_A_select_interrupt_1_rising_edge_mode(SI32_PBCFG_0);	\
		SI32_PBCFG_A_enable_interrupt_1(SI32_PBCFG_0);	\
		NVIC_ClearPendingIRQ(PBEXT1_IRQn);	\
		NVIC_EnableIRQ(PBEXT1_IRQn);	\
	}while(0);

#endif
extern uint8_t rf2xx_trx_end;

void rf2xx_spi_init(void);
	
void rf2xx_port_off(void);

//void delay_us(uint32_t cnt);

void rf2xx_trx_reset(void);

void rf2xx_port_init(void);

#endif



#include "arch_include.h"
#include "as3933.h"
#include "as3933_hal.h"
#include "rf2xx_include.h"
#include "systick.h"
#include "as3933_spi.h"
uint8_t false_wake_up;
extern uint8_t wake;
extern uint8_t enter_config_event;
extern int flag;
extern uint64_t caca;
extern uint64_t next_wakeup_time;
void led_off(void)
{
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x0200);
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_3, 0x0200);
}
void led_on(void)
{
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x0200);//PB3.9
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x0200);
}

void sync_port_on(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0008);
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_0, 0x0008);	
}

void sync_port_off(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0008);
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_0, 0x0008);
}

void sync_interval_port_on(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0004);
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_0, 0x0004);	
}

void sync_interval_port_off(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0004);
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_0, 0x0004);
}

void DCDC0_clk_enable(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0,
									  SI32_CLKCTRL_A_APBCLKG0_DCDC0CEN_ENABLED_U32
									  |SI32_CLKCTRL_A_APBCLKG0_LCD0CEN_ENABLED_U32);
	SI32_CLKCTRL_A_enable_apb_to_modules_1(SI32_CLKCTRL_0,
									  SI32_CLKCTRL_A_APBCLKG1_MISC0CEN_ENABLED_U32);
}

void open_debug(void)
{
	SI32_SCONFIG_A_enable_power_mode_8_debugging(SI32_SCONFIG_0);
}


void pmu_init(void)
{
	DCDC0_clk_enable();
	SI32_DCDC_A_enter_bypass_mode(SI32_DCDC_0);

	vdrv_on();
	SI32_SCONFIG_A_enable_power_mode_8_debugging(SI32_SCONFIG_0);
}

void trig_init(void)
{
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0008);
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_0, 0x0008);	
}

void trig_on(void)
{
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_0, 0x0008);	
}

void trig_off(void)
{
	SI32_PBSTD_A_write_pins_high(SI32_PBSTD_0, 0x0008); 
}

uint32_t enter_sleep_cnt = 0;
extern int tDelay;
void pmu_enter_sleep(void)
{
	uint32_t ldo_saved_value;
	tDelay = 0;
// 	if(flag = 1)
// 	{
// 		while(caca<next_wakeup_time)
// 		{
// 			caca = get_current_time();
// 		}
// 		__NOP;
// 	}
	enter_sleep_cnt++;
	rf2xx_tat_enter_sleep_mode();
 	rf2xx_port_off();
	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_0, 0x0001);	
	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_0, 0x0001);
// 	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_2, 0x0004);	
// 	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_2, 0x0004);
// 	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x0003);	
// 	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x0003);
// 	vdrv_off();
// 	led_off();
// 	i2c_port_off();
//	as3933_spi_port_off();
	
	SI32_CLKCTRL_A_enable_power_mode_8(SI32_CLKCTRL_0);
	SysTick->CTRL = 0;  
	SI32_RTC_B_clear_alarm0_interrupt(SI32_RTC_0);

	ldo_saved_value = SI32_LDO_A_read_control(SI32_LDO_0);
	SI32_LDO_A_select_digital_bias_high(SI32_LDO_0);
	SI32_LDO_A_select_memory_bias_high(SI32_LDO_0);
	SI32_LDO_A_select_analog_bias_high(SI32_LDO_0);
	
// 	SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3,0x0002);
// 	SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3,0x0002);


//	SI32_LDO_A_write_control(SI32_LDO_0, 0x00363636); // 1.9 V
	SI32_LDO_A_write_control(SI32_LDO_0, 0x00343434);

	SI32_VMON_B_select_vbat_high_threshold(SI32_VMON_0);		
	Systick_off();
	SI32_PMU_A_clear_wakeup_flags(SI32_PMU_0);
	SCB->SCR = SCB_SCR_SLEEPDEEP_Msk; 

	__DSB();
	__ISB();
	__WFI();
	Systick_Init();
	SI32_LDO_A_write_control(SI32_LDO_0, ldo_saved_value);
	if(wake == 1){
		if(as3933_data_read() != 0){
			false_wake_up = 1;
			wake = 0; 
			spi0_as_register_direct_command(0x00);	 //clear wake-up state to listening mode
		}
	NVIC_ClearPendingIRQ(PMATCH_IRQn);	
	NVIC_EnableIRQ( PMATCH_IRQn  );		
			led_off();
	}
	vdrv_on();
	wdtimer_stop();
}

void vdrv_on(void)
{
	SI32_PMU_A_select_vdrv_connected_to_vbat(SI32_PMU_0);
}

void vdrv_off(void)
{
	SI32_PMU_A_select_vdrv_high_impedence(SI32_PMU_0);	
}

void intwk_init(void)
{
	SI32_PBSTD_A_write_pbskipen(SI32_PBSTD_0, 0x0004);	
	SI32_PBCFG_A_select_interrupt_0_pin(SI32_PBCFG_0, 0);	
	SI32_PBCFG_A_select_interrupt_0_rising_edge_mode(SI32_PBCFG_0);	
	SI32_PBCFG_A_enable_interrupt_0(SI32_PBCFG_0);	
	NVIC_ClearPendingIRQ(PBEXT0_IRQn);	
	NVIC_EnableIRQ(PBEXT0_IRQn);		
}

void pinwk_init(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_0(SI32_CLKCTRL_0, SI32_CLKCTRL_A_APBCLKG0_PB0);
	SI32_PBSTD_A_disable_pullup_resistors(SI32_PBSTD_0);
	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_2, 0X0002); //PB2.1
	SI32_PMU_A_write_pwpol(SI32_PMU_0, 0x0000);	   
	SI32_PMU_A_write_pwen(SI32_PMU_0, 0x2000);	//wake.13	  
	SI32_PMU_A_clear_wakeup_flags(SI32_PMU_0);

	SI32_PMU_A_enable_pin_wake(SI32_PMU_0);
	SI32_PMU_A_enable_pin_wake_event(SI32_PMU_0);
                                                                                     
	SI32_PMU_A_clear_wakeup_flags(SI32_PMU_0);

	NVIC_ClearPendingIRQ(PMATCH_IRQn);
	NVIC_EnableIRQ(PMATCH_IRQn);
}





void led_start(void)
{
	uint32_t cnt = 50;
	while(cnt--)
	{
		led_on();
		delay_us(50000);
		led_off();
		delay_us(50000);
	}
}

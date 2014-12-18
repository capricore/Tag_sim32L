#include "rf2xx_include.h"
#include "systick.h"
uint32_t timedelay;
static uint8_t us_multiple;
static uint16_t ms_multiple;
uint8_t systick_num = 0;
void Systick_Init(void)
{
	systick_num = 1;
	if(SysTick_Config(SystemCoreClock/1000))	//设置为1毫秒
	{
		/*capture error*/
		while(1);
	}
}

// void delay_10us(uint32_t t) {
// 	SysTick_Config(SystemCoreClock/100000);
// 	timedelay = t;
// 	while(timedelay!=0){
// 	}
// 	SysTick_Config(SystemCoreClock/1000);
// }
// void delay_1ms(uint32_t t) {
// // 	SysTick_Config(32000);
// 	timedelay = t;
// 	while(timedelay!=0) {
// 	}
// }

// void delay_500ms(uint32_t t) {
// 	SysTick_Config(SystemCoreClock/2);
// 	timedelay = t;
// 	while(timedelay!=0){
// 	}
// 	SysTick_Config(SystemCoreClock/1000);
// }

/*void delay_us(uint32_t t) {
	int i, j;
	for (i=0; i<t; i++) {
		for (j=0; j<4; j++);
	}
}
*/

void Systick_off(void)
{
	systick_num = 2;
	SysTick->CTRL&=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	 
}


void delay_ms(uint32_t t) {
/*	timedelay = t;
	while(timedelay!=0) {
	}
*/
	while(t--)
	{
		delay_us(1000);
	}
}

void delay_s(uint32_t t) {
	uint32_t i=0;
	for (i=0; i<t; i++) {
		delay_ms(1000);
	}
}

void delay_init(uint8_t sys_clk) {                                       
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;	  			// 选择外部时钟作为基准时钟
	
	us_multiple = (sys_clk); 
	ms_multiple = (uint16_t)(us_multiple * 1000);
	
}


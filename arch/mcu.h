#ifndef MCU_H_
#define MCU_H_

void delay_us(uint32_t cnt);

//#define ENTER_CRITICAL_REGION()	__disable_irq()

//#define	LEAVE_CRITICAL_REGION()	__enable_irq()

#define ENTER_CRITICAL_REGION()	\
	cpu_sr = __get_PRIMASK();\
	__set_PRIMASK(1)

#define	LEAVE_CRITICAL_REGION()	\
	__set_PRIMASK(0)

#endif


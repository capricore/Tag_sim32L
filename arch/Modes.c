#include "arch_include.h"
#include "sysparam_include.h"

void system_init_from_reset(void)
{
	sysclk_init_from_reset();
	
 	wdtimer_stop();

	pb_init_from_reset();

	resume_sysparam();

	pmu_init();
	rtc_init();

 	pinwk_init();

//	trig_init();
//	intwk_init();
	
	open_debug();
}



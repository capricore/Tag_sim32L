#include <SI32_WDTIMER_A_Type.h>
#include <si32_device.h>
#include <SI32_CLKCTRL_A_Type.h>

void wdtimer_stop(void)
{
	SI32_CLKCTRL_A_enable_apb_to_modules_1(SI32_CLKCTRL_0,
										   SI32_CLKCTRL_A_APBCLKG1_MISC1);	
	SI32_WDTIMER_A_stop_counter(SI32_WDTIMER_0);
}


#ifndef PMU_H
#define PMU_H

#include "SI32_SCONFIG_A_Type.h"
#include "SI32_DCDC_A_Type.h"
#include "SI32_CLKCTRL_A_Type.h"
#include "SI32_PMU_A_Type.h"
#include <SI32_LDO_A_Type.h>

void pmu_init(void);

void pmu_enter_sleep(void);

void open_debug(void);

void vdrv_on(void);

void vdrv_off(void);

void pinwk_init(void);

void intwk_init(void);

void led_on(void);

void led_off(void);

void led_start(void);

void sync_port_on(void);

void sync_port_off(void);

void trig_init(void);

void trig_on(void);

void trig_off(void);

void sync_interval_port_on(void);

void sync_interval_port_off(void);

#endif


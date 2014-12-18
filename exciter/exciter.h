#ifndef EXCITER_H
#define EXCITER_H

#include "si32_device.h"
#include "SI32_TIMER_A_Type.h"
#include "SI32_PBCFG_A_Type.h"
#include "SI32_PBSTD_A_Type.h"
#include "SI32_CLKCTRL_A_Type.h"

void wave_125k_init(void);

void wave_125k_start(void);

void wave_125k_stop(void);

void wave_4k_init(void);

void wave_4k_start(void);

void wave_4k_stop(void);

#endif

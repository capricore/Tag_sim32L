#ifndef __EVENT_EXCITER_H
#define __EVENT_EXCITER_H

#ifdef  __cplusplus
	extern "C" {
#endif
		
#define EX_TYPE_GENERAL 0
#define EX_TYPE_EN_EX 1
#define EX_TYPE_CONFIG 2
#define EX_TYPE_TEST 3
#define EX_TYPE_UNDEFINED 4

#include <stdint.h>

uint8_t exciterTypeAnalyze(uint16_t exciterID);


#ifdef  __cplusplus
	}
#endif		
		
#endif //__EVENT_EXCITER_H

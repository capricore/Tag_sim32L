#include "event_exciter.h"
#include "config.h"
#include "configuration.h"
extern uint16_t lastEID;

extern CFG_PARAS cfg_paras;

extern uint8_t secOverflow, minOverflow;


uint8_t exciterTypeAnalyze(uint16_t exciterID)
{
	uint8_t exciterType;
	uint16_t temp = exciterID;
	if (temp == cfg_paras.cfg_ptim) {
		exciterType = EX_TYPE_TEST;
	} else if (temp == cfg_paras.cfg_cmsk){
		exciterType = EX_TYPE_CONFIG;
	} else { 
		temp = exciterID & EXCITER_MASK;
		if (temp == cfg_paras.cfg_gmsk) {
			exciterType = EX_TYPE_GENERAL;
		}	else if (temp == (cfg_paras.cfg_emsk & EXCITER_MASK)){
			exciterType = EX_TYPE_EN_EX;
		} 
		
		else {
			exciterType = EX_TYPE_UNDEFINED;
		}
	}
	
	return exciterType;
}




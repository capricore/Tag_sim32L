#include "sysparam.h"
#include "sysparam_include.h"
#include "arch_include.h"

extern uint16_t id;
extern uint16_t panid;
uint32_t interval;
extern uint8_t param_temp_buff[FLASH_PAGE_SIZE];

void resume_sysparam(void)
{
	int flag = 0;
	copy_sysparam_to_buff();
	get_sysparam(ID_POS, (uint8_t *)&id, 2);
	if((id == 0x0)||(id == 0xffff))
	{
		id = 0x1111;
		set_sysparam_to_buff(ID_POS, (uint8_t *)&id, 2);
		flag = 1;		
	}
	get_sysparam(PANID_POS, (uint8_t *)&panid, 2);
	if((panid == 0)||(panid == 0xffff))
	{
		panid = 0x301;
		set_sysparam_to_buff(PANID_POS, (uint8_t*)&panid, 2);
		flag = 1;			
	}
	panid = 1;
	get_sysparam(INTERVAL_POS, (uint8_t *)&interval, 4);
	if((interval == 0) || (interval == 0XFFFFFFFF))
	{
		interval = 0x05;
		set_sysparam_to_buff(INTERVAL_POS, (uint8_t*)&interval, 4);
		flag = 1;		
	}
	interval = 10;
	if(flag)
	{
		copy_sysparam_to_flash();
	}
}



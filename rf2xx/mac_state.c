#include "rf2xx_include.h"

void mac_init(void)
{	
	rf2xx_Init( panid, id);
	rf2xx_protoc_init();
	rf_rx_on();
	rf2xx_recv_on();
}


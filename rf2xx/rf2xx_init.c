#include "rf2xx_include.h"

int rf2xx_Init(uint16_t panId, uint16_t myAddr) 
{
	int status;
	rf2xx_spi_init();
	if (rf2xx_tat_init( ) != TAT_SUCCESS) {
		status = -1;
	} else if (rf2xx_tat_set_operating_channel( ) != TAT_SUCCESS) {
		status = -1;
	} else if (rf2xx_tat_set_clock_speed( true, CLKM_DISABLED ) != TAT_SUCCESS) {
		status = -1;
	} else{

		rf2xx_tat_set_irq_mask();
		
		rf2xx_tat_set_enhanced_feature( );

		rf2xx_tat_set_bitrate( );
		
		rf2xx_tat_set_tx_power_level();
		
		rf2xx_tat_set_short_address( myAddr );
		
		rf2xx_tat_set_pan_id( panId);
		
		rf2xx_tat_set_device_role( false );

		rf2xx_tat_configure_csma( 4,3,4,0x7f); 
	
		rf2xx_tat_use_auto_tx_crc( true ); 

		status = 0;
	} 

	return status;
}

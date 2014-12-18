#include "rf2xx_include.h"
#include "arch_include.h"
#include "mcu.h"
static bool rf2xx_tat_if_sleeping( void )
{
    bool sleeping = false;

    if (RF_GET_SLP() != 0) {
        sleeping = true;
    }

    return sleeping;
}


tat_status_t rf2xx_tat_init( void )
{
    tat_status_t init_status = TAT_SUCCESS;

    delay_us( TIME_TO_ENTER_P_ON  );

    rf2xx_port_init( );

    rf2xx_trx_reset( ); 
	delay_us( TIME_TO_ENTER_P_ON  );
	
	rf2xx_hal_subregister_write( SR_TRX_CMD, CMD_FORCE_TRX_OFF );
	delay_us( TIME_TO_ENTER_P_ON  );

    if (rf2xx_tat_get_trx_state( ) != TRX_OFF) 
	{
        init_status = TAT_TIMED_OUT;
    } 
	else 
	{
		int16_t ret;
		uint8_t version_number;
		uint8_t part_number;
		uint8_t rg_man_id_0, rg_man_id_1;
		ret = rf2xx_hal_register_read(RG_PART_NUM, &part_number);
		if(ret < 0)
			return TAT_TIMED_OUT;
		if(part_number == RF230_PART_NUM)
		{	
			rf2xx_version = AT86_RF230;
			ret = rf2xx_hal_register_read( RG_VERSION_NUM, &version_number);
			if(ret < 0)
				return TAT_TIMED_OUT;
			if((version_number != RF230_REV_A) || (version_number != RF230_REV_B))
			{
				return TAT_UNSUPPORTED_DEVICE;
			}
			ret = rf2xx_hal_register_read(RG_MAN_ID_0, &rg_man_id_0);
			if(ret < 0)
				return TAT_TIMED_OUT;
			ret = rf2xx_hal_register_read(RG_MAN_ID_1, &rg_man_id_1);
			if(ret < 0)
				return TAT_TIMED_OUT;
			
			if((rg_man_id_0 != RF230_MAN_ID_0) && (rg_man_id_1 != RF230_MAN_ID_1))
				return TAT_UNSUPPORTED_DEVICE;
		}	
		else if(part_number == RF231_PART_NUM)
		{
			rf2xx_version = AT86_RF231;
			ret = rf2xx_hal_register_read( RG_VERSION_NUM, &version_number);
			if( ret < 0 )
			{
				return TAT_TIMED_OUT;
			}
			if(version_number != RF231_REV_A)
			{
				return TAT_UNSUPPORTED_DEVICE;
			}
			ret = rf2xx_hal_register_read(RG_MAN_ID_0, &rg_man_id_0);
			if(ret < 0)
				return TAT_TIMED_OUT;
			ret = rf2xx_hal_register_read(RG_MAN_ID_1, &rg_man_id_1);
			if(ret < 0)
				return TAT_TIMED_OUT;
			
			if((rg_man_id_0 != RF231_MAN_ID_0) && (rg_man_id_1 != RF231_MAN_ID_1))
				return TAT_UNSUPPORTED_DEVICE;			
		}
		else if(part_number == RF212_PART_NUM)
		{
			rf2xx_version = AT86_RF212;
			ret = rf2xx_hal_register_read( RG_VERSION_NUM, &version_number);
			if(ret < 0)
			{
				return TAT_TIMED_OUT;
			}
			if(version_number != RF212_REV_A)
			{
				return TAT_UNSUPPORTED_DEVICE;
			}
			ret = rf2xx_hal_register_read(RG_MAN_ID_0, &rg_man_id_0);
			if(ret < 0)
				return TAT_TIMED_OUT;
			ret = rf2xx_hal_register_read(RG_MAN_ID_1, &rg_man_id_1);
			if(ret < 0)
				return TAT_TIMED_OUT;
			
			if((rg_man_id_0 != RF212_MAN_ID_0) && (rg_man_id_1 != RF212_MAN_ID_1))
				return TAT_UNSUPPORTED_DEVICE;			
		}
		else
		{
			return TAT_NO_DEVICE;
		}
    }

    return init_status;
}

void rf2xx_tat_reset_state_machine( void )
{
    RF_SLP_LOW();
    delay_us( TIME_NOCLK_TO_WAKE );
    rf2xx_hal_subregister_write( SR_TRX_CMD, CMD_FORCE_TRX_OFF );
    delay_us( TIME_CMD_FORCE_TRX_OFF);
}

uint8_t rf2xx_tat_get_trx_state( void )
{
	uint8_t trx_state;
	rf2xx_hal_subregister_read( SR_TRX_STATUS, &trx_state );
    return trx_state;
}


uint8_t trx_current_state;
tat_status_t rf2xx_tat_set_trx_state( uint8_t new_state )
{
	uint8_t original_state;
    if (!((new_state == TRX_OFF ) || (new_state == RX_ON) || (new_state == PLL_ON) ||
        (new_state == RX_AACK_ON ) || (new_state == TX_ARET_ON ))) 
    {

        return TAT_INVALID_ARGUMENT;
    }

    if (rf2xx_tat_if_sleeping( ) == true) 
	{ 
		return TAT_WRONG_STATE; 
	}

    original_state = rf2xx_tat_get_trx_state( );

    if ((original_state == BUSY_RX ) || (original_state == BUSY_TX) ||
        (original_state == BUSY_RX_AACK) || (original_state == BUSY_TX_ARET)) 
    {
        return TAT_BUSY_STATE;
    }

    if (new_state == original_state) 
	{ 
		return TAT_SUCCESS; 
	}

    if( new_state == TRX_OFF )
	{
        rf2xx_tat_reset_state_machine( ); 
    } 
	else 
	{
        if ((new_state == TX_ARET_ON) && (original_state == RX_AACK_ON)) 
		{
            rf2xx_hal_subregister_write( SR_TRX_CMD, PLL_ON );
	    	delay_us( TIME_STATE_TRANSITION_PLL_ACTIVE);
        } 
		else if ((new_state == RX_AACK_ON) && (original_state == TX_ARET_ON)) 
		{
            rf2xx_hal_subregister_write( SR_TRX_CMD, PLL_ON );
	    	delay_us( TIME_STATE_TRANSITION_PLL_ACTIVE );
        }

        rf2xx_hal_subregister_write( SR_TRX_CMD, new_state );
		
        if (original_state == TRX_OFF) 
		{
	    	delay_us( TIME_TRX_OFF_TO_PLL_ACTIVE);
        } 
		else 
		{
	    	delay_us( TIME_STATE_TRANSITION_PLL_ACTIVE );
        } 
    }
	
    tat_status_t set_state_status = TAT_TIMED_OUT;

	trx_current_state = rf2xx_tat_get_trx_state();
	if(trx_current_state == new_state)
	{
		set_state_status = TAT_SUCCESS; 
	}
//     if( rf2xx_tat_get_trx_state( ) == new_state )
// 	{ 
// 		set_state_status = TAT_SUCCESS; 
// 	}

    return set_state_status;
}

int rf2xx_tat_get_operating_channel( void )
{
	uint8_t channel;
	uint8_t cc_number;
	uint8_t cc_band;
	int band_channel;
	if(RF_IS_23X)
	{
		rf2xx_hal_subregister_read( SR_CHANNEL, &channel );
		band_channel = channel;
	}
	else if(RF_IS_212)
	{
		rf2xx_hal_subregister_read(RF212_SR_CC_BAND, &cc_band);
		rf2xx_hal_register_read(RF212_RG_CC_CTRL_0, &cc_number);
		rf2xx_hal_subregister_read(SR_CHANNEL, &channel);
		band_channel = get_band_channel(cc_band, channel, cc_number);
		
	}

	return band_channel;
}

tat_status_t rf2xx_tat_set_operating_channel( void )
{
	tat_status_t channel_set_status;
	int channel;
	if(RF_IS_23X)
	{
		channel = channel_2_4G;
		if ((channel < RF23X_MIN_CHANNEL) || (channel > RF23X_MAX_CHANNEL)) 
		{
			return TAT_INVALID_ARGUMENT;
		}
		if (rf2xx_tat_if_sleeping() == true) 
		{ 
			return TAT_WRONG_STATE; 
		}
		if (rf2xx_tat_get_operating_channel( ) == channel) 
		{ 
			return TAT_SUCCESS; 
		}
		rf2xx_hal_subregister_write( SR_CHANNEL, (uint8_t)channel );

		channel_set_status = TAT_TIMED_OUT;
	
		if (rf2xx_tat_get_operating_channel( ) == channel) 
		{
			channel_set_status = TAT_SUCCESS;
		}	

	}
	else if(RF_IS_212)
	{	
		channel = rf212_band_set;
		if ((channel < RF212_MIN_CHANNEL) || (channel > RF212_MAX_CHANNEL)) 
		{
			return TAT_INVALID_ARGUMENT;
		}
		if (rf2xx_tat_if_sleeping() == true) 
		{ 
			return TAT_WRONG_STATE; 
		}
		if (rf2xx_tat_get_operating_channel( ) == channel) 
		{ 
			return TAT_SUCCESS; 
		}

		if(set_band_channel())
		{
			return TAT_INVALID_ARGUMENT;
		}

		rf2xx_hal_subregister_write(RF212_SR_CC_BAND, rf212_cc_band);
		rf2xx_hal_register_write(RF212_RG_CC_CTRL_0, rf212_cc_number);
		rf2xx_hal_subregister_write(SR_CHANNEL, rf212_channel);

		channel_set_status = TAT_TIMED_OUT;
		
		if (rf2xx_tat_get_operating_channel( ) == channel) 
		{
			channel_set_status = TAT_SUCCESS;
		}			

	}
	else
	{
		channel_set_status = TAT_NO_DEVICE;
	}
		
    return channel_set_status;
}

uint8_t rf2xx_tat_get_tx_power_level( void )
{	
	uint8_t tx_power;
	
	if(RF_IS_23X)
	{
		rf2xx_hal_subregister_read( SR_TX_PWR, &tx_power);
	}
	else if(RF_IS_212)
	{
		rf2xx_hal_register_read( RG_PHY_TX_PWR, &tx_power);
	}
	return tx_power;
}

tat_status_t rf2xx_tat_set_tx_power_level(void)
{
    if (rf2xx_tat_if_sleeping( ) == true) 
	{ 
		return TAT_WRONG_STATE; 
	}

	if(RF_IS_23X)
	{
		if (rf23x_txpower > TX_PWR_17_2DBM) 
		{ 
			return TAT_INVALID_ARGUMENT; 
		}
		rf2xx_hal_subregister_write( SR_TX_PWR, rf23x_txpower );
	}
	else if(RF_IS_212)
	{
		if (rf212_txpower < WPAN_TXPWR_N4DBM || rf212_txpower > WPAN_TXPWR_P2DBM) 
		{ 
			return TAT_INVALID_ARGUMENT; 
		}
				
		rf2xx_hal_register_write( RG_PHY_TX_PWR, rf212_txpower );
		rf2xx_hal_subregister_write(RF212_SR_PA_BOOST, rf212_pa_boost);

	}
	return TAT_SUCCESS;
}


tat_status_t rf2xx_tat_set_modulation_datarate(uint8_t mode) 
{
	uint8_t tmp;
	rf2xx_hal_register_read(RF212_RG_TRX_CTRL_2, &tmp);
	tmp &= 0xC0;
	tmp |= mode;
	rf2xx_hal_register_write(RF212_RG_TRX_CTRL_2, tmp);

	if(mode == BPSK_20 || mode==BPSK_40) 
	{
		rf2xx_hal_subregister_write(RF212_SR_GC_TX_OFFS, BPSK_TX_OFFSET);
	
		rf2xx_hal_subregister_write(RF212_SR_AACK_ACK_TIME, AACK_ACK_TIME_12_SYMBOLS);
	} 
	else 
	{
		rf2xx_hal_subregister_write(RF212_SR_GC_TX_OFFS, OQPSK_TX_OFFSET);
		rf2xx_hal_subregister_write(RF212_SR_AACK_ACK_TIME, AACK_ACK_TIME_12_SYMBOLS);
	}
	
	return TAT_SUCCESS;
}


tat_status_t rf2xx_tat_do_ed_scan( uint8_t *ed_level )
{
	int16_t ret;
    uint8_t current_state = rf2xx_tat_get_trx_state( );

    if ((current_state != RX_ON ) && (current_state != BUSY_RX)) 
	{
        return TAT_WRONG_STATE;
    }

    rf2xx_hal_register_write( RG_PHY_ED_LEVEL, 0 );

    delay_us( TIME_ED_MEASUREMENT ); 
    ret = rf2xx_hal_register_read( RG_PHY_ED_LEVEL, ed_level);
	
    if( ret< 0 ) return TAT_WRONG_STATE;	
	
    return TAT_SUCCESS;
}


uint8_t rf2xx_tat_get_cca_mode( void )
{
	uint8_t cca_mode;
    rf2xx_hal_subregister_read( SR_CCA_MODE, &cca_mode);
	return cca_mode;
}

uint8_t rf2xx_tat_get_ed_threshold( void )
{
	uint8_t cca_ed_threshold;
	rf2xx_hal_subregister_read( SR_CCA_ED_THRES, &cca_ed_threshold);
	return cca_ed_threshold;
}


tat_status_t rf2xx_tat_set_cca_mode( uint8_t mode, uint8_t ed_threshold )
{
    if ((mode != CCA_ED) && (mode != CCA_CARRIER_SENSE) && (mode != CCA_CARRIER_SENSE_WITH_ED)) 
	{
        return TAT_INVALID_ARGUMENT;
    }

    if (ed_threshold > RF2XX_MAX_ED_THRESHOLD) { return TAT_INVALID_ARGUMENT; }


    if (rf2xx_tat_if_sleeping( ) == true) { return TAT_WRONG_STATE; }

    rf2xx_hal_subregister_write( SR_CCA_MODE, mode );
    rf2xx_hal_subregister_write( SR_CCA_ED_THRES, ed_threshold );

    return TAT_SUCCESS;
}


tat_status_t rf2xx_tat_do_cca( void )
{
	uint8_t tat_state;
	int16_t ret;
	uint8_t status;
	tat_status_t cca_status;
	tat_state = rf2xx_tat_get_trx_state();
	if(tat_state == BUSY_RX)
	{
		return TAT_CCA_BUSY;
	}
	else if((tat_state == PLL_ON) || (tat_state == TRX_OFF) || (tat_state == TX_ARET_ON))
	{
		if(rf2xx_tat_set_trx_state(RX_ON) != TAT_SUCCESS)
			return  TAT_TIMED_OUT;
	}
	else
	{
		if(tat_state != RX_ON)
			return TAT_WRONG_STATE;
	}

	rf2xx_hal_subregister_write( SR_CCA_REQUEST, TAT_START_CCA );

	delay_us( TIME_CCA);

	ret = rf2xx_hal_register_read( RG_TRX_STATUS, &status);
	if( ret < 0)
	return TAT_TIMED_OUT;	
	cca_status = TAT_CCA_BUSY; 

	if ((status & TAT_CCA_DONE_MASK) != TAT_CCA_DONE_MASK) 
	{
	    cca_status = TAT_TIMED_OUT;
	} 
	else 
	{
	    if ((status & TAT_CCA_IDLE_MASK) != TAT_CCA_IDLE_MASK) 
		{
	        cca_status = TAT_CCA_BUSY;
	    } 
		else 
		{
	        cca_status = TAT_CCA_IDLE;
	    } 
	} 

	return cca_status;
}


tat_status_t rf2xx_tat_get_rssi_value( uint8_t *rssi )
{
    uint8_t current_state = rf2xx_tat_get_trx_state( );
    tat_status_t retval = TAT_WRONG_STATE;

    if ((current_state == RX_ON) || (current_state == BUSY_RX)) 
	{

        rf2xx_hal_subregister_read( SR_RSSI, rssi);
        retval = TAT_SUCCESS;
    }

    return retval;
}

uint8_t rf2xx_tat_batmon_get_voltage_threshold( void )
{
	uint8_t batmon_vth;
    rf2xx_hal_subregister_read( SR_BATMON_VTH, &batmon_vth);
	return batmon_vth;
}

uint8_t rf2xx_tat_batmon_get_voltage_range( void )
{
	uint8_t batmon_hr;
    rf2xx_hal_subregister_read( SR_BATMON_HR, &batmon_hr);
	return batmon_hr;
}

tat_status_t rf2xx_tat_batmon_configure( bool range, uint8_t voltage_threshold )
{
    if (voltage_threshold > BATTERY_MONITOR_HIGHEST_VOLTAGE) 
	{
        return TAT_INVALID_ARGUMENT;
    }

    if (rf2xx_tat_if_sleeping( ) == true) 
	{ 
		return TAT_WRONG_STATE; 
	}

    if (range == true) 
	{
        rf2xx_hal_subregister_write( SR_BATMON_HR, BATTERY_MONITOR_HIGH_VOLTAGE );
    } 
	else 
	{
        rf2xx_hal_subregister_write( SR_BATMON_HR, BATTERY_MONITOR_LOW_VOLTAGE );
    } 
	
    rf2xx_hal_subregister_write( SR_BATMON_VTH, voltage_threshold );

    return TAT_SUCCESS;
}

tat_status_t rf2xx_tat_batmon_get_status( void )
{
	uint8_t batmon;
    tat_status_t batmon_status = TAT_BAT_LOW;
	rf2xx_hal_subregister_read( SR_BATMON_OK, &batmon);
    if (batmon == BATTERY_MONITOR_VOLTAGE_UNDER_THRESHOLD) 
	{
        batmon_status = TAT_BAT_OK;
    }

    return batmon_status;
}

uint8_t rf2xx_tat_get_clock_speed( void )
{
	uint8_t clk_speed;
    rf2xx_hal_subregister_read( SR_CLKM_CTRL, &clk_speed);
	return clk_speed;
}

tat_status_t rf2xx_tat_set_clock_speed( bool direct, uint8_t clock_speed )
{
    if (clock_speed > CLKM_16MHZ) 
	{ 
		return TAT_INVALID_ARGUMENT; 
	}

    if (rf2xx_tat_get_clock_speed( ) == clock_speed) 
	{ 
		return TAT_SUCCESS; 
	}

    if (direct == false) 
	{
        rf2xx_hal_subregister_write( SR_CLKM_SHA_SEL, 1 );
    } 
	else 
	{
        rf2xx_hal_subregister_write( SR_CLKM_SHA_SEL, 0 );
    } 

    rf2xx_hal_subregister_write( SR_CLKM_CTRL, clock_speed );

    return TAT_SUCCESS;
}

tat_status_t rf2xx_tat_calibrate_filter( void )
{
	uint8_t fin_start;
	tat_status_t filter_calibration_status;
    uint8_t trx_state = rf2xx_tat_get_trx_state( );

    if ((trx_state != TRX_OFF ) && (trx_state != PLL_ON)) 
	{ 
		return TAT_WRONG_STATE; 
	}

    rf2xx_hal_subregister_write( SR_FTN_START, 1 );

    delay_us( TIME_FTN_TUNING );

    filter_calibration_status = TAT_TIMED_OUT;
	rf2xx_hal_subregister_read( SR_FTN_START, &fin_start);
    if (fin_start == FTN_CALIBRATION_DONE) 
	{
        filter_calibration_status = TAT_SUCCESS;
    }

    return filter_calibration_status;
}

tat_status_t rf2xx_tat_calibrate_pll( void )
{
	tat_status_t pll_calibration_status;

    if (rf2xx_tat_get_trx_state( ) != PLL_ON) { return TAT_WRONG_STATE; }

    rf2xx_hal_subregister_write( SR_PLL_DCU_START, 1 );
    rf2xx_hal_subregister_write( SR_PLL_CF_START, 1 );

    delay_us( TIME_PLL_LOCK * 2 );

    pll_calibration_status = TAT_TIMED_OUT;

    return pll_calibration_status;
}


tat_status_t rf2xx_tat_enter_sleep_mode( void )	
{
    if (rf2xx_tat_if_sleeping( ) == true) 
	{ 
		return TAT_SUCCESS; 
	}

    rf2xx_tat_reset_state_machine( ); 

    tat_status_t enter_sleep_status = TAT_TIMED_OUT;

    if (rf2xx_tat_get_trx_state( ) == TRX_OFF) 
	{
        RF_SLP_HIGH();
		
        enter_sleep_status = TAT_SUCCESS;
    }

    return enter_sleep_status;
}

tat_status_t rf2xx_tat_leave_sleep_mode( void )
{
    if (rf2xx_tat_if_sleeping( ) == false) 
	{ 	
		return TAT_SUCCESS; 
	}

    RF_SLP_LOW( );

    delay_us( TIME_SLEEP_TO_TRX_OFF );

    tat_status_t leave_sleep_status = TAT_TIMED_OUT;

    if (rf2xx_tat_get_trx_state( ) == TRX_OFF) 
	{
        leave_sleep_status = TAT_SUCCESS;
    }

    return leave_sleep_status;
}

void rf2xx_tat_use_auto_tx_crc( bool auto_crc_on )
{
	if(RF_IS_230)
	{
		if (auto_crc_on == true) 
		{
			rf2xx_hal_subregister_write( RF230_SR_TX_AUTO_CRC_ON, 1 );
		} 
		else 
		{
			rf2xx_hal_subregister_write( RF230_SR_TX_AUTO_CRC_ON, 0 );
		}
	}
	else if(RF_IS_231_OR_212)
	{
		if (auto_crc_on == true) 
		{
			rf2xx_hal_subregister_write( RF231_RF212_SR_TX_AUTO_CRC_ON, 1 );
		} 
		else 
		{
			rf2xx_hal_subregister_write( RF231_RF212_SR_TX_AUTO_CRC_ON, 0 );
		}
	}
}

uint8_t rf2xx_tat_get_device_role( void )
{
	uint8_t role;
	rf2xx_hal_subregister_read( SR_I_AM_COORD, &role);
    return role;
}

void rf2xx_tat_set_device_role( bool i_am_coordinator )
{
    if (i_am_coordinator == true) 
	{
        rf2xx_hal_subregister_write( SR_I_AM_COORD, 0);
    } 
	else 
	{
        rf2xx_hal_subregister_write( SR_I_AM_COORD, 0);
    } 
}

void rf2xx_tat_set_irq_mask(void)
{
	if(RF_IS_230)
	{
		rf2xx_hal_register_write( RG_IRQ_MASK, RF230_SUPPORTED_INTERRUPT_MASK);
	}
	else if(RF_IS_231_OR_212)
	{
		rf2xx_hal_register_write( RG_IRQ_MASK, RF231_RF212_SUPPORTED_INTERRUPT_MASK);
	}
	
}

uint8_t rf2xx_tat_get_irq_mask(void)
{
	uint8_t irq_msk;
	rf2xx_hal_register_read( RG_IRQ_MASK, &irq_msk);
	return irq_msk;
}

uint8_t rf2xx_tat_get_irq_status(void)
{
	uint8_t irq_status;
	rf2xx_hal_register_read( RG_IRQ_STATUS, &irq_status);
	return irq_status;
}

void rf2xx_tat_set_enhanced_feature(void)
{
	if(RF_IS_231_OR_212)
	{
		rf2xx_hal_subregister_write(RF231_RF212_SR_AACK_FVN_MODE, 0);
		rf2xx_hal_subregister_write(SR_AACK_SET_PD, 0); 	
		rf2xx_hal_subregister_write(RF231_RF212_SR_RX_SAFE_MODE, 1);
		rf2xx_hal_subregister_write(RF231_RF212_SR_IRQ_MASK_MODE, 0);
		rf2xx_hal_subregister_write(RF231_RF212_SR_IRQ_2_EXT_EN, 0);
	}
}

void rf2xx_tat_set_bitrate(void)
{
	if(RF_IS_231)
	{
		rf2xx_hal_subregister_write(RF231_SR_OQPSK_DATA_RATE, rf231_bitrate);
		rf2xx_hal_subregister_write(RF231_SR_ANT_DIV_EN, RF231_ANT_DIV_DISABLE );
		rf2xx_hal_subregister_write(RF231_SR_ANT_EXT_SW_EN, RF231_ANT_EXT_SW_SWITCH_DISABLE); 		
	}
	else if(RF_IS_212)
	{
		rf2xx_tat_set_modulation_datarate(rf212_bitrate) ; 
//		rf2xx_tat_set_tx_power_level(WPAN_TXPWR_P10DBM, R212_PABOOST_ON) ; 
	}			
}

uint16_t rf2xx_tat_get_pan_id( void )
{
	int16_t ret;
	uint16_t pan_id;
	uint8_t pan_id_15_8;
	uint8_t pan_id_7_0;
    ret = rf2xx_hal_register_read( RG_PAN_ID_1, &pan_id_15_8);
	if(ret < 0)
	  	return 0;

    ret = rf2xx_hal_register_read( RG_PAN_ID_0, &pan_id_7_0);
    if( ret < 0)
	return 0;

    pan_id = ((uint16_t)(pan_id_15_8 << 8)) | (pan_id_7_0);

    return pan_id;
}

void rf2xx_tat_set_pan_id( uint16_t new_pan_id )
{
    uint8_t pan_byte = new_pan_id & 0xFF; 
    rf2xx_hal_register_write( RG_PAN_ID_0, pan_byte );

    pan_byte = (new_pan_id >> 8) & 0xFF;
    rf2xx_hal_register_write( RG_PAN_ID_1, pan_byte );
}

uint16_t rf2xx_tat_get_short_address( void )
{
	int16_t ret;
	uint8_t short_address_15_8;
	uint8_t short_address_7_0;
	uint16_t short_address;
    ret = rf2xx_hal_register_read( RG_SHORT_ADDR_1, &short_address_15_8);
	if(ret < 0)
		return 0;

    ret = rf2xx_hal_register_read( RG_SHORT_ADDR_0, &short_address_7_0);

 	if(ret < 0)
 		return 0;
	
    short_address = ((uint16_t)(short_address_15_8 << 8)) | short_address_7_0;

    return short_address;
}

void rf2xx_tat_set_short_address( uint16_t new_short_address )
{
    uint8_t short_address_byte = new_short_address & 0xFF; 
    rf2xx_hal_register_write( RG_SHORT_ADDR_0, short_address_byte );

    short_address_byte = (new_short_address >> 8) & 0xFF; 
    rf2xx_hal_register_write( RG_SHORT_ADDR_1, short_address_byte );
}

uint8_t rf2xx_tat_get_ed_value(void )
{
	uint8_t ed_value;
	rf2xx_hal_register_read( RG_PHY_ED_LEVEL, &ed_value);

	return ed_value;
}

tat_status_t rf2xx_tat_configure_csma( uint8_t min_be, uint8_t frame_retry,uint8_t csma_retry,uint16_t csma_seed)
{	
	uint8_t reg_value;
    if (rf2xx_tat_if_sleeping( ) == true) 
	{ 
		return TAT_WRONG_STATE; 
	}

	if(RF_IS_231_OR_212)
	{
		reg_value = min_be;
		rf2xx_hal_subregister_write(RF231_RF212_SR_MIN_BE, reg_value);

	    reg_value = ((frame_retry & 0x0f)<<4);
	    reg_value |=((csma_retry & 0x07)<<1);
	    rf2xx_hal_register_write(RG_XAH_CTRL, reg_value);

		
	    reg_value = (uint8_t)csma_seed;
	    rf2xx_hal_register_write(RG_CSMA_SEED_0, reg_value);
	    reg_value = ((csma_seed >>8)&0x0f);
	    rf2xx_hal_subregister_write(SR_CSMA_SEED_1, reg_value);

	}
	else if(RF_IS_230)
	{
		reg_value = ((frame_retry & 0x0f)<<4);
		reg_value |=((csma_retry & 0x07)<<1);
		rf2xx_hal_register_write(RG_XAH_CTRL, reg_value);
		reg_value = (uint8_t)csma_seed;
		rf2xx_hal_register_write(RG_CSMA_SEED_0, reg_value);
		if(min_be > 3)
			min_be = 3;
		reg_value =((min_be & 0x03)<<6);
		reg_value |=((csma_seed >>8)& 0x07);
		rf2xx_hal_register_write(RG_CSMA_SEED_1, reg_value);

	}

    return TAT_SUCCESS;
}

tat_status_t rf2xx_tat_send_data_with_retry( uint8_t frame_length, uint8_t *frame, uint8_t retries )
{
    tat_status_t task_status = TAT_CHANNEL_ACCESS_FAILURE;
	uint8_t transaction_status;

    if ((frame_length > RF2XX_MAX_TX_FRAME_LENGTH) ||
        (frame_length < TAT_MIN_IEEE_FRAME_LENGTH)) 
    {
        return TAT_INVALID_ARGUMENT;
    }

    if (rf2xx_tat_get_trx_state( ) != TX_ARET_ON)  
		 return TAT_WRONG_STATE; 

    rf2xx_clear_trx_end_flag();

	RF_SLP_HIGH();
	RF_SLP_LOW();
    rf2xx_hal_frame_write( frame, frame_length ); 

    bool retry = false;
    do{
        while (rf2xx_get_trx_end_flag() == 0) ;

        rf2xx_hal_subregister_read( SR_TRAC_STATUS, &transaction_status);     

        switch(transaction_status)
        {
          case TAT_SUCCESS_DATA_PENDING:
		  case TAT_TRANSMISSION_SUCCESS: 	 
		  	  task_status = TAT_SUCCESS;
			  retry = false;
			  break;
		  case TAT_TRANSMISSION_INVALID:
		  	  task_status = TAT_SUCCESS;
			  retry = false;
			  break;		  	
		  case TAT_BUSY_CHANNEL:
		  	   task_status = TAT_TRX_BUSY;
			   break;
		  case TAT_TRANSIMITION_NO_ACK:
		  	   task_status = TAT_NO_ACK;
			   retry = false;
			   break;
        }
		
   }while (retry == true);
  return task_status;
}

tat_status_t rf2xx_tat_send_basic_with_retry( uint8_t frame_length, uint8_t *frame, uint8_t retries )
{
	uint8_t transaction_status;
	uint32_t cpu_sr = cpu_sr;

    tat_status_t task_status = TAT_CHANNEL_ACCESS_FAILURE;


    if ((frame_length > RF2XX_MAX_TX_FRAME_LENGTH) ||
        (frame_length < TAT_MIN_IEEE_FRAME_LENGTH)) 
    {
        return TAT_INVALID_ARGUMENT;
    }
	
	ENTER_CRITICAL_REGION();
    rf2xx_hal_frame_write( frame, frame_length ); 
	LEAVE_CRITICAL_REGION();
	RF_SLP_HIGH();
	RF_SLP_LOW();

//	rf2xx_hal_sram_read(0, 0x1f, rf_ram);
	rf2xx_clear_trx_end_flag();
    while (!rf2xx_get_trx_end_flag());
	rf2xx_clear_trx_end_flag();
    rf2xx_hal_subregister_read( SR_TRAC_STATUS, &transaction_status);

    switch(transaction_status)
    {
	   case TAT_SUCCESS_DATA_PENDING:
       case TAT_TRANSMISSION_SUCCESS:
	   	    task_status = TAT_SUCCESS;
			break;
	   case TAT_BUSY_CHANNEL:
	        task_status = TAT_CHANNEL_ACCESS_FAILURE;
			break;

       case TAT_TRANSIMITION_NO_ACK: 
	   	    task_status = TAT_NO_ACK;
			break;
       case TAT_TRANSMISSION_INVALID: 
	        task_status = TAT_SUCCESS;
			break;	    
   }
   
   return task_status;
}

uint8_t rf_ram[128];
uint8_t rf2xx_send_timeout_flag = 0;

tat_status_t rf2xx_tat_send_packet( uint8_t frame_length, uint8_t *frame, uint16_t timeout)
{
    tat_status_t task_status = TAT_SUCCESS;
	uint8_t transaction_status;

    rf2xx_hal_frame_write( frame, frame_length ); 
	RF_SLP_HIGH();
	RF_SLP_LOW();
//	rf2xx_hal_sram_read(0, 0x6b, rf_ram);
	rf2xx_clear_trx_end_flag();
//	timeout_add_item(timeout, &rf2xx_send_timeout_flag, TIMEOUT_MS);
    while ((!rf2xx_get_trx_end_flag()));
	
	rf2xx_clear_trx_end_flag();
    rf2xx_hal_subregister_read( SR_TRAC_STATUS, &transaction_status);

    switch(transaction_status)
    {
	   case TAT_SUCCESS_DATA_PENDING:
       case TAT_TRANSMISSION_SUCCESS:
	   	    task_status = TAT_SUCCESS;
			break;
	   case TAT_BUSY_CHANNEL:
	        task_status = TAT_CHANNEL_ACCESS_FAILURE;
			break;

       case TAT_TRANSIMITION_NO_ACK: 
	   	    task_status = TAT_NO_ACK;
			break;
       case TAT_TRANSMISSION_INVALID: 
	        task_status = TAT_SUCCESS;
			break;	    
   }
  
   return task_status;
}

tat_status_t rf2xx_tat_get_transaction_state(void)
{
	tat_status_t task_status;
	uint8_t transaction_status;
    rf2xx_hal_subregister_read( SR_TRAC_STATUS, &transaction_status);

    switch(transaction_status)
    {
	   case TAT_SUCCESS_DATA_PENDING:
       case TAT_TRANSMISSION_SUCCESS:
	   	    task_status = TAT_SUCCESS;
			break;
	   case TAT_BUSY_CHANNEL:
	        task_status = TAT_CHANNEL_ACCESS_FAILURE;
			break;

       case TAT_TRANSIMITION_NO_ACK: 
	   	    task_status = TAT_NO_ACK;
			break;
       case TAT_TRANSMISSION_INVALID: 
	        task_status = TAT_SUCCESS;
			break;	    
   }
 
   return task_status;
}

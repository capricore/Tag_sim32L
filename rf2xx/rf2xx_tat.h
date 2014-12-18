#ifndef RF2XX_TAT_H_
#define RF2XX_TAT_H_


#define TAT_STATUS_START_VALUE                  ( 0x40 )

typedef enum{
    //!< The requested service was performed successfully.
    TAT_SUCCESS = TAT_STATUS_START_VALUE,
    //!< The connected device is not an Atmel AT86RF230.
    TAT_UNSUPPORTED_DEVICE,
    //!< One or more of the supplied function arguments are invalid.
    TAT_INVALID_ARGUMENT,
    //!< The requested service timed out.
    TAT_TIMED_OUT,
    //!< The end-user tried to do an invalid state transition.
    TAT_WRONG_STATE,
    //!< The radio transceiver is busy receiving or transmitting.
    TAT_BUSY_STATE,
    //!< The requested state transition could not be completed.
    TAT_STATE_TRANSITION_FAILED,
    //!< Channel in idle. Ready to transmit a new frame.
    TAT_CCA_IDLE,
    //!< Channel busy.
    TAT_CCA_BUSY,
    //!< Transceiver is busy receiving or transmitting data.
    TAT_TRX_BUSY,
    //!< Measured battery voltage is lower than voltage threshold.
    TAT_BAT_LOW,
    //!< Measured battery voltage is above the voltage threshold.
    TAT_BAT_OK,
    //!< The CRC failed for the actual frame.
    TAT_CRC_FAILED,
    //!< The channel access failed during the auto mode.
    TAT_CHANNEL_ACCESS_FAILURE,
    //!< No acknowledge frame was received.
    TAT_NO_ACK,
	//! NO RF connect.
	TAT_NO_DEVICE,
}tat_status_t;

typedef enum{
    //!< Use energy detection above threshold mode.
    CCA_ED                    = 0,
    //!< Use carrier sense mode.
    CCA_CARRIER_SENSE         = 1,
    //!< Use a combination of both energy detection and carrier sense.
    CCA_CARRIER_SENSE_WITH_ED = 2
}tat_cca_mode_t;

typedef enum{

    CLKM_DISABLED      = 0,
    CLKM_1MHZ          = 1,
    CLKM_2MHZ          = 2,
    CLKM_4MHZ          = 3,
    CLKM_8MHZ          = 4,
    CLKM_16MHZ         = 5    
}tat_clkm_speed_t;


#define RF2XX_MIN_ED_THRESHOLD                  ( 0 )
#define RF2XX_MAX_ED_THRESHOLD                  ( 15 )
#define RF2XX_MAX_TX_FRAME_LENGTH               ( 127 ) //!< 127 Byte PSDU.

#define TAT_CCA_DONE_MASK     ( 1 << 7 ) //!< Mask used to check the CCA_DONE bit.
#define TAT_CCA_IDLE_MASK     ( 1 << 6 ) //!< Mask used to check the CCA_STATUS bit.

#define TAT_START_CCA ( 1 ) //!< Value in the CCA_REQUEST subregister that initiate a cca.

#define BATTERY_MONITOR_HIGHEST_VOLTAGE         ( 15 )
#define BATTERY_MONITOR_VOLTAGE_UNDER_THRESHOLD ( 0 )
#define BATTERY_MONITOR_HIGH_VOLTAGE            ( 1 )
#define BATTERY_MONITOR_LOW_VOLTAGE             ( 0 )

#define TAT_TRANSMISSION_SUCCESS  ( 0 )
#define TAT_SUCCESS_DATA_PENDING  ( 1 )
#define TAT_BUSY_CHANNEL          ( 3 )
#define TAT_TRANSIMITION_NO_ACK   ( 5 )
#define TAT_TRANSMISSION_INVALID  ( 7 )


#define TAT_MIN_IEEE_FRAME_LENGTH ( 5 )

#define FTN_CALIBRATION_DONE                    ( 0 )


tat_status_t rf2xx_tat_init( void );

uint8_t rf2xx_tat_get_trx_state( void );

tat_status_t rf2xx_tat_set_trx_state( uint8_t new_state );

int rf2xx_tat_get_operating_channel( void );

tat_status_t rf2xx_tat_set_operating_channel( void );

uint8_t rf2xx_tat_get_tx_power_level( void );

tat_status_t rf2xx_tat_set_tx_power_level(void);

tat_status_t rf2xx_tat_set_modulation_datarate(uint8_t mode) ;

uint8_t rf2xx_tat_get_cca_mode( void );

tat_status_t rf2xx_tat_set_cca_mode( uint8_t mode, uint8_t ed_threshold );

tat_status_t rf2xx_tat_do_cca( void );

uint8_t rf2xx_tat_batmon_get_voltage_threshold( void );

uint8_t rf2xx_tat_batmon_get_voltage_range( void );

tat_status_t rf2xx_tat_batmon_configure( bool range, uint8_t voltage_threshold );

tat_status_t rf2xx_tat_batmon_get_status( void );

uint8_t rf2xx_tat_get_clock_speed( void );

tat_status_t rf2xx_tat_set_clock_speed( bool direct, uint8_t clock_speed );

tat_status_t rf2xx_tat_calibrate_filter( void );

tat_status_t rf2xx_tat_calibrate_pll( void );

tat_status_t rf2xx_tat_enter_sleep_mode( void )	;

tat_status_t rf2xx_tat_leave_sleep_mode( void );

void rf2xx_tat_use_auto_tx_crc( bool auto_crc_on );

uint8_t rf2xx_tat_get_device_role( void );

void rf2xx_tat_set_device_role( bool i_am_coordinator );

void rf2xx_tat_set_irq_mask(void);

uint8_t rf2xx_tat_get_irq_mask(void);

uint8_t rf2xx_tat_get_irq_status(void);

void rf2xx_tat_set_enhanced_feature(void);

void rf2xx_tat_set_bitrate(void);

uint16_t rf2xx_tat_get_pan_id( void );

void rf2xx_tat_set_pan_id( uint16_t new_pan_id );

uint16_t rf2xx_tat_get_short_address( void );

void rf2xx_tat_set_short_address( uint16_t new_short_address );

uint8_t rf2xx_tat_get_ed_value(void);

tat_status_t rf2xx_tat_configure_csma( uint8_t min_be, uint8_t frame_retry,uint8_t csma_retry,uint16_t csma_seed);

tat_status_t rf2xx_tat_send_basic_with_retry( uint8_t frame_length, uint8_t *frame, uint8_t retries );

tat_status_t rf2xx_tat_send_data_with_retry( uint8_t frame_length, uint8_t *frame, uint8_t retries );

tat_status_t rf2xx_tat_send_packet( uint8_t frame_length, uint8_t *frame, uint16_t timeout);

tat_status_t rf2xx_tat_get_transaction_state(void);

#endif



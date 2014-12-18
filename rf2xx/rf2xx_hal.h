#ifndef RF2XX_HAL_H_
#define RF2XX_HAL_H_

#define HAL_DUMMY_READ         ( 0x00 ) //!< Dummy value for the SPI.

#define HAL_TRX_CMD_RW         ( 0xC0 ) //!< Register Write (short mode).
#define HAL_TRX_CMD_RR         ( 0x80 ) //!< Register Read (short mode).
#define HAL_TRX_CMD_FW         ( 0x60 ) //!< Frame Transmit Mode (long mode).
#define HAL_TRX_CMD_FR         ( 0x20 ) //!< Frame Receive Mode (long mode).
#define HAL_TRX_CMD_SW         ( 0x40 ) //!< SRAM Write.
#define HAL_TRX_CMD_SR         ( 0x00 ) //!< SRAM Read.
#define HAL_TRX_CMD_RADDRM     ( 0x7F ) //!< Register Address Mask.

#define HAL_CALCULATED_CRC_OK   ( 0 ) //!< CRC calculated over the frame including the CRC field should be 0.

#define HAL_BAT_LOW_MASK       ( 0x80 ) //!< Mask for the BAT_LOW interrupt.
#define HAL_TRX_UR_MASK        ( 0x40 ) //!< Mask for the TRX_UR interrupt.
#define HAL_TRX_END_MASK       ( 0x08 ) //!< Mask for the TRX_END interrupt.
#define HAL_RX_START_MASK      ( 0x04 ) //!< Mask for the RX_START interrupt.
#define HAL_PLL_UNLOCK_MASK    ( 0x02 ) //!< Mask for the PLL_UNLOCK interrupt.
#define HAL_PLL_LOCK_MASK      ( 0x01 ) //!< Mask for the PLL_LOCK interrupt.
#define HAL_MIN_FRAME_LENGTH   ( 0x03 ) //!< A frame should be at least 3 bytes.
#define HAL_MAX_FRAME_LENGTH   ( 0x7F ) //!< A frame should no more than 127 bytes.

typedef struct{
    uint8_t length;
    uint8_t data[ HAL_MAX_FRAME_LENGTH ];
    uint8_t ed_value;
    bool crc;
} hal_rx_frame_t;

int16_t rf2xx_hal_register_read( uint8_t address, uint8_t *value);

int16_t rf2xx_hal_register_write( uint8_t address, uint8_t value );

int16_t rf2xx_hal_subregister_read( uint8_t address, uint8_t mask, uint8_t position, uint8_t *register_value);

int16_t rf2xx_hal_subregister_write( uint8_t address, uint8_t mask, uint8_t position, uint8_t value );

int16_t rf2xx_hal_frame_read( hal_rx_frame_t *rx_frame );

int16_t rf2xx_hal_frame_write( uint8_t *write_buffer, uint8_t length );

int16_t rf2xx_hal_frame_write( uint8_t *write_buffer, uint8_t length );

int16_t rf2xx_hal_sram_write( uint8_t address, uint8_t length, uint8_t *data );

int16_t rf2xx_hal_sram_read( uint8_t address, uint8_t length, uint8_t *data );

#endif


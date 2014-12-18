#include "rf2xx_include.h"

/*! \brief  This function reads data from one of the radio transceiver's registers.
 *
 *  \param  address Register address to read from. See datasheet for register
 *                  map.
 *
 *  \see Look at the at86rf230_registermap.h file for register address definitions.
 *
 *  \returns The actual value of the read register.
 *
 *  \ingroup hal_avr_api
 */
int16_t rf2xx_hal_register_read( uint8_t address, uint8_t *value)
{
	int16_t ret = 0;
	uint32_t cpu_sr = cpu_sr;
	
	address &= HAL_TRX_CMD_RADDRM;
	address |= HAL_TRX_CMD_RR;

	ENTER_CRITICAL_REGION( );

	ret = spi1_w8r8(address, value);
	
	LEAVE_CRITICAL_REGION( );

	return ret;
}


/*! \brief  This function writes a new value to one of the radio transceiver's
 *          registers.
 *
 *  \see Look at the at86rf230_registermap.h file for register address definitions.
 *
 *  \param  address Address of register to write.
 *  \param  value   Value to write.
 *
 *  \ingroup hal_avr_api
 */
int16_t rf2xx_hal_register_write( uint8_t address, uint8_t value )
{
    int16_t ret;
	uint32_t cpu_sr = cpu_sr;
	address = HAL_TRX_CMD_RW | (HAL_TRX_CMD_RADDRM & address);

	ENTER_CRITICAL_REGION();

	ret = spi1_w8w8(address, value);
	
	LEAVE_CRITICAL_REGION( );

	return ret;
}


/*! \brief  This function reads the value of a specific subregister.
 *
 *  \see Look at the at86rf230_registermap.h file for register and subregister
 *       definitions.
 *
 *  \param  address  Main register's address.
 *  \param  mask  Bit mask of the subregister.
 *  \param  position   Bit position of the subregister
 *  \retval Value of the read subregister.
 *
 *  \ingroup hal_avr_api
 */
int16_t rf2xx_hal_subregister_read( uint8_t address, uint8_t mask, uint8_t position, uint8_t *register_value)
{
	int16_t register_value_ret;
	register_value_ret = rf2xx_hal_register_read( address, register_value );

	if(register_value_ret < 0)return register_value_ret;
			
	*register_value &= mask;
	*register_value >>= position; 

	return register_value_ret;
}


/*! \brief  This function writes a new value to one of the radio transceiver's
 *          subregisters.
 *
 *  \see Look at the at86rf230_registermap.h file for register and subregister
 *       definitions.
 *
 *  \param  address  Main register's address.
 *  \param  mask  Bit mask of the subregister.
 *  \param  position  Bit position of the subregister
 *  \param  value  Value to write into the subregister.
 *
 *  \ingroup hal_avr_api
 */
int16_t rf2xx_hal_subregister_write( uint8_t address, uint8_t mask, uint8_t position, uint8_t value )
{
	uint8_t register_value;
	int16_t register_value_ret;
	register_value_ret = rf2xx_hal_register_read( address, &register_value);

	if(  register_value_ret < 0 )return register_value_ret;
	
	register_value &= ~mask;

	value <<= position;
	value &= mask;

	value |= register_value;

	register_value_ret = rf2xx_hal_register_write( address, value );

	return register_value_ret;
}


/*! \brief  This function will upload a frame from the radio transceiver's frame
 *          buffer.
 *
 *          If the frame currently available in the radio transceiver's frame buffer
 *          is out of the defined bounds. Then the frame length, lqi value and crc
 *          be set to zero. This is done to indicate an error.
 *
 *  \param  rx_frame    Pointer to the data structure where the frame is stored.
 *
 *  \ingroup hal_avr_api
 */
int16_t rf2xx_hal_frame_read( hal_rx_frame_t *rx_frame )
{
	int16_t ret = 0;
	uint32_t cpu_sr = cpu_sr;
	uint8_t frame_length;
	ENTER_CRITICAL_REGION( );

	spi1_read_buff(HAL_TRX_CMD_FR, rx_frame->data, &frame_length, 1);

	if ((frame_length >= HAL_MIN_FRAME_LENGTH) && (frame_length <= HAL_MAX_FRAME_LENGTH))
	{
		uint8_t i = 0;
		uint16_t crc = 0;
		for(; i<frame_length; i++){
			crc = crc_ccitt_update( crc, (rx_frame->data)[i]);
		}
		rx_frame->length = frame_length;
		rx_frame->ed_value = (rx_frame->data)[frame_length];
		if (crc == HAL_CALCULATED_CRC_OK)
		{
			rx_frame->crc = true;
			ret = 0;
		}
		else
		{
			rx_frame->crc = false;
			ret = -1;
		}		
	}
	else
	{
		rx_frame->length = 0;
		rx_frame->ed_value    = 0;
		rx_frame->crc    = 0;
		ret = -1;
	}
	LEAVE_CRITICAL_REGION( );

	return ret;
}


/*! \brief  This function will download a frame to the radio transceiver's frame
 *          buffer.
 *
 *  \param  write_buffer    Pointer to data that is to be written to frame buffer.
 *  \param  length          Length of data. The maximum length is 127 bytes.
 *
 *  \ingroup hal_avr_api
 */
int16_t rf2xx_hal_frame_write( uint8_t *write_buffer, uint8_t length )
{
	int16_t ret;
	uint32_t cpu_sr = cpu_sr;
	length &= HAL_TRX_CMD_RADDRM;

	ENTER_CRITICAL_REGION( );

	ret = spi1_write_buff(HAL_TRX_CMD_FW, write_buffer, length);

	LEAVE_CRITICAL_REGION( );

	return ret;
}


/*! \brief Read SRAM
 *
 * This function reads from the SRAM of the radio transceiver.
 *
 * \param address Address in the TRX's SRAM where the read burst should start
 * \param length Length of the read burst
 * \param data Pointer to buffer where data is stored.
 *
 * \ingroup hal_avr_api
 */
int16_t rf2xx_hal_sram_read( uint8_t address, uint8_t length, uint8_t *data )
{
	int16_t ret = 0;
	uint32_t cpu_sr = cpu_sr;
	ENTER_CRITICAL_REGION( );

	ret = spi1_read_buff_with_addr(HAL_TRX_CMD_SR, address, data, length);
	
	LEAVE_CRITICAL_REGION( );

	return ret;
}


/*! \brief Write SRAM
 *
 * This function writes into the SRAM of the radio transceiver.
 *
 * \param address Address in the TRX's SRAM where the write burst should start
 * \param length  Length of the write burst
 * \param data    Pointer to an array of bytes that should be written
 *
 * \ingroup hal_avr_api
 */
int16_t rf2xx_hal_sram_write( uint8_t address, uint8_t length, uint8_t *data )
{
	int16_t ret;
	uint32_t cpu_sr = cpu_sr;
	ENTER_CRITICAL_REGION( );

	ret = spi1_write_buff_with_addr(HAL_TRX_CMD_SW, address, data, length);

	LEAVE_CRITICAL_REGION( );

	return ret;
}

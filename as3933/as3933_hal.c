#include "as3933_include.h"

uint8_t as3933_hal_register_read(uint8_t address)
{
 	uint8_t value;

 	AS3933_SPI_CS_ENABLE();
	
	SPI0_ReadWrite(0x40 | address);
	
	value = SPI0_ReadWrite(address);

 	AS3933_SPI_CS_DISABLE();
	
	return value;
		
}

uint8_t as3933_hal_register_write(uint8_t address, uint8_t value)
{
 	AS3933_SPI_CS_ENABLE();
	
	SPI0_ReadWrite(0x00 | address);
	
	SPI0_ReadWrite(value);

 	AS3933_SPI_CS_DISABLE();
	
	return 0;
}

void spi0_as_register_direct_command(uint8_t command)
{
	AS3933_SPI_CS_ENABLE();
	
	SPI0_ReadWrite(0xC0 | command);
	
	SPI0_ReadWrite(command);

 	AS3933_SPI_CS_DISABLE();
}


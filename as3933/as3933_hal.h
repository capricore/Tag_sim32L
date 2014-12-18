#ifndef AS3933_HAL_H_
#define AS3933_HAL_H_

uint8_t as3933_hal_register_read(uint8_t address);

uint8_t as3933_hal_register_write(uint8_t address, uint8_t value);

void spi0_as_register_direct_command(uint8_t command);

#endif

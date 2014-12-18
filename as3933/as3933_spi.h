#ifndef AS3933_SPI_H_
#define AS3933_SPI_H_
//#include <SI32_PBSTD_A_Type.h>

// #define AS3933_SPI_CS_OUT()		SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x0008)
// #define AS3933_SPI_CS_ENABLE()	(SI32_PBSTD_A_write_pins_high(SI32_PBSTD_3, 0x0008))
// #define AS3933_SPI_CS_DISABLE()		(SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x0008))

#define AS3933_SPI_CS_OUT()		SI32_PBSTD_A_set_pins_push_pull_output(SI32_PBSTD_3, 0x0020)
#define AS3933_SPI_CS_ENABLE()	(SI32_PBSTD_A_write_pins_high(SI32_PBSTD_3, 0x0020))
#define AS3933_SPI_CS_DISABLE()		(SI32_PBSTD_A_write_pins_low(SI32_PBSTD_3, 0x0020))
void as3933_spi_port_init(void);

void as3933_spi_init(void);

void as3933_spi_port_off(void);

uint8_t SPI0_ReadWrite(uint8_t data);

#endif


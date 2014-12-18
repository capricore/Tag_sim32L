#ifndef AS3933_H_
#define AS3933_H_

#define  as3933_pindat_read_clk(void)	(SI32_PBSTD_A_read_pin(SI32_PBSTD_3,4))
#define as3933_pindat_read_dat(void)	(SI32_PBSTD_A_read_pin(SI32_PBSTD_3,3))

#define AS3933_PINDAT_CLK_IN()	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_3, 0x0010)
#define AS3933_PINDAT_DAT_IN()	SI32_PBSTD_A_set_pins_digital_input(SI32_PBSTD_3, 0x0008)

void as3933_port_init(void);

void as3933_init(void);

int as3933_data_read(void) ;

uint16_t crc_compute(uint8_t *buf, uint16_t size,uint16_t initialValue,uint8_t isLast);

void as3933_wake_int_init(void);
#endif


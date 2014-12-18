#ifndef CRC_H_
#define CRC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define CRC_POLY (0x1021)
#define INITIAL_VALUE ((uint16_t)0xFFFF)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned char byte;

uint16_t crc_initial(void);
uint16_t crc_step(uint16_t crc, byte b);
uint16_t crc_finish(uint16_t crc);
uint16_t crc_calculate(uint16_t prev_crc, uint8_t *bytes, uint16_t len);

uint16_t crc_ccitt_update(uint16_t crc, uint8_t data);
#ifdef __cplusplus
 }
#endif

#endif // __MYCRC_H__

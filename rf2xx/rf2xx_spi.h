#ifndef RF2XX_SPI_H
#define RF2XX_SPI_H

int spi1_read_buff(const uint8_t cmd, uint8_t *buf, uint8_t *len, int8_t offset);

int spi1_read_buff_with_addr(const uint8_t cmd, const uint8_t addr, uint8_t *buf, uint8_t len);

int spi1_w8r8(uint8_t cmd, uint8_t *rx);

int spi1_w8w8(uint8_t cmd, uint8_t tx);

int spi1_write_buff(const uint8_t cmd, const uint8_t *buf, uint8_t len);

int spi1_write_buff_with_addr(const uint8_t cmd, uint8_t addr, const uint8_t *buf, uint8_t len);

#endif


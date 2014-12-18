#include <SI32_SPI_B_Type.h>
#include <si32_device.h>

int spi1_read_buff(const uint8_t cmd, uint8_t *buf, uint8_t *len, int8_t offset)
{
	int ret = 0;
	int16_t len_16,offset_16;
	int16_t length;
	uint8_t i = 0;	
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
	//	SI32_SPI_B_select_nss_data_count(SI32_SPI, len);
	SI32_SPI_B_select_transfer_count_one(SI32_SPI);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI);
	SI32_SPI_B_clear_nss (SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, cmd);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, 0);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	*len = SI32_SPI_B_read_rx_fifo_u8(SI32_SPI);
	len_16 = *len;
	offset_16 = offset;
	length = len_16 + offset_16;
	
	if(length < 0 || length > 127)
		ret = -1;
	else{
		while(length--){
			SI32_SPI_B_flush_rx_fifo(SI32_SPI);
			SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, 0);
			while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
			while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
			buf[i++] = SI32_SPI_B_read_rx_fifo_u8(SI32_SPI);
		}
	}
		
	SI32_SPI_B_set_nss (SI32_SPI);
	
	return ret;	
}

int spi1_read_buff_with_addr(const uint8_t cmd, const uint8_t addr, uint8_t *buf, uint8_t len)
{
	int ret = 0;
	uint8_t i = 0;
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
	
//	SI32_SPI_B_select_nss_data_count(SI32_SPI, len+1);
	SI32_SPI_B_select_transfer_count_one(SI32_SPI);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI);	
	SI32_SPI_B_clear_nss (SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, cmd);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));	
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, addr);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));	
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);			
	while(len--)
	{	
		SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, 0);
		while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
		while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
		buf[i++] = SI32_SPI_B_read_rx_fifo_u8(SI32_SPI);
		SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	}
	SI32_SPI_B_set_nss (SI32_SPI);
	return ret;	
}


int spi1_w8w8(uint8_t cmd, uint8_t tx)
{
	int ret = 0;
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	SI32_SPI_B_select_nss_data_count(SI32_SPI, 1);
	SI32_SPI_B_select_transfer_count_one(SI32_SPI);	
	
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI);	
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_clear_nss (SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, cmd);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, tx);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_set_nss (SI32_SPI);
	return ret;
}


int spi1_w8r8(uint8_t cmd, uint8_t *rx)
{
	int ret = 0;
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
//	SI32_SPI_B_select_nss_data_count(SI32_SPI, 1);
	SI32_SPI_B_select_transfer_count_one(SI32_SPI);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI);	
	
	SI32_SPI_B_clear_nss (SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, cmd);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, 0);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	*rx = SI32_SPI_B_read_rx_fifo_u8(SI32_SPI);
	SI32_SPI_B_set_nss (SI32_SPI);
	return ret;

}


int spi1_write_buff(const uint8_t cmd, const uint8_t *buf, uint8_t len)
{
	int ret = 0;
	uint8_t i = 0;
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
	
//	SI32_SPI_B_select_nss_data_count(SI32_SPI, 0);
	
	SI32_SPI_B_select_transfer_count_one(SI32_SPI); 		
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI); 
	SI32_SPI_B_clear_nss (SI32_SPI);	
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, cmd);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, len);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	while(len--){
		SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, buf[i]);
		while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
		while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
		SI32_SPI_B_flush_rx_fifo(SI32_SPI);
		i++;
	}
	SI32_SPI_B_set_nss (SI32_SPI);

	return ret;
}


int spi1_write_buff_with_addr(const uint8_t cmd, uint8_t addr, const uint8_t *buf, uint8_t len)
{
	int ret = 0;
	uint8_t i = 0;
	SI32_SPI_B_Type* SI32_SPI = SI32_SPI_1;
	SI32_SPI_B_disable_auto_nss_mode(SI32_SPI);
	SI32_SPI_B_select_4wire_master_mode_nss_high(SI32_SPI);
	
//	SI32_SPI_B_select_nss_data_count(SI32_SPI, len+1);
	SI32_SPI_B_select_transfer_count_one(SI32_SPI);
	
	SI32_SPI_B_flush_rx_fifo(SI32_SPI);
	SI32_SPI_B_flush_tx_fifo(SI32_SPI);
	SI32_SPI_B_clear_nss (SI32_SPI);	
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, cmd);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, addr);
	while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
	while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
	while(len--){
		SI32_SPI_B_write_tx_fifo_u8(SI32_SPI, buf[i]);
		while(SI32_SPI_B_is_shift_register_empty_interrupt_pending(SI32_SPI));
		while(SI32_SPI_B_get_rx_fifo_count(SI32_SPI)<1);
		i++;
	}
	SI32_SPI_B_set_nss (SI32_SPI);
	return ret;
}


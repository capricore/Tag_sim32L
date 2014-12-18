#ifndef RF2XX_OPTION_H_
#define RF2XX_OPTION_H_

 inline void rf2xx_clear_trx_end_flag(void);

 inline void rf2xx_set_trx_end_flag(void);
 
 inline uint8_t rf2xx_get_trx_end_flag(void);

 __INLINE void rf_tx_on(void);

__INLINE void rf_rx_on(void);

__INLINE uint8_t rf_if_rx(void);

#endif

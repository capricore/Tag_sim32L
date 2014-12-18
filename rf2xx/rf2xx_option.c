#include "rf2xx_include.h"

uint8_t rf2xx_trx_end_flag;
uint8_t rx_flag;

inline void rf2xx_clear_trx_end_flag(void)
{
	rf2xx_trx_end_flag = 0;
}

inline void rf2xx_set_trx_end_flag(void)
{
 	rf2xx_trx_end_flag = 1;
}

inline uint8_t rf2xx_get_trx_end_flag(void)
{
	return rf2xx_trx_end_flag;
}

__INLINE void rf_tx_on(void)
{
	rx_flag = 0;
}
__INLINE void rf_rx_on(void)
{
	rx_flag = 1;
}
__INLINE uint8_t rf_if_rx(void)
{
	return rx_flag;
}


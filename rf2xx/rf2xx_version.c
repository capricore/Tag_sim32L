#include "rf2xx_include.h"

#define TRX_END_TIME_BASIC 8
#define TRX_END_TIME_BASIC_BASE 2

#define TRX_END_TIME_EXTEND 8
#define TRX_END_TIME_EXTEND_BASE	30
#define PA_BOOST_ON 	1
#define PA_BOOST_OFF 	0
uint8_t rf2xx_version;

uint32_t time_cca = TIME_CCA;
uint32_t trx_end_time_basic =  TRX_END_TIME_BASIC;
uint32_t trx_end_time_extend = TRX_END_TIME_EXTEND+TRX_END_TIME_EXTEND_BASE;

uint16_t panid = 0x0001;
uint16_t id = 0x1234;
uint8_t channel_2_4G = 23;
//uint8_t channel_780M = 110;

uint8_t rf231_bitrate = RF231_ALTRATE_250KBPS;
uint8_t rf212_bitrate = BPSK_20;
uint8_t rf23x_txpower = TX_PWR_3DBM;
uint8_t rf212_txpower = WPAN_TXPWR_P10DBM;
uint8_t rf212_pa_boost = PA_BOOST_ON;

//float rf212_band_input;
int rf212_band_set = 7810;
uint8_t rf212_cc_number;
uint8_t rf212_cc_band;
uint8_t rf212_channel;


int set_band_channel(void)
{
	int set_flag = 0;
	int band_temp;

	switch(rf212_band_set)
	{
		case RF212_BAND_EU_868_3:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 0;
			set_flag = 1;
			break;		
		case RF212_BAND_NA_906:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 1;			
			set_flag = 1;
			break;		
		case RF212_BAND_NA_908:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 2;			
			set_flag = 1;
			break;	
		case RF212_BAND_NA_910:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 3;
			set_flag = 1;
			break;	
		case RF212_BAND_NA_912:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 4;			
			set_flag = 1;
			break;	
		case RF212_BAND_NA_914:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 5;			
			set_flag = 1;
			break;	
		case RF212_BAND_NA_916:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 6;
			set_flag = 1;
			break;	
		case RF212_BAND_NA_918:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 7;			
			set_flag = 1;
			break;	
		case RF212_BAND_NA_920:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 8;			
			set_flag = 1;
			break;				
		case RF212_BAND_NA_922:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 9;
			set_flag = 1;
			break;	
		case RF212_BAND_NA_924:
			rf212_cc_band = 0;
			rf212_cc_number = 0;
			rf212_channel = 10;			
			set_flag = 1;
			break;				
	}
	if(set_flag)
	{
		return 0;
	}

	band_temp = rf212_band_set/10;
	band_temp *= 10;
	if(band_temp == rf212_band_set)
	{
		if((rf212_band_set < RF212_BAND_BASE_4)&&(rf212_band_set > RF212_BAND_END_5))
			return -1;
		else
		{
			if(rf212_band_set < RF212_BAND_END_4)
			{
				rf212_cc_band = 4;
				rf212_cc_number = (rf212_band_set - RF212_BAND_BASE_4)/10;
				rf212_channel = 0;				
			}
			else 
			{
				rf212_cc_band = 5;
				rf212_cc_number = (rf212_band_set - RF212_BAND_BASE_5)/10;
				rf212_channel = 0;
			}
		}
	}
	else
	{
		if(rf212_band_set >= RF212_BAND_BASE_1 && rf212_band_set<= RF212_BAND_END_1)
		{
			rf212_cc_band = 1;
			rf212_cc_number = (rf212_band_set - RF212_BAND_BASE_1);
			rf212_channel = 0;
		}
		else if((rf212_band_set >= RF212_BAND_BASE_2 && rf212_band_set<= RF212_BAND_END_2))
		{
			rf212_cc_band = 2;
			rf212_cc_number = (rf212_band_set - RF212_BAND_BASE_2);
			rf212_channel = 0;		
		}
		else if((rf212_band_set >= RF212_BAND_BASE_3 && rf212_band_set<= RF212_BAND_END_3))
		{
			rf212_cc_band = 3;
			rf212_cc_number = (rf212_band_set - RF212_BAND_BASE_3);
			rf212_channel = 0;		
		}	
		else
		{
			return -1;
		}

	}

	return 0;
}

int get_band_channel(uint8_t band, uint8_t channel, uint8_t cc_number)
{
	int band_channel;
	
	switch(band)
	{
		case 0:
			if(!channel)
				band_channel = RF212_BAND_EU_868_3;
			else if(channel > 0x0a)
				band_channel = 0;
			else
				band_channel = RF212_BAND_NA_906 + (channel-1)*20;
			break;
		case 1:
			band_channel = RF212_BAND_BASE_1 + cc_number;
			break;
		case 2:
			band_channel = RF212_BAND_BASE_2 + cc_number;
			break;
		case 3:
			band_channel = RF212_BAND_BASE_3 + cc_number;
			break;
		case 4:
			band_channel = RF212_BAND_BASE_4 + cc_number*10;
			break;
		case 5:
			band_channel = RF212_BAND_BASE_5 + cc_number*10;
			break;
		default:
			band_channel = 0;
			break;
	}

	return band_channel;
}

uint32_t get_transaction_time(uint8_t len)
{
	uint32_t trans_time;
	if(len == 9)
	{
		switch(rf212_bitrate)
		{
			case BPSK_20:
				trans_time = 276;
				break;
			case BPSK_40:
				trans_time = 138;
				break;
			case OQPSK_SIN_250:
				trans_time = 23;
				break;
			default:
				while(1);
//				break;
		}
	}
	else
	{
		trans_time = 0;
	}
	return trans_time;
}

void calc_time_cca(void)
{
	if(RF_IS_230)
		time_cca = TIME_CCA;
	else if(RF_IS_231)
	{
		time_cca = TIME_CCA>>rf231_bitrate;
	}
	else if(RF_IS_212)
	{
		switch(rf212_bitrate)
		{
			case BPSK_20:
				time_cca = (TIME_CCA<<4);
				break;
			case BPSK_40:
				time_cca = (TIME_CCA<<3);
				break;
			case OQPSK_SIN_RC_100:
				time_cca = (TIME_CCA << 2);
				break;
			case OQPSK_SIN_250:
				time_cca = TIME_CCA;
				break;
			default:
				time_cca = (TIME_CCA <<4 );
				break;
		}

	}
}

void calc_trx_end_time_basic(void)
{
	if(RF_IS_230)
		trx_end_time_basic = TRX_END_TIME_BASIC;
	else if(RF_IS_231)
	{
		trx_end_time_basic = (TRX_END_TIME_BASIC>>rf231_bitrate)+TRX_END_TIME_BASIC_BASE;
	}
	else if(RF_IS_212)
	{
		switch(rf212_bitrate)
		{
			case BPSK_20:
				trx_end_time_basic = (TRX_END_TIME_BASIC<<3);
				break;
			case BPSK_40:
				trx_end_time_basic = (TRX_END_TIME_BASIC<<2);
				break;
			case OQPSK_SIN_RC_100:
				trx_end_time_basic = (TRX_END_TIME_BASIC << 1);
				break;
			case OQPSK_SIN_250:
				trx_end_time_basic = TRX_END_TIME_BASIC;
				break;
			default:
				trx_end_time_basic = (TRX_END_TIME_BASIC <<3 );
				break;
		}

	}	

}

void calc_trx_end_time_extend(void)
{
	if(RF_IS_230)
		trx_end_time_extend = TRX_END_TIME_EXTEND+TRX_END_TIME_EXTEND_BASE;
	else if(RF_IS_231)
	{
		trx_end_time_extend = (TRX_END_TIME_EXTEND>>rf231_bitrate)+TRX_END_TIME_EXTEND_BASE;
	}
	else if(RF_IS_212)
	{
		switch(rf212_bitrate)
		{
			case BPSK_20:
				trx_end_time_extend = (TRX_END_TIME_EXTEND<<3)+TRX_END_TIME_EXTEND_BASE;
				break;
			case BPSK_40:
				trx_end_time_extend = (TRX_END_TIME_EXTEND<<2)+TRX_END_TIME_EXTEND_BASE;
				break;
			case OQPSK_SIN_RC_100:
				trx_end_time_extend = (TRX_END_TIME_EXTEND << 1)+TRX_END_TIME_EXTEND_BASE;
				break;
			case OQPSK_SIN_250:
				trx_end_time_extend = TRX_END_TIME_EXTEND+TRX_END_TIME_EXTEND_BASE;
				break;
			default:
				trx_end_time_extend = (TRX_END_TIME_EXTEND <<3 )+TRX_END_TIME_EXTEND_BASE;
				break;
		}

	}	

}


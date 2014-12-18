#ifndef RF2XX_VERSION_H_
#define RF2XX_VERSION_H_

#define AT86_RF230 	1
#define AT86_RF231 	2
#define AT86_RF212	3

#define RF231_RF212_SUPPORTED_INTERRUPT_MASK          ( 0xFF )

#define RF230_SUPPORTED_INTERRUPT_MASK          ( 0xCF )

#define RF230_PART_NUM 		2	 
#define RF230_REV_A 		1	 
#define RF230_REV_B  		2
#define RF230_MAN_ID_0 	0x1F
#define RF230_MAN_ID_1 	0x00 

#define RF231_PART_NUM 		3	 
#define RF231_REV_A 		2	 
#define RF231_MAN_ID_0 	0x1F
#define RF231_MAN_ID_1 	0x00 

#define RF212_PART_NUM 		7	 
#define RF212_REV_A 		1	 
#define RF212_MAN_ID_0 	0x1F
#define RF212_MAN_ID_1 	0x00  

#define RF23X_MIN_CHANNEL                       ( 11 )
#define RF23X_MAX_CHANNEL                       ( 26 )

#define RF212_MIN_CHANNEL					7690
#define RF212_MAX_CHANNEL					9350

#define RF212_BAND_CHINA_780 	7800
#define RF212_BAND_CHINA_782 	7820
#define RF212_BAND_CHINA_784 	7840
#define RF212_BAND_CHINA_786 	7860

#define RF212_BAND_EU_868_3 	8683

#define RF212_BAND_NA_906		9060
#define RF212_BAND_NA_908		9080
#define RF212_BAND_NA_910		9100
#define RF212_BAND_NA_912		9120
#define RF212_BAND_NA_914		9140
#define RF212_BAND_NA_916		9160
#define RF212_BAND_NA_918		9180
#define RF212_BAND_NA_920		9200
#define RF212_BAND_NA_922		9220
#define RF212_BAND_NA_924		9240

#define RF212_BAND_BASE_1	7690
#define RF212_BAND_END_1	7945
#define RF212_BAND_BASE_2	8570
#define RF212_BAND_END_2	8825
#define RF212_BAND_BASE_3	9030
#define RF212_BAND_END_3	9285
#define RF212_BAND_BASE_4	7690
#define RF212_BAND_END_4	8630
#define RF212_BAND_BASE_5	8330
#define RF212_BAND_END_5	9350


typedef enum//R212 Modify
{
	RF212_CHANNEL_EU  = (113),	// 欧洲频段(868.3MHz)
	RF212_CHANNEL_NA0 = (30),	// 北美频段0~10 (906~924MHz)
	RF212_CHANNEL_NA1 = (50),
	RF212_CHANNEL_NA2 = (70),
	RF212_CHANNEL_NA3 = (90),
	RF212_CHANNEL_NA4 = (110),
	RF212_CHANNEL_NA5 = (130),
	RF212_CHANNEL_NA6 = (150),
	RF212_CHANNEL_NA7 = (170),
	RF212_CHANNEL_NA8 = (190),
	RF212_CHANNEL_NA9 = (210),
	RF212_CHANNEL_CN0 = (110),  	// 中国频段0~3 (780~786MHz)
	RF212_CHANNEL_CN1 = (130), 
	RF212_CHANNEL_CN2 = (150), 
	RF212_CHANNEL_CN3 = (170), 
	RF23x_CHANNEL_11 = (11),	// 2405MHz
 	RF23x_CHANNEL_12 = (12),	// 2410MHz
 	RF23x_CHANNEL_13 = (13),	// ...
	RF23x_CHANNEL_14 = (14),
	RF23x_CHANNEL_15 = (15),
	RF23x_CHANNEL_16 = (16),
	RF23x_CHANNEL_17 = (17),
	RF23x_CHANNEL_18 = (18),
	RF23x_CHANNEL_19 = (19),
	RF23x_CHANNEL_20 = (20),
	RF23x_CHANNEL_21 = (21),
	RF23x_CHANNEL_22 = (22),
	RF23x_CHANNEL_23 = (23),
	RF23x_CHANNEL_24 = (24),
	RF23x_CHANNEL_25 = (25),
	RF23x_CHANNEL_26 = (26)	// 2480MHz
}wpan_channel_t;


typedef enum {
	BPSK_20                   = 0x00,//IEEE 802.15.4
	BPSK_40                   = 0x04,//IEEE 802,15.4
	OQPSK_SIN_RC_100          = 0x08,//IEEE 802.15.4-2006
	OQPSK_SIN_RC_200          = 0x09,//特有
	OQPSK_SIN_RC_400_SCR_ON   = 0x2A,//特有，启用扰码器
	OQPSK_SIN_RC_400_SCR_OFF  = 0x0A,//特有，关闭扰码器	
	OQPSK_SIN_250             = 0x0C,//IEEE 802.15.4-2006
	OQPSK_SIN_500             = 0x0D,//特有
	OQPSK_SIN_1000_SCR_ON     = 0x2E,//特有，启用扰码器
	OQPSK_SIN_1000_SCR_OFF    = 0x0E,//特有，关闭扰码器	
	OQPSK_RC_250              = 0x1C,//IEEE 802.15.4c China
	OQPSK_RC_500              = 0x09,//特有
	OQPSK_RC_1000_SCR_ON      = 0x3E,//特有，启用扰码器
	OQPSK_RC_1000_SCR_OFF     = 0x1E //特有，关闭扰码器	
} rf212_modulation_datarate_t;

typedef enum {
	 WPAN_TXPWR_P10DBM = (0xC0),
	 WPAN_TXPWR_P5DBM =  (0xE7),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_P4DBM =  (0xE8),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_P3DBM =  (0xE9),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_P2DBM =  (0xEA),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_P1DBM =  (0xCA),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_P0DBM =  (0xAA),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N1DBM =  (0xAB),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N2DBM =  (0x45),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N3DBM =  (0x25),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N4DBM =  (0x03),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N5DBM =  (0x04),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N6DBM =  (0x05),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N7DBM =  (0x06),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N8DBM =  (0x07),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N9DBM =  (0x08),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N10DBM =  (0x09),  //PHY-TX-POWER VALUE
	 WPAN_TXPWR_N11DBM =  (0x0A)  //PHY-TX-POWER VALUE
} rf212_txpower_t;

typedef enum{
    TIME_TO_ENTER_P_ON               = 510, //!< Transition time from VCC is applied to P_ON.
    TIME_P_ON_TO_TRX_OFF             = 510, //!< Transition time from P_ON to TRX_OFF.
    TIME_SLEEP_TO_TRX_OFF            = 880, //!< Transition time from SLEEP to TRX_OFF.
    TIME_RESET                       = 6,   //!< Time to hold the RST pin low during reset
    TIME_ED_MEASUREMENT              = 140, //!< Time it takes to do a ED measurement.
    TIME_CCA                         = 140, //!< Time it takes to do a CCA.
    TIME_PLL_LOCK                    = 150, //!< Maximum time it should take for the PLL to lock.
    TIME_FTN_TUNING                  = 25,  //!< Maximum time it should take to do the filter tuning.
    TIME_NOCLK_TO_WAKE               = 6,   //!< Transition time from *_NOCLK to being awake.
    TIME_CMD_FORCE_TRX_OFF           = 1,    //!< Time it takes to execute the FORCE_TRX_OFF command.
    TIME_TRX_OFF_TO_PLL_ACTIVE       = 180, //!< Transition time from TRX_OFF to: RX_ON, PLL_ON, TX_ARET_ON and RX_AACK_ON.
    TIME_STATE_TRANSITION_PLL_ACTIVE = 1, //!< Transition time from PLL active state to another.
}tat_trx_timing_t;

#define BPSK_TX_OFFSET       		3
#define OQPSK_TX_OFFSET      		2

#define AACK_ACK_TIME_12_SYMBOLS 	0
#define AACK_ACK_TIME_2_SYMBOLS   	1

#define R212_PABOOST_ON   			1          
#define R212_PABOOST_OFF  			0     

#define TX_PWR_3DBM                             ( 0 )
#define TX_PWR_17_2DBM                          ( 15 ) 

#define RF_IS_230	(rf2xx_version == AT86_RF230)
#define RF_IS_231	(rf2xx_version == AT86_RF231)
#define RF_IS_212	(rf2xx_version == AT86_RF212)
#define RF_IS_23X	((rf2xx_version == AT86_RF230) || (rf2xx_version == AT86_RF231))
#define RF_IS_231_OR_212 ((rf2xx_version == AT86_RF212) || (rf2xx_version == AT86_RF231))

extern uint8_t rf2xx_version;
extern uint32_t time_cca;
extern uint32_t trx_end_time_basic;
extern uint32_t trx_end_time_extend;

extern uint8_t rf231_bitrate ;
extern uint8_t rf212_bitrate ;
extern uint8_t rf23x_txpower ;
extern uint8_t rf212_txpower ;
extern uint8_t rf212_pa_boost ;

extern uint16_t panid;
extern uint16_t id;
extern uint8_t channel_2_4G;
//extern uint8_t channel_780M;

extern int rf212_band_set;
extern uint8_t rf212_cc_number;
extern uint8_t rf212_cc_band;
extern uint8_t rf212_channel;

void calc_time_cca(void);

void calc_trx_end_time_basic(void);

void calc_trx_end_time_extend(void);

int set_band_channel(void);

int get_band_channel(uint8_t band, uint8_t channel, uint8_t cc_number);

uint32_t get_transaction_time(uint8_t len);

#endif


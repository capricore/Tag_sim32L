#if 0
#ifndef RF2XX_RF231_REGISTERMAP_H
#define RF2XX_RF231_REGISTERMAP_H

#define HAVE_REGISTER_MAP (1)

#define RG_TRX_STATUS                    (0x01)

#define SR_CCA_DONE                  0x01, 0x80, 7

#define SR_CCA_STATUS                0x01, 0x40, 6
#define SR_reserved_01_3             0x01, 0x20, 5

#define SR_TRX_STATUS                0x01, 0x1f, 0

#define P_ON                     (0)

#define BUSY_RX                  (1)

#define BUSY_TX                  (2)

#define RX_ON                    (6)

#define TRX_OFF                  (8)

#define PLL_ON                   (9)

#define SLEEP                    (15)

#define BUSY_RX_AACK             (17)

#define BUSY_TX_ARET             (18)

#define RX_AACK_ON               (22)

#define TX_ARET_ON               (25)

#define RX_ON_NOCLK              (28)

#define RX_AACK_ON_NOCLK         (29)

#define BUSY_RX_AACK_NOCLK       (30)

#define RG_TRX_STATE                     (0x02)

#define SR_TRAC_STATUS               0x02, 0xe0, 5

#define SR_TRX_CMD                   0x02, 0x1f, 0

#define CMD_NOP                  (0)

#define CMD_TX_START             (2)

#define CMD_FORCE_TRX_OFF        (3)

#define CMD_RX_ON                (6)

#define CMD_TRX_OFF              (8)

#define CMD_PLL_ON               (9)

#define CMD_RX_AACK_ON           (22)

#define CMD_TX_ARET_ON           (25)


#define RG_TRX_CTRL_0                    (0x03)

#define SR_PAD_IO                    0x03, 0xc0, 6

#define SR_PAD_IO_CLKM               0x03, 0x30, 4

#define CLKM_2mA                 (0)

#define CLKM_4mA                 (1)

#define CLKM_6mA                 (2)

#define CLKM_8mA                 (3)

#define SR_CLKM_SHA_SEL              0x03, 0x08, 3

#define SR_CLKM_CTRL                 0x03, 0x07, 0

#define CLKM_no_clock            (0)

#define CLKM_1MHz                (1)

#define CLKM_2MHz                (2)

#define CLKM_4MHz                (3)

#define CLKM_8MHz                (4)

#define CLKM_16MHz               (5)

#define RG_PHY_TX_PWR                    (0x05)

#define RF231_RF212_SR_IRQ_MASK_MODE    0x04, 0x02, 1
#define RF231_RF212_SR_IRQ_2_EXT_EN     0x04, 0x40, 6
#define RF231_RF212_SR_PA_BOOST 			RG_PHY_TX_PWR, 0x80, 7

#define RF230_SR_TX_AUTO_CRC_ON            0x05, 0x80, 7

#define RF231_RF212_SR_TX_AUTO_CRC_ON			0x04, 0x20, 5

#define SR_reserved_05_2             0x05, 0x70, 4

#define SR_TX_PWR                    0x05, 0x0f, 0

#define RG_PHY_RSSI                      (0x06)

#define SR_reserved_06_1             0x06, 0xe0, 5

#define SR_RSSI                      0x06, 0x1f, 0

#define RG_PHY_ED_LEVEL                  (0x07)

#define SR_ED_LEVEL                  0x07, 0xff, 0

#define RG_PHY_CC_CCA                    (0x08)

#define SR_CCA_REQUEST               0x08, 0x80, 7

#define SR_CCA_MODE                  0x08, 0x60, 5

#define SR_CHANNEL                   0x08, 0x1f, 0

#define RG_CCA_THRES                     (0x09)

#define SR_CCA_CS_THRES              0x09, 0xf0, 4

#define SR_CCA_ED_THRES              0x09, 0x0f, 0

#define RF231_RF212_SR_RX_SAFE_MODE   	0x0c, 0x80, 7

#define RF231_SR_OQPSK_DATA_RATE   0x0c, 0x03, 0

#define RF231_SR_ANT_DIV_EN   0x0d, 0x08, 3

#define RF231_SR_ANT_EXT_SW_EN   0x0d, 0x04, 2

#define RF231_ALTRATE_250KBPS   (0)

#define RF231_ANT_DIV_DISABLE   (0)

#define RF231_ANT_EXT_SW_SWITCH_DISABLE   (0)



#define RG_IRQ_MASK                      (0x0e)

#define SR_IRQ_MASK                  0x0e, 0xff, 0

#define RG_IRQ_STATUS                    (0x0f)

#define SR_IRQ_7_BAT_LOW             0x0f, 0x80, 7

#define SR_IRQ_6_TRX_UR              0x0f, 0x40, 6

#define SR_IRQ_5                     0x0f, 0x20, 5

#define SR_IRQ_4                     0x0f, 0x10, 4

#define SR_IRQ_3_TRX_END             0x0f, 0x08, 3

#define SR_IRQ_2_RX_START            0x0f, 0x04, 2

#define SR_IRQ_1_PLL_UNLOCK          0x0f, 0x02, 1

#define SR_IRQ_0_PLL_LOCK            0x0f, 0x01, 0

#define RG_VREG_CTRL                     (0x10)

#define SR_AVREG_EXT                 0x10, 0x80, 7

#define SR_AVDD_OK                   0x10, 0x40, 6

#define SR_AVREG_TRIM                0x10, 0x30, 4

#define AVREG_1_80V              (0)

#define AVREG_1_75V              (1)

#define AVREG_1_84V              (2)

#define AVREG_1_88V              (3)

#define SR_DVREG_EXT                 0x10, 0x08, 3

#define SR_DVDD_OK                   0x10, 0x04, 2

#define SR_DVREG_TRIM                0x10, 0x03, 0

#define DVREG_1_80V              (0)

#define DVREG_1_75V              (1)

#define DVREG_1_84V              (2)

#define DVREG_1_88V              (3)

#define RG_BATMON                        (0x11)
#define SR_reserved_11_1             0x11, 0xc0, 6

#define SR_BATMON_OK                 0x11, 0x20, 5

#define SR_BATMON_HR                 0x11, 0x10, 4

#define SR_BATMON_VTH                0x11, 0x0f, 0

#define RG_XOSC_CTRL                     (0x12)

#define SR_XTAL_MODE                 0x12, 0xf0, 4

#define SR_XTAL_TRIM                 0x12, 0x0f, 0

#define RF212_RG_WPAN_MODEL	  1

#define RF212_RG_TRX_CTRL_2		  0x0c
#define RF212_RG_CC_CTRL_0		  0x13
#define RF212_ARG_CC_TRRL_1		  0x14
#define RF212_SR_CC_BAND			  RG_CC_TRRL_1, 0x07, 0
#define RF212_RG_RF_CTRL_0		  0x16
#define RF212_SR_GC_TX_OFFS		  RG_RF_CTRL_0, 0x03, 0 
#define RF212_RG_XAH_CTRL_1		  0x17
#define RF212_SR_AACK_ACK_TIME	  RG_XAH_CTRL_1, 0x04, 2

#define RG_FTN_CTRL                      (0x18)

#define SR_FTN_START                 0x18, 0x80, 7
#define SR_reserved_18_2             0x18, 0x40, 6

#define SR_FTNV                      0x18, 0x3f, 0

#define RG_PLL_CF                        (0x1a)

#define SR_PLL_CF_START              0x1a, 0x80, 7
#define SR_reserved_1a_2             0x1a, 0x70, 4

#define SR_PLL_CF                    0x1a, 0x0f, 0

#define RG_PLL_DCU                       (0x1b)

#define SR_PLL_DCU_START             0x1b, 0x80, 7
#define SR_reserved_1b_2             0x1b, 0x40, 6

#define SR_PLL_DCUW                  0x1b, 0x3f, 0

#define RG_PART_NUM                      (0x1c)

#define SR_PART_NUM                  0x1c, 0xff, 0

#define RG_VERSION_NUM                   (0x1d)

#define SR_VERSION_NUM               0x1d, 0xff, 0

#define RG_MAN_ID_0                      (0x1e)

#define SR_MAN_ID_0                  0x1e, 0xff, 0

#define RG_MAN_ID_1                      (0x1f)

#define SR_MAN_ID_1                  0x1f, 0xff, 0

#define RG_SHORT_ADDR_0                  (0x20)

#define SR_SHORT_ADDR_0              0x20, 0xff, 0

#define RG_SHORT_ADDR_1                  (0x21)

#define SR_SHORT_ADDR_1              0x21, 0xff, 0

#define RG_PAN_ID_0                      (0x22)

#define SR_PAN_ID_0                  0x22, 0xff, 0

#define RG_PAN_ID_1                      (0x23)

#define SR_PAN_ID_1                  0x23, 0xff, 0

#define RG_IEEE_ADDR_0                   (0x24)

#define SR_IEEE_ADDR_0               0x24, 0xff, 0

#define RG_IEEE_ADDR_1                   (0x25)

#define SR_IEEE_ADDR_1               0x25, 0xff, 0

#define RG_IEEE_ADDR_2                   (0x26)

#define SR_IEEE_ADDR_2               0x26, 0xff, 0

#define RG_IEEE_ADDR_3                   (0x27)

#define SR_IEEE_ADDR_3               0x27, 0xff, 0

#define RG_IEEE_ADDR_4                   (0x28)

#define SR_IEEE_ADDR_4               0x28, 0xff, 0

#define RG_IEEE_ADDR_5                   (0x29)

#define SR_IEEE_ADDR_5               0x29, 0xff, 0

#define RG_IEEE_ADDR_6                   (0x2a)

#define SR_IEEE_ADDR_6               0x2a, 0xff, 0

#define RG_IEEE_ADDR_7                   (0x2b)

#define SR_IEEE_ADDR_7               0x2b, 0xff, 0

#define RG_XAH_CTRL                      (0x2c)

#define SR_MAX_FRAME_RETRIES         0x2c, 0xf0, 4

#define SR_MAX_CSMA_RETRIES          0x2c, 0x0e, 1
#define SR_reserved_2c_3             0x2c, 0x01, 0

#define RG_CSMA_SEED_0                   (0x2d)

#define SR_CSMA_SEED_0               0x2d, 0xff, 0

#define RG_CSMA_SEED_1                   (0x2e)

#define RF212_RF230_SR_AACK_FVN_MODE   	0x2e, 0xc0, 6

#define RF231_RG_CSMA_BE		0X2F

#define RF212_RF230_SR_MIN_BE	0X2F,0X0F,0

#define RF230_SR_MIN_BE 	0x2e,0xc0,6

#define SR_reserved_2e_2             0x2e, 0x30, 4

#define SR_I_AM_COORD                0x2e, 0x08, 3

#define SR_CSMA_SEED_1               0x2e, 0x07, 0

#define SR_AACK_SET_PD		  RG_CSMA_SEED_1, 0x20, 5


#endif 
#endif


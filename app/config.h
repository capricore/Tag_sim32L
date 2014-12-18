#define SYS_PARA_MAGIC_NUM 0x3344


/* Exciter id masks */
#define TEST_EXCITER_MASK (0x7000)		
#define CONFIG_EXCITER_MASK (0x2000)
#define EN_EX_EXCITER_MASK (0x6000)
#define GENERAL_EXCITER_MASK (0x4000)
#define EXCITER_MASK (0xf000)

#define SYN_WAKE_TIME (50)	//ms
#define SYN_TEST_TIME (10*1000)	//ms
#define SYN_CONFIG_TIME (5*60*1000)
#define SYN_ED_MIN 0x0 

#define CAVE_WAKE_TIME (100)
#define MAX_PCONSIGNAL (2)
#define MAX_PNOSIGNAL (3)
#define MAX_CNOSIGNAL (2)
#define CAVE_ED_MIN (0x0)

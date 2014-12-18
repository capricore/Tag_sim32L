#include <stdio.h>
#include <si32_device.h>
#include "CPU.h"

// MODULE VARIABLES
//------------------------------------------------------------------------------
//Variable that incrments 1000 times / second
volatile uint32_t msTicks;

// APB Clock Frequency in Hz
uint32_t SystemPeripheralClock;
uint32_t DataTxTime = 0;
extern int tDelay;

//==============================================================================
// 2nd LEVEL  INTERRUPT HANDLERS
//==============================================================================
/*NONE*/

//==============================================================================
// 1st LEVEL  INTERRUPT HANDLERS
//==============================================================================
void SysTick_Handler(void) 
{
  //msTicks++;  
  tDelay--;
  if(DataTxTime > 0)
	{
		DataTxTime--;
	}
  /*NO SECOND LEVEL HANDLER SPECIFIED*/
}

//------------------------------------------------------------------------------
void NMI_Handler(void) 
{
  printf("NMI_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/  
  halt();
}

//------------------------------------------------------------------------------
void HardFault_Handler(void) 
{
  printf("HardFault_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/  
  halt();
}

//------------------------------------------------------------------------------
void MemManage_Handler(void) 
{
  printf("MemManage_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/  
  halt();
}

//------------------------------------------------------------------------------
void BusFault_Handler(void) 
{
  printf("BusFault_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/  
  halt();
}

//------------------------------------------------------------------------------
void UsageFault_Handler(void) 
{
  printf("UsageFault_Handler\n");
  /*NO SECOND LEVEL HANDLER SPECIFIED (halt USED)*/  
  halt();
}


//==============================================================================
// CONFIGURATION FUNCTIONS
//==============================================================================
// Sets up systick
void cpu_update(void)
{
  set_ahb_clock(SystemCoreClock);

# if SI32_BASE_CPU_ARMV7M
  // set Priority for Cortex-M0 System Interrupts.
  NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);    
  // TBD the rest of them  
# endif
}

//==============================================================================
// SUPPORT FUNCTIONS
//==============================================================================
void set_ahb_clock(uint32_t freq)
{
  // UPDATE SYSTICK
  if (SysTick_Config(freq / 1000))   
  {
    printf("ERROR: SysTick_Config failed\n"); 
  }

  // UPDATE ITM DIVIDER
  *((uint32_t *) 0xE0040010) = ((50 * freq) / 20000000) - 1;
}// set_ahb_clock

//------------------------------------------------------------------------------
uint32_t get_msTicks(void)
{
  return msTicks;
}// get_msTicks

//------------------------------------------------------------------------------
void halt(void)
{
  printf("Halted\n");
  // Configurable Fault Status Register
  printf(" CFSR: 0x%08X\n", SCB->CFSR);
  // Hard Fault Status Register
  printf(" HFSR: 0x%08X\n", SCB->HFSR);
  // Debug Fault Status Register
  printf(" DFSR: 0x%08X\n", SCB->DFSR);
  // Memory Management Fault Address Register
  printf("MMFAR: 0x%08X\n", SCB->MMFAR);
  // Bus Fault Address register
  printf(" BFAR: 0x%08X\n", SCB->BFAR);
  //Auxiliary Fault Status Register
  printf(" AFSR: 0x%08X\n", SCB->AFSR);
  while(1);                         
}

//-eof--------------------------------------------------------------------------

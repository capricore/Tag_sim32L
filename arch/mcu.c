#include <stdint.h>

// void delay_us(uint32_t cnt)
// {
// 	uint32_t i ;
// 	while(cnt--){
// 		i = 20;
// 		while(i--);
// 	}	
// }

__asm void delay_us(uint32_t cnt)
{
	ALIGN
	PUSH {r1}
	MOV r1,#5		
	MUL r0,r1		
	SUB r0,#4				
loop
	SUBS r0,#1	
	CMP r0,#0
	BNE loop
	POP {r1}
	BX lr	
	
}


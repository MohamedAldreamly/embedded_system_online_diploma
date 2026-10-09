/*
 * CortexMX_OS.c
 *
 *	Created on	: OCT, 9 2026
 *  Author: Mohamed aldremly
 */

#include "CortexMX_OS_porting.h"

void HardFault_Handler(){
	while(1);
}
void MemManage_Handler(){
	while(1);
}
void BusFault_Handler(){
	while(1);
}

void UsageFault_Handler(){
	while(1);
}


__attribute__((naked)) void SVC_Handler(void)
{
    __asm volatile (
        "TST LR, #4           \n\t"
        "ITE EQ               \n\t"
        "MRSEQ R0, MSP        \n\t"
        "MRSNE R0, PSP        \n\t"
        "B OS_SVC_services    \n\t"
    );
}

void HW_init(){
	//	initalize Clock Tree (RCC -> SysTick Timer & CPU) 8MHZ
	//	8MHZ
	//	1 count -> 0.125 us
	//	X count -> 1ms
	//	X = 8000 count
}

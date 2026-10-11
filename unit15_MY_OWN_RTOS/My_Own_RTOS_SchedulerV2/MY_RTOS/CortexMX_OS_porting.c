/*
 * CortexMX_OS.c
 *
 *	Created on	: OCT, 9 2026
 *  Author: Mohamed aldremly
 */

#include "CortexMX_OS_porting.h"

unsigned char SysTickLED;

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
        "TST LR, #4			\n\t"
        "ITE EQ				\n\t"
        "MRSEQ R0, MSP		\n\t"
        "MRSNE R0, PSP		\n\t"
        "B OS_SVC			\n\t"
    );
}

void HW_init(){
	//initalize Clock Tree (RCC -> SysTick Timer & CPU) 8MHZ
	//init HW
	//Clock tree
	//RCC Default Value makes CPU Clock & SysTick Timer clock = 8MHZ



	//decrease PendSV interrupt priority to be smaller than or equal SysTICK Timer
	//SysTICK have a priority 14
	__NVIC_SetPriority(PendSV_IRQn, 15);

}

void trigger_OS_PendSV(){
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void Start_Ticker(){
	//This Clock tree should be defined in HW_init()
	//	8MHZ
	//	1 count -> 0.125 us
	//	X count -> 1ms
	//	X = 8000 count
	SysTick_Config(8000);
}

void SysTick_Handler(){

	SysTickLED ^=1;
	//determine Pcurrent & pnext
	Decide_whatNext();
	//Switch Context & Restore
	trigger_OS_PendSV();
}


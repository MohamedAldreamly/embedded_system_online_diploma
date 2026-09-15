/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    main.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
 //Learn-in-depth
 
#include "RCC.h"
#include "EXTI.h"
#include "NVIC.h"

// register address
#define GPIOA_BASE 		0x40010800
#define GPIOA_CRH  		*(volatile uint32 *)(GPIOA_BASE + 0x04)
#define GPIOA_ODR 		*(volatile uint32 *)(GPIOA_BASE + 0x0C)

void GPIO_TOGLE_LED(){
	GPIOA_ODR ^= 1<<13 ;
}
int main(void)
{
	RCC_enumiInit();
	RCC_enumEnablePeripheralClock	( APB2_BUS , GPOIA_RCC );
	EXTI_enumInit(EXTI_LINE_0, EXTI_PORTA, Rising_Edge , GPIO_TOGLE_LED) ;                                 

	//Init GPIOA
	GPIOA_CRH &= 0xFF0FFFFF;
	GPIOA_CRH |= 0x00200000;
	while(1)
	{
	}
}


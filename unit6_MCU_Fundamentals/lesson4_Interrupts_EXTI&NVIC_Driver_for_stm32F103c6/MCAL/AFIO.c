/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   AFIO.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
#include "AFIO.h"
 
void AFIO_Init(void)
{
    /*
     * Enable AFIO clock on APB2
     */
    RCC_enumEnablePeripheralClock(APB2_BUS, AFIO_RCC);
}
/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   NVIC.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
#include "NVIC.h"

void NVIC_voidEnableInterrupt(uint8 Copy_u8IRQNumber)
{
	NVIC->ISER[Copy_u8IRQNumber / 32] = (1UL << (Copy_u8IRQNumber % 32));
}

void NVIC_voidDisableInterrupt(uint8 Copy_u8IRQNumber)
{
	NVIC->ICER[Copy_u8IRQNumber / 32] = (1UL << (Copy_u8IRQNumber % 32));
}


void NVIC_SetPendingInterrupt(uint8 Copy_u8IRQNumber)
{
    NVIC->ISPR[Copy_u8IRQNumber / 32U] = (1UL << (Copy_u8IRQNumber % 32U));
}

void NVIC_ClearPendingInterrupt(uint8 Copy_u8IRQNumber)
{
    NVIC->ICPR[Copy_u8IRQNumber / 32U] = (1UL << (Copy_u8IRQNumber));
}

uint8 NVIC_GetPendingInterrupt(uint8 Copy_u8IRQNumber)
{
    return (uint8)
           ((NVIC->ISPR[Copy_u8IRQNumber / 32U] >> (Copy_u8IRQNumber % 32U)) & 1UL);
}

uint8 NVIC_GetActiveInterrupt(uint8 Copy_u8IRQNumber)
{
    return (uint8)
           ((NVIC->IABR[Copy_u8IRQNumber / 32U] >> (Copy_u8IRQNumber % 32U)) & 1UL);
}

void NVIC_SetPriority(uint8 Copy_u8IRQNumber, uint8 Priority)
{
    if (Priority < 16U)
    {
        NVIC->IPR[Copy_u8IRQNumber] = (uint8)(Priority << 4U);
    }
}

uint8 NVIC_GetPriority(uint8 Copy_u8IRQNumber)
{
    return (uint8)(NVIC->IPR[Copy_u8IRQNumber] >> 4U);
}

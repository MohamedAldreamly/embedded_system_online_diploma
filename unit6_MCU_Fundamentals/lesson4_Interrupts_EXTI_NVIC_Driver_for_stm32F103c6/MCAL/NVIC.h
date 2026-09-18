/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    NVIC.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 

#ifndef NVIC_H_
#define NVIC_H_

#include "BIT_MATH.h"
#include "STD_TYPES.h"

/*		External Interrupt Numbers	for NVIC	*/

/* NVIC Base Address */
#define NVIC_BASE_ADDRESS    0xE000E100UL

typedef struct
{
    volatile uint32 ISER[8];       /* 0x000 - 0x01C */
    uint32 RESERVED0[24];           /* 0x020 - 0x07C */
    volatile uint32 ICER[8];       /* 0x080 - 0x09C */
    uint32 RESERVED1[24];           /* 0x0A0 - 0x0FC */
    volatile uint32 ISPR[8];       /* 0x100 - 0x11C */
    uint32 RESERVED2[24];           /* 0x120 - 0x17C */
    volatile uint32 ICPR[8];       /* 0x180 - 0x19C */
    uint32 RESERVED3[24];           /* 0x1A0 - 0x1FC */
    volatile uint32 IABR[8];       /* 0x200 - 0x21C */
    uint32 RESERVED4[56];           /* 0x220 - 0x2FC */
    volatile uint8 IPR[240];       /* 0x300 - 0x3EF */

} NVIC_t;

/* NVIC Peripheral Pointer */
#define NVIC    ((NVIC_t *)NVIC_BASE_ADDRESS)

void NVIC_voidEnableInterrupt(uint8 Copy_u8IRQNumber);
void NVIC_voidDisableInterrupt(uint8 Copy_u8IRQNumber);
void NVIC_SetPendingInterrupt(uint8 Copy_u8IRQNumber);
void NVIC_ClearPendingInterrupt(uint8 Copy_u8IRQNumber);
uint8 NVIC_GetPendingInterrupt(uint8 Copy_u8IRQNumber);
uint8 NVIC_GetActiveInterrupt(uint8 Copy_u8IRQNumber);
void NVIC_SetPriority(uint8 Copy_u8IRQNumber, uint8 Priority);
uint8 NVIC_GetPriority(uint8 Copy_u8IRQNumber);


#endif /*	NVIC_H_	*/

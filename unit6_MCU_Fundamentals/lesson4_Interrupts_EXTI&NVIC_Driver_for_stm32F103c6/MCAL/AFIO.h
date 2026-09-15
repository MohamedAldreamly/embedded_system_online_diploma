/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    AFIO.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 

#ifndef AFIO_H_
#define AFIO_H_

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "RCC.h"

/* AFIO Base Address */
#define AFIO_BASE_ADDRESS        0x40010000UL


typedef struct
{
    volatile uint32 EVCR;
    volatile uint32 MAPR;
    volatile uint32 EXTICR[4];
    volatile uint32 MAPR2;

} AFIO_Type;

#define AFIO                    ((AFIO_Type *)AFIO_BASE_ADDRESS)



#define EXTI_PORTA  0b0000
#define EXTI_PORTB  0b0001
#define EXTI_PORTC  0b0010
#define EXTI_PORTD  0b0011
#define EXTI_PORTE  0b0100
#define EXTI_PORTF  0b0101
#define EXTI_PORTG  0b0110

#define AFIO_EN 0


void AFIO_Init(void);

#endif /*	AFIO_H_	*/

/* 
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    EXTI.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Update  : SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : ATMEGA32
 *
 */
#ifndef EXTI_H_
#define EXTI_H_

#include "STD_TYPES.h"
#include "BIT_MATH.h"

/* =========================================================
 *                EXTI Sense Control
 * =========================================================
 */

/*
 * INT0:
 *
 * ISC01 ISC00
 *   0     0   -> Low Level
 *   0     1   -> Any Logical Change
 *   1     0   -> Falling Edge
 *   1     1   -> Rising Edge
 *
 *
 * INT1:
 *
 * ISC11 ISC10
 *   0     0   -> Low Level
 *   0     1   -> Any Logical Change
 *   1     0   -> Falling Edge
 *   1     1   -> Rising Edge
 */

#define INT0_ISC_POSITION       0U
#define INT1_ISC_POSITION       2U

#define LOW_LEVEL               0U
#define ANY_LOGICAL_CHANGE      1U
#define FALLING_EDGE            2U
#define RISING_EDGE             3U


/* =========================================================
 *                     MCUCR Register
 * =========================================================
 */

#define EXTI_MCUCR       (*(volatile uint8*)0x55)


/* =========================================================
 *                      INT2 Control
 * =========================================================
 */

/*
 * ISC2:
 *
 * 0 -> Falling Edge
 * 1 -> Rising Edge
 */

#define INT2_ISC_POSITION                    6U

#define INT2_FALLING_EDGE       0U
#define INT2_RISING_EDGE        1U


/* =========================================================
 *                    MCUCSR Register
 * =========================================================
 */

#define EXTI_MCUCSR      (*(volatile uint8*)0x54)


/* =========================================================
 *             General Interrupt Control Register
 * =========================================================
 *
 * Bit 7 : INT1 Enable
 * Bit 6 : INT0 Enable
 * Bit 5 : INT2 Enable
 */

#define EXTI_GICR        (*(volatile uint8*)0x5B)

#define INT1_ENABLE_BIT         7U
#define INT0_ENABLE_BIT         6U
#define INT2_ENABLE_BIT         5U


/* =========================================================
 *              General Interrupt Flag Register
 * =========================================================
 *
 * Bit 7 : INTF1
 * Bit 6 : INTF0
 * Bit 5 : INTF2
 */

#define EXTI_GIFR        (*(volatile uint8*)0x5A)

#define INTF1_FLAG_BIT          7U
#define INTF0_FLAG_BIT          6U
#define INTF2_FLAG_BIT          5U


/*	Status Register
 * Bit 7 : Global Interrupt Enable
 */

#define EXTI_SREG        (*(volatile uint8*)0x5F)

#define GIE                     7U


#define EXTI_INT0 0 
#define EXTI_INT1 1 
#define EXTI_INT2 2 

// APIs Defination

void EXTI_voidInit(void);
void EXTI_voidActiveINT(uint8 Copy_u8EXTI_LINE , uint8 Copy_u8EXTISenceMode);


#endif /* EXTI_H_ */
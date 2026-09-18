/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    EXTI.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : ATMEGA32
 *
 */ 
 
#include <avr/interrupt.h> 
#include "EXTI.h"

void EXTI_voidInit(void){
	
	/*	Global Interrupt Enable	*/
	SET_BIT(EXTI_SREG , GIE);
}

void EXTI_voidActiveINT(uint8 Copy_u8EXTI_LINE , uint8 Copy_u8EXTISenceMode){
	switch (Copy_u8EXTI_LINE){
		case EXTI_INT0:
			// configer Mode 
			CLR_FIELD(EXTI_MCUCR , INT0_ISC_POSITION ,2);
			SET_FIELD(EXTI_MCUCR , INT0_ISC_POSITION,Copy_u8EXTISenceMode);
			// INT0 ENABLE
			SET_BIT(EXTI_GICR , INT0_ENABLE_BIT);
			break ;
		case EXTI_INT1:
			// configer Mode 
			CLR_FIELD(EXTI_MCUCR , INT1_ISC_POSITION ,2);
			SET_FIELD(EXTI_MCUCR , INT1_ISC_POSITION,Copy_u8EXTISenceMode);
			// INT1 ENABLE
			SET_BIT(EXTI_GICR , INT1_ENABLE_BIT);
			break ;
		case EXTI_INT2:
			// configer Mode 
			CLR_FIELD(EXTI_MCUCSR , INT2_ISC_POSITION ,1);
			SET_FIELD(EXTI_MCUCSR , INT2_ISC_POSITION,Copy_u8EXTISenceMode);
			// INT2 ENABLE
			SET_BIT(EXTI_GICR , INT2_ENABLE_BIT);
			break ;	
		default:
			break;	
	}
}
	
ISR(INT0_vect)
{
    /* Code executed when INT0 occurs */
	TOG_BIT(PORTD, 5);
}

ISR(INT1_vect)
{
    /* Code executed when INT0 occurs */
	TOG_BIT(PORTD, 6);
	
}

ISR(INT2_vect)
{
    /* Code executed when INT0 occurs */
	TOG_BIT(PORTD, 7);
}
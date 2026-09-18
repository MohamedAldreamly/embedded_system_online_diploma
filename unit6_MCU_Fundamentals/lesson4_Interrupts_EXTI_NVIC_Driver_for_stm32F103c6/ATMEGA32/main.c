/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    main.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *	Update	: SEP, 15 2026
 *  Target  : ATMEGA32
 *
 */ 
 

#define PORTD		*((volatile uint8*)0x32)
#define DDRD 		*((volatile uint8*)0x31)
#define PIND 		*((volatile uint8*)0x30)

#include "EXTI.h"
int main (void){
	
	/* PD2 -> Input */
	CLR_BIT(DDRD, 2);

	/* Enable internal pull-up */
	SET_BIT(PORTD, 2);
	
	/* PD5 -> Output */
    SET_BIT(DDRD, 5);

    /* Initially LOW */
    CLR_BIT(PORTD, 5);

	/* INT0 on Rising Edge */
	EXTI_voidActiveINT(EXTI_INT0, RISING_EDGE);

	/* Global Interrupt Enable */
	EXTI_voidInit();
	
	while (1){
		
	}
}
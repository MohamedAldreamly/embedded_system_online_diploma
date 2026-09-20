/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    main.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *	Date	: SEP, 16 2026
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
 //Learn-in-depth
 
#include "RCC.h"
#include "EXTI.h"
#include "NVIC.h"

int main (void){
	GPIO_voidInit();
	
	GPIO_voidSetMode	(GPIOA , GPIO_Input_mode, GPIO_Floating_input, GPIO_PIN1);
	GPIO_voidReadPin	(GPIOA , GPIO_PIN1);
	GPIO_voidSetMode	(GPIOA , GPIO_Output_mode_speed_2_MHz, GPIO_output_push-pull, GPIO_PIN1);

	GPIO_voidWritePin	(GPIO_t Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8Value);
	
}
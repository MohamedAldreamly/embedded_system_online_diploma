/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   GPIO.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 16 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
#include "GPIO.h"

void GPIO_voidInit(void){
	
    RCC_enumEnablePeripheralClock(APB2_BUS, GPIOA_RCC);
}

void GPIO_voidSetMode	(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8PortMode, uint8 Copy_u8Mode){
	
	
	if (Copy_u8Pin>=8){
		REG = 1 ; 
		Copy_u8Pin -=8;
	}

	if (REG==0){
		CLR_FIELD(Copy_Port->CRL,Copy_u8Pin*4,4);
		SET_FIELD(Copy_Port->CRL,Copy_u8Pin*4, Copy_u8PortMode);
		SET_FIELD(Copy_Port->CRL,Copy_u8Pin*4+2, Copy_u8Mode);

		
	}else{ 
		CLR_FIELD(Copy_Port->CRH,Copy_u8Pin*4,4);
		SET_FIELD(Copy_Port->CRH,Copy_u8Pin*4, Copy_u8PortMode);
		SET_FIELD(Copy_Port->CRH,Copy_u8Pin*4+2, Copy_u8Mode);

	}	
}


uint8 GPIO_u8ReadPin(volatile  GPIO_t *Copy_Port, uint8 Copy_u8Pin){
    return (uint8)GET_BIT(Copy_Port->IDR, Copy_u8Pin);
}
void GPIO_voidWritePin(volatile GPIO_t *Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8Value)
{
    if (Copy_u8Value == GPIO_PIN_HIGH){
        Copy_Port->BSRR = (1UL << Copy_u8Pin);
    }else{
        Copy_Port->BRR = (1UL << Copy_u8Pin);
    }
}

void GPIO_voidResetPin	(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin){
	SET_BIT(Copy_Port->BRR,Copy_u8Pin);
}
void GPIO_voidSetPin	(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin){
	SET_BIT(Copy_Port->BSRR,Copy_u8Pin);
}

void GPIO_voidSetPullMode	(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8PullMode){
	if (Copy_u8PullMode == GPIO_INPUT_PULLUP){
        Copy_Port->BSRR = (1UL << Copy_u8Pin);
    }else{
        Copy_Port->BRR = (1UL << Copy_u8Pin);
    }
}

int wait_ms(uint32_t time){
	uint32_t i ,j;
	for (i=0;i<time;i++)
		for (j=0;j<255;j++);
}



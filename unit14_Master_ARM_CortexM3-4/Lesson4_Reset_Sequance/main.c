/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 17 2026
 *
 */

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_EXTI_driver.h"



unsigned int IRQ_Flag = 0 ;

enum CPUAccessLevel{
	privileged,
	unprivileged
};

void SWITCH_CPU_AccessLevel(enum CPUAccessLevel level){
	switch(level){
	case privileged:
		//clear bit 0 in CONTROL REG
		__asm(	"mrs r3 , CONTROL	\n\t"
				"lsr r3,r3,#0x1		\n\t"
				"lsl r3,r3,#0x1		\n\t"
				"msr CONTROL,r3		\n\t");

		break;

	case unprivileged:
		//set bit 0 in CONTROL REG
		__asm(	"mrs r3 , CONTROL	\n\t"
				"orr r3,r3,#1		\n\t"
				"msr CONTROL,r3	\n\t");
		break;
	}
}


void HardFault_Handler(){

}

void MemManage_Handler(){

}

void BusFault_Handler(){

}
void UsageFault_Handler(){

}

void EXTI9_Callback(void){
	IRQ_Flag = 1;

	//CPU in Handler Mode
	SWITCH_CPU_AccessLevel(privileged);

}

void  clock_init(){
	//Enable clock GPIOA
	RCC_GPIOA_CLK_EN();
	//Enable clock GPIOB
	RCC_GPIOB_CLK_EN();
	//Enable clock AFIO
	RCC_AFIO_CLK_EN();
}

void EXTI_init(){

	EXTI_PinConfig_t EXTI_CFG;
	EXTI_CFG.EXTI_PIN = EXTI9PB9;
	EXTI_CFG.Trigger_Case = EXTI_Trigger_RISING;
	EXTI_CFG.P_IRQ_CallBack =EXTI9_Callback;
	EXTI_CFG.IRQ_EN = EXTI_IRQ_Enable;

	MCAL_EXTI_GPIO_Init(&EXTI_CFG);
}



int main(void)
{

	clock_init();
	EXTI_init();

	IRQ_Flag = 1;
	SWITCH_CPU_AccessLevel(unprivileged);


    while(1){
    	if(IRQ_Flag){
    		IRQ_Flag = 0;
    	}
    }
}

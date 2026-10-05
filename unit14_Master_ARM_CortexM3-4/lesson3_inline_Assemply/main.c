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

#include "lcd.h"
#include "keypad.h"

unsigned int IRQ_Flag = 0 ;


unsigned int CPU_CONTROL_register = 0 ;
unsigned int CPU_IPSR_register = 0 ;

void EXTI9_Callback(void){
	IRQ_Flag = 1;

	//CPU in Handler Mode

//MRS CPU_IPSR_register,IPSR
	__asm("MRS %[out0],IPSR"
			:[out0]"=r"(CPU_IPSR_register));

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

//int VAL1 = 3;
//int VAL2 = 7;
//int VAL3 = 10;



int main(void)
{

	clock_init();
	EXTI_init();

	IRQ_Flag = 1;

	__asm("nop \n\t nop \n\t nop \n\t");
////mov VAL1 , 0Xff
//	__asm("mov %0,#0xff"
//			:"=r"(VAL1));//frist : output parameter //"=r" only Write

////mov R0,VAL1
//	__asm("mov R0,%0"
//				:	//frist : output parameter
//				:"r"(VAL1));//Second : input parameter //"r" Write And Read

////add VAl3,VAL1,VAl2
//	__asm("add %[out0],%[in0],%[in1]"
//			:[out0]"=r"(VAL3)		//frist : output parameter
//			:[in0]"r"(VAL1),		//Second : input parameter
//			 [in1]"r"(VAL2)
//			 :"r3");// you tell to compilar to not use this reg

	// CPU in thread mode
//MRS CPU_CONTROL_register, CONTROL
	__asm("MRS %[out0], CONTROL"
			:[out0]"=r"(CPU_CONTROL_register));




	__asm("nop \n\t nop \n\t nop \n\t");

    while(1){
    	if(IRQ_Flag){
    		IRQ_Flag = 0;
    	}
    }
}

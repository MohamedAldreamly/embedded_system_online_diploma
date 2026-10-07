/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: OCT, 7 2026
 *
 */

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_EXTI_driver.h"

#define TaskA_Stack_Size 100	//100 Byte
#define TaskB_Stack_Size 100	//100 Byte

extern unsigned int _estack;

//main Stack
unsigned int* _S_MSP = &_estack;
unsigned int _E_MSP ;

//Procces Stack Task A
unsigned int _S_PSP_TA ;
unsigned int _E_PSP_TA ;

//Procces Stack Task B
unsigned int _S_PSP_TB ;
unsigned int _E_PSP_TB ;

#define OS_SET_PSP(add)			__asm volatile("mov r0, %0 \n\t msr PSP,r0" ::"r"(add))
#define OS_SWITCH_SP_to_PSP		__asm volatile("mrs r0,CONTROL \n\t mov r1,#0x02 \n\t orr r0,r0,r1 \n\t msr CONTROL,r0")
#define OS_SWITCH_SP_to_MSP		__asm volatile("mrs r0,CONTROL \n\t mov r1,#0x05 \n\t and r0,r0,r1 \n\t msr CONTROL,r0")


#define OS_Generate_Exception	__asm volatile("SVC #0x3");

uint8_t TASKA_flag,TASKB_flag, IRQ_Flag = 0 ;

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

void SVC_Handler(){
	SWITCH_CPU_AccessLevel(privileged);
}

void EXTI9_Callback(void){

	if(IRQ_Flag == 0){
		TASKA_flag = 1;
		IRQ_Flag = 1;
	}else if (IRQ_Flag == 1)
	{
		TASKB_flag = 1;
		IRQ_Flag = 0;
	}




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





int TaskA (int a, int b, int c){

	return a+b+c;
}

int TaskB (int a, int b, int c,int d){

	return a+b+c;
}

void MainOS(){
	//Main Stack
	_E_MSP = (*_S_MSP - 512);

	//Task A
	_S_PSP_TA = (_E_MSP - 4);
	_E_PSP_TA = (_S_PSP_TA - TaskA_Stack_Size);

	//Task B
	_S_PSP_TB = (_E_PSP_TA - 4);
	_E_PSP_TB = (_S_PSP_TB - TaskB_Stack_Size);

	while (1){
		__asm("NOP");
		if (TASKA_flag==1){
			//set PSP Register = _S_PSP_TA
			OS_SET_PSP(_S_PSP_TA);
			// SP -> PSP
			OS_SWITCH_SP_to_PSP;
			//Switch from privileged -> unprivileged
			SWITCH_CPU_AccessLevel(unprivileged);

			TaskA(1, 2, 3);

			//Switch from unprivileged -> privileged
			OS_Generate_Exception;
			// SP -> PSP
			OS_SWITCH_SP_to_MSP;

		}else if (TASKA_flag==1)
		{
			//set PSP Register = _S_PSP_TB
			OS_SET_PSP(_S_PSP_TB);
			// SP -> PSP
			OS_SWITCH_SP_to_PSP;
			//Switch from privileged -> unprivileged
			SWITCH_CPU_AccessLevel(unprivileged);


			TaskB(1, 2, 3,4);

			//Switch from unprivileged -> privileged
			OS_Generate_Exception;
			// SP -> PSP
			OS_SWITCH_SP_to_MSP;
		}

	}
}

int main(void)
{

	clock_init();
	EXTI_init();

	MainOS();

	IRQ_Flag = 1;

    while(1){
    	if(IRQ_Flag){
    		IRQ_Flag = 0;
    	}
    }
}

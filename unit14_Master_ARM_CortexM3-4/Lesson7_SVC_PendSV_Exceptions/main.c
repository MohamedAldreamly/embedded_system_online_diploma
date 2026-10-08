/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: OCT, 7 2026
 *
 */

#include "core_cm3.h"

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_EXTI_driver.h"


void PendSV_Handler(){

}

void OS_SVC_services (int* stackFromPinter){
	//OS_SVC_Set Stack -> r0 -> argument = = stackFromPinter
	//OS_SVC_Set Stack : r0,r1,r2,r3,r12,LR,PC,XPSR

	unsigned char SVC_number ;
	unsigned int val1,val2;

	SVC_number = *((unsigned char*)(((unsigned char*)stackFromPinter[6])[-2]));
	val1 = stackFromPinter[0];
	val2 = stackFromPinter[1];

	switch (SVC_number){
		case 1:	//add
			stackFromPinter[0] = val1 + val2;
			break;
		case 2: //sub
			stackFromPinter[0] = val1 - val2;
			break;
		case 3: //mul
			stackFromPinter[0] = val1 * val2;
			break;
		case 4: //PendSV
			SCB->ICSR |= SCB_CFSR_IMPRECISERR_Msk;
			break;
		}

}
__attribute ((naked)) void SVC_Handler()
{
	__asm("TST LR,#0x04 \n \t"
			"ITE EQ"
			"MSREQ r0 MSP \n \t"
			"MSRNE r0 PSP \n \t"
			"B OS_SVC_services");
}

int OS_SVC_Set(int a, int b, int SVC_ID){

	int result;
	switch (SVC_ID){
	case 1:	//add
		__asm("SVC #0x01");
		break;
	case 2: //sub
		__asm("SVC #0x02");
		break;
	case 3: //mul
		__asm("SVC #0x03");
		break;
	}
	__asm("MOV %0,r0":"=r"(result));
	return result ;
}

int main(void)
{

	IRQ_Flag = OS_SVC_Set(3,3,1);//add
	IRQ_Flag = OS_SVC_Set(3,3,1);//sub
	IRQ_Flag = OS_SVC_Set(3,3,1);//mul
	IRQ_Flag = OS_SVC_Set(0,0,4);//PendSV


    while(1){

    }
}

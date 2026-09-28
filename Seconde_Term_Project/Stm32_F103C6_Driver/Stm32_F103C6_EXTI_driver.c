/*
 * Stm32_F103C6_EXTI_driver.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 18 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

//include
#include "Stm32_F103C6_EXTI_driver.h"

/*
 *==========================================================
 *                  	Generic Variable
 *==========================================================
 */

void (*GP_IRQ_Callbacks [16]) () ;

/*
 *==========================================================
 *                  	Generic Macros
 *==========================================================
 */

#define AFIO_GPIO_EXTI_Mapping(x)	(	(x==GPIOA)?0:\
										(x==GPIOB)?1:\
										(x==GPIOC)?2:\
										(x==GPIOD)?3:0	)


/*
 *===========================================================
 *					 	Generic Functions
 *===========================================================
 */

void Enable_NVIC(uint16_t IRQ){
	switch (IRQ) {
		case 0 :
			NVIC_IRQ6_EXTI0_Enable;
			break;
		case 1 :
			NVIC_IRQ7_EXTI1_Enable;
			break;
		case 2 :
			NVIC_IRQ8_EXTI2_Enable;
			break;
		case 3 :
			NVIC_IRQ9_EXTI3_Enable;
			break;
		case 4 :
			NVIC_IRQ10_EXTI4_Enable;
			break;
		case 5 :
		case 6 :
		case 7 :
		case 8 :
		case 9 :
			NVIC_IRQ23_EXTI5_9_Enable;
			break;
		case 10 :
		case 11 :
		case 12 :
		case 13 :
		case 14 :
		case 15 :
			NVIC_IRQ40_EXTI10_15_Enable;
			break;
	}
}

void Disable_NVIC(uint16_t IRQ){
	switch (IRQ) {
		case 0 :
			NVIC_IRQ6_EXTI0_Disable;
			break;
		case 1 :
			NVIC_IRQ7_EXTI1_Disable;
			break;
		case 2 :
			NVIC_IRQ8_EXTI2_Disable;
			break;
		case 3 :
			NVIC_IRQ9_EXTI3_Disable;
			break;
		case 4 :
			NVIC_IRQ10_EXTI4_Disable;
			break;
		case 5 :
		case 6 :
		case 7 :
		case 8 :
		case 9 :
			NVIC_IRQ23_EXTI5_9_Disable;
			break;
		case 10 :
		case 11 :
		case 12 :
		case 13 :
		case 14 :
		case 15 :
			NVIC_IRQ40_EXTI10_15_Disable;
			break;
	}
}

void Update_EXTI (EXTI_PinConfig_t* EXTI_Config){

	//1- Configure GPIO to be AF Input -> Floting INPUT

	GPIO_PinConfig_t PinCfg;
	PinCfg.GPIO_PinNumber = EXTI_Config->EXTI_PIN.GPIO_PIN;
	PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
	MCAL_GPIO_Init(EXTI_Config->EXTI_PIN.GPIO_Port,&PinCfg);
	//===============================================

	//2- UPDATE AFIO to Route between EXTI Line with PORT A,B,C,D
	uint8_t AFIO_EXTICR_index    = EXTI_Config->EXTI_PIN.EXTI_InputLineNumber / 4;
	uint8_t AFIO_EXTICR_Position = (EXTI_Config->EXTI_PIN.EXTI_InputLineNumber % 4) * 4;

	//clear the for bit
	AFIO->EXTICR[AFIO_EXTICR_index] &= ~(0x0F<<AFIO_EXTICR_Position);
	AFIO->EXTICR[AFIO_EXTICR_index] |= ((AFIO_GPIO_EXTI_Mapping(EXTI_Config->EXTI_PIN.GPIO_Port) & 0x0F) << AFIO_EXTICR_Position);
	//===============================================

	//3- Update Rising and Falling Register
	EXTI->RTSR &= ~(1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	EXTI->FTSR &= ~(1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);

	if(EXTI_Config->Trigger_Case == EXTI_Trigger_RISING){
		EXTI->RTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}else if(EXTI_Config->Trigger_Case == EXTI_Trigger_RISING){
		EXTI->FTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}else if (EXTI_Config->Trigger_Case == EXTI_Trigger_RISING){
		EXTI->RTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
		EXTI->FTSR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}

	//===============================================
	//4- Update IRQ Handling CALLBACK
	GP_IRQ_Callbacks[EXTI_Config->EXTI_PIN.EXTI_InputLineNumber] = EXTI_Config->P_IRQ_CallBack;

	//===============================================
	//5- Eanble/Disable IRQ EXTI&NVIC

	if(EXTI_Config->IRQ_EN == EXTI_IRQ_Enable){
		EXTI->IMR |= (1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
		Enable_NVIC(EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}else{
		EXTI->IMR &= ~(1<<EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
		Disable_NVIC(EXTI_Config->EXTI_PIN.EXTI_InputLineNumber);
	}
}



/*
 *===========================================================
 *					 	APIs Functions Definitions
 *===========================================================
 */


/**================================================================
 * @Fn				-MCAL_EXTI_GPIO_DeInit
 * @brief		 	-reset EXTI Resister and NVIC corresponding to IRQ MASK
 * @retval			-none
 * Note				-none
 */

void MCAL_EXTI_GPIO_DeInit(void){
	EXTI->IMR = 0x00000000;
	EXTI->EMR = 0x00000000;
	EXTI->RTSR = 0x00000000;
	EXTI->FTSR = 0x00000000;
	EXTI->SWIER = 0x00000000;
	//rc_w cleared by writing a '1' into the bit
	EXTI->PR = 0xFFFFFFFF;

	//Disable EXTI IRQ from NVIC

	NVIC_IRQ6_EXTI0_Disable;
	NVIC_IRQ7_EXTI1_Disable;
	NVIC_IRQ8_EXTI2_Disable;
	NVIC_IRQ9_EXTI3_Disable;
	NVIC_IRQ10_EXTI4_Disable;
	NVIC_IRQ23_EXTI5_9_Disable;
	NVIC_IRQ40_EXTI10_15_Disable ;

}


/**================================================================
* @Fn				-MCAL_EXTI_GPIO_Init
* @brief		 	-this is used to Initialize EXTI from Specific GPIO PIN and specify the MASK/Trigger Condition and IRQ CallBack
* @param[in] 		-EXTI_Config set by @ref EXTI_define, EXTI_define, EXTI_Trigger_define and EXTI_IQR_define
* @retval			-none
* Note				-Stm32F103C6 MCU has GPIO A,B,C,D,E Modules
* 						But LQFP48 Package has only GPIO A,B,PART of C/D eexported as external PINS from the MCU
*					-Also Mandatory to Enable RCC Clock for AFIO and the corresponding GPIO
*/

void MCAL_EXTI_GPIO_Init(EXTI_PinConfig_t* EXTI_Config){

	Update_EXTI (EXTI_Config);
}

/**================================================================
* @Fn				-MCAL_EXTI_GPIO_Update
* @brief		 	-this is used to Initialize EXTI from Specific GPIO PIN and specify the MASK/Trigger Condition and IRQ CallBack
* @param[in] 		-EXTI_Config set by @ref EXTI_define, EXTI_define, EXTI_Trigger_define and EXTI_IQR_define
* @retval			-none
* Note				-Stm32F103C6 MCU has GPIO A,B,C,D,E Modules
* 						But LQFP48 Package has only GPIO A,B,PART of C/D eexported as external PINS from the MCU
*					-Also Mandatory to Enable RCC Clock for AFIO and the corresponding GPIO
*/

void MCAL_EXTI_GPIO_Update(EXTI_PinConfig_t* EXTI_Config){

	Update_EXTI (EXTI_Config);
}


/*
 *==========================================================
 *                  	ISR Functions
 *==========================================================
 */

void EXTI0_IRQHandler(void)
{
	/*	Clear pending Bit		*/
		EXTI->PR = (1UL << 0);

	if (GP_IRQ_Callbacks[0] != NULL)
	{
		GP_IRQ_Callbacks[0]();  // Calling the func @ main
	}

}

void EXTI1_IRQHandler(void)
{
	if (GP_IRQ_Callbacks[1] != NULL)
	{
		GP_IRQ_Callbacks[1]();  // Calling the func @ main
	}
	/*	Clear pending Bit		*/
	EXTI->PR = (1UL << 1);
}
void EXTI2_IRQHandler(void)
{
	if (GP_IRQ_Callbacks[2] != NULL)
	{
		GP_IRQ_Callbacks[2]();  // Calling the func @ main
	}
	/*	Clear pending Bit		*/
	EXTI->PR = (1UL << 2);

}
void EXTI3_IRQHandler(void)
{
	if (GP_IRQ_Callbacks[3] != NULL)
	{
		GP_IRQ_Callbacks[3]();  // Calling the func @ main
	}
	/*	Clear pending Bit		*/
	EXTI->PR = (1UL << 3);

}

void EXTI4_IRQHandler(void)
{
	if (GP_IRQ_Callbacks[4] != NULL)
	{
		GP_IRQ_Callbacks[4]();  // Calling the func @ main
	}
	/*	Clear pending Bit		*/
	EXTI->PR = (1UL << 4);

}
void EXTI9_5_IRQHandler(void)
{
	if (EXTI -> PR & (1 << 5))  // Check if the pending bit of line 5 is set
	{
		if (GP_IRQ_Callbacks[5] != NULL)
		{
			GP_IRQ_Callbacks[5]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 5);
	}

	if (EXTI -> PR & (1 << 6))  // Check if the pending bit of line 6 is set
	{
		if (GP_IRQ_Callbacks[6] != NULL)
		{
			GP_IRQ_Callbacks[6]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 6);
	}

	if (EXTI -> PR & (1 << 7))  // Check if the pending bit of line 5 is set
	{
	if (GP_IRQ_Callbacks[7] != NULL)
		{
		GP_IRQ_Callbacks[7]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 7);
	}

	if (EXTI -> PR & (1 << 8))  // Check if the pending bit of line 5 is set
	{
		if (GP_IRQ_Callbacks[8] != NULL)
		{
			GP_IRQ_Callbacks[8]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 8);
	}

	if (EXTI -> PR & (1 << 9))  // Check if the pending bit of line 5 is set
	{
		if (GP_IRQ_Callbacks[9] != NULL)
		{
			GP_IRQ_Callbacks[9]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 9);

	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI -> PR & (1 << 10))  // Check if the pending bit of line 10 is set
	{
		if (GP_IRQ_Callbacks[10] != NULL)
		{
			GP_IRQ_Callbacks[10]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 10);

	}

	if (EXTI -> PR & (1 << 11))  // Check if the pending bit of line 11 is set
	{
		if (GP_IRQ_Callbacks[11] != NULL)
		{
			GP_IRQ_Callbacks[11]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 11);

	}

	if (EXTI -> PR & (1 << 12))  // Check if the pending bit of line 12 is set
	{
		if (GP_IRQ_Callbacks[12] != NULL)
		{
			GP_IRQ_Callbacks[12]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 12);

	}

	if (EXTI -> PR & (1 << 13))  // Check if the pending bit of line 13 is set
	{
		if (GP_IRQ_Callbacks[13] != NULL)
		{
			GP_IRQ_Callbacks[13]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 13);

	}

	if (EXTI -> PR & (1 << 14))  // Check if the pending bit of line 14 is set
	{
		if (GP_IRQ_Callbacks[14] != NULL)
		{
			GP_IRQ_Callbacks[14]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 14);

	}
	if (EXTI -> PR & (1 << 15))  // Check if the pending bit of line 15 is set
	{
		if (GP_IRQ_Callbacks[15] != NULL)
		{
			GP_IRQ_Callbacks[15]();  // Calling the func @ main
		}
		EXTI->PR = (1UL << 15);

	}
}


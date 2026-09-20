/*
 *	Stm32F103C6_RCC_driver.h
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 17 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

#ifndef INC_STM32_F103C6_RCC_DRIVER_H_
#define INC_STM32_F103C6_RCC_DRIVER_H_

//include
#include "Stm32_F103C6_gpio_driver.h"
#include "stm32f103x6.h"

#define HSI_RC_Clk			(uint32_t)16000000
#define HSE_Clk		(uint32_t)8000000

uint32_t MCAL_RCC_GetSYS_CLCKFreq(void);

uint32_t MCAL_RCC_GetHCLKFreq(void);
uint32_t MCAL_RCC_GetPCLK2Freq(void);

uint32_t MCAL_RCC_GetPCLK1Freq(void);

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
// Macros Configuration References:
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//Refernce Macros


#endif /* STM32_F103C6_RCC_DRIVER_H_ */

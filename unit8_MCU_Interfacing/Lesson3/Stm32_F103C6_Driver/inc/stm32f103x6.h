/*
 *	stm32f103x6.h
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 17 2026
 *	Defination 	: MCU Device Header
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

#ifndef INC_STM32F103X6_H_
#define INC_STM32F103X6_H_

//-----------------------------
//Includes
//-----------------------------

#include <stdint.h>
#include <stdlib.h>

//-----------------------------
//Base addresses for Memories
//-----------------------------

#define FLASH_Memory_BASE						0x080000000UL
#define System_Memory_BASE						0x1FFFFF000UL
#define SRAM_Memory_BASE						0x200000000UL

#define Peripheral_BASE							0x400000000UL
#define Cortex_M3_Internal_Peripheral_BASE		0xE00000000UL

//-----------------------------
//Base addresses for AHB Peripherals
//-----------------------------

#define RCC_BASE_ADDRESS						0x40021000UL

#define NVIC_BASE_ADDRESS    					0xE000E100UL


//-----------------------------
//Base addresses for APB2 Peripherals
//-----------------------------

//GPIO
//A, B fully included in LQFP48 Package
#define GPIOA_BASE_ADDRESS						0x40010800UL
#define GPIOB_BASE_ADDRESS						0x40010C00UL

//C, D partial included in LQFP48 Package
#define GPIOC_BASE_ADDRESS						0x40011000UL
#define GPIOD_BASE_ADDRESS						0x40011400UL

//E not included in LQFP48 Package
#define GPIOE_BASE_ADDRESS						0x40011800UL

//EXTI
#define EXTI_BASE_ADDRESS						0x40010400UL

//AFIO
#define AFIO_BASE_ADDRESS						0x40010000UL

//USART1
#define USART1_BASE_ADDRESS						0x40013800UL

//-----------------------------
//Base addresses for APB1 Peripherals
//-----------------------------

//USAR2
#define USART2_BASE_ADDRESS						0x40004400UL
//USAR3
#define USART3_BASE_ADDRESS						0x40004800UL

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register:
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral NVIC register:
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
    volatile uint32_t ISER[8];       /* 0x000 - 0x01C */
    uint32_t RESERVED0[24];           /* 0x020 - 0x07C */
    volatile uint32_t ICER[8];       /* 0x080 - 0x09C */
    uint32_t RESERVED1[24];           /* 0x0A0 - 0x0FC */
    volatile uint32_t ISPR[8];       /* 0x100 - 0x11C */
    uint32_t RESERVED2[24];           /* 0x120 - 0x17C */
    volatile uint32_t ICPR[8];       /* 0x180 - 0x19C */
    uint32_t RESERVED3[24];           /* 0x1A0 - 0x1FC */
    volatile uint32_t IABR[8];       /* 0x200 - 0x21C */
    uint32_t RESERVED4[56];           /* 0x220 - 0x2FC */
    volatile uint8_t IPR[240];       /* 0x300 - 0x3EF */

} NVIC_t;

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral GPIO register:
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct {
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;
} GPIO_t;


//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral RCC register:
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct{
	volatile uint32_t CR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB1RSTR;
	volatile uint32_t AHBENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB1ENR;
	volatile uint32_t BDCR;
}RCC_t;

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral EXTI register:
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct{
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
}EXTI_t;

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral AFIO register:
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
    volatile uint32_t RESERVED;
    volatile uint32_t MAPR2;

} AFIO_t;

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral USART register:
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;

} USART_t;

//-*-*-*-*-*-*-*-*-*-*-
//Peripheral Instants:
//-*-*-*-*-*-*-*-*-*-*-*

#define NVIC    			((NVIC_t *)NVIC_BASE_ADDRESS)


#define GPIOA 				((volatile GPIO_t *)GPIOA_BASE_ADDRESS)
#define GPIOB 				((volatile GPIO_t *)GPIOB_BASE_ADDRESS)
#define GPIOC 				((volatile GPIO_t *)GPIOC_BASE_ADDRESS)
#define GPIOD 				((volatile GPIO_t *)GPIOD_BASE_ADDRESS)
#define GPIOE 				((volatile GPIO_t *)GPIOE_BASE_ADDRESS)

#define EXTI 				((volatile EXTI_t*)EXTI_BASE_ADDRESS)
#define RCC 				((volatile RCC_t*)RCC_BASE_ADDRESS)
#define AFIO				((volatile AFIO_t *)AFIO_BASE_ADDRESS)

#define USART1				((volatile USART_t *)USART1_BASE_ADDRESS)
#define USART2				((volatile USART_t *)USART2_BASE_ADDRESS)
#define USART3				((volatile USART_t *)USART3_BASE_ADDRESS)


//-*-*-*-*-*-*-*-*-*-*-*-
//Generic Macros:
//-*-*-*-*-*-*-*-*-*-*-*


//-*-*-*-*-*-*-*-*-*-*-*-*
//IVT
//-*-*-*-*-*-*-*-*-*-*-*-*-
//EXTI

#define EXTI0IRQ     6
#define EXTI1IRQ     7
#define EXTI2IRQ     8
#define EXTI3IRQ     9
#define EXTI4IRQ    10

#define EXTI5IRQ     23
#define EXTI6IRQ     23
#define EXTI7IRQ     23
#define EXTI8IRQ     23
#define EXTI9IRQ     23

#define EXTI10IRQ    40
#define EXTI11IRQ    40
#define EXTI12IRQ    40
#define EXTI13IRQ    40
#define EXTI14IRQ    40
#define EXTI15IRQ    40


//USART
#define USART1IRQ    37
#define USART2IRQ    38
#define USART3IRQ    39


//-*-*-*-*-*-*-*-*-*-*-*-
//clock enable Macros:
//-*-*-*-*-*-*-*-*-*-*-*

#define RCC_GPIOA_CLK_EN()	(RCC->APB2ENR |= 1<<2)
#define RCC_GPIOB_CLK_EN()	(RCC->APB2ENR |= 1<<3)
#define RCC_GPIOC_CLK_EN()	(RCC->APB2ENR |= 1<<4)
#define RCC_GPIOD_CLK_EN()	(RCC->APB2ENR |= 1<<5)
#define RCC_GPIOE_CLK_EN()	(RCC->APB2ENR |= 1<<6)

#define RCC_AFIO_CLK_EN()	(RCC->APB2ENR |= 1<<0)

//clock Enable USART
#define RCC_USART1_CLK_EN()	(RCC->APB2ENR |= 1<<14)
#define RCC_USART2_CLK_EN()	(RCC->APB1ENR |= 1<<17)
#define RCC_USART3_CLK_EN()	(RCC->APB1ENR |= 1<<18)

//clock Reset USART
#define RCC_USART1_CLK_Reset()	(RCC->APB2RSTR |= 1<<14)
#define RCC_USART2_CLK_Reset()	(RCC->APB1RSTR |= 1<<17)
#define RCC_USART3_CLK_Reset()	(RCC->APB1RSTR |= 1<<18)

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//NVIC IRQ Enable/Disable Macros:
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
#define NVIC_IRQ6_EXTI0_Enable 				(NVIC->ISER[0] |=1<<6)
#define NVIC_IRQ7_EXTI1_Enable 				(NVIC->ISER[0] |=1<<7)
#define NVIC_IRQ8_EXTI2_Enable 				(NVIC->ISER[0] |=1<<8)
#define NVIC_IRQ9_EXTI3_Enable 				(NVIC->ISER[0] |=1<<9)
#define NVIC_IRQ10_EXTI4_Enable 			(NVIC->ISER[0] |=1<<10)
#define NVIC_IRQ23_EXTI5_9_Enable 			(NVIC->ISER[0] |=1<<23)
#define NVIC_IRQ40_EXTI10_15_Enable 		(NVIC->ISER[1] |=1<<8)


#define NVIC_IRQ6_EXTI0_Disable 				(NVIC->ICER[0] |=1<<6)
#define NVIC_IRQ7_EXTI1_Disable 				(NVIC->ICER[0] |=1<<7)
#define NVIC_IRQ8_EXTI2_Disable 				(NVIC->ICER[0] |=1<<8)
#define NVIC_IRQ9_EXTI3_Disable 				(NVIC->ICER[0] |=1<<9)
#define NVIC_IRQ10_EXTI4_Disable 				(NVIC->ICER[0] |=1<<10)
#define NVIC_IRQ23_EXTI5_9_Disable 				(NVIC->ICER[0] |=1<<23)
#define NVIC_IRQ40_EXTI10_15_Disable 			(NVIC->ICER[1] |=1<<8)

//USART
#define NVIC_IRQ37_USART1_Enable 			(NVIC->ISER[1] |=(1<<(USART1IRQ-32)))
#define NVIC_IRQ38_USART2_Enable 			(NVIC->ISER[1] |=(1<<(USART2IRQ-32)))
#define NVIC_IRQ39_USART3_Enable 			(NVIC->ISER[1] |=(1<<(USART3IRQ-32)))

#define NVIC_IRQ37_USART1_Disable 			(NVIC->ICER[1] |=(1<<(USART1IRQ-32)))
#define NVIC_IRQ38_USART2_Disable 			(NVIC->ICER[1] |=(1<<(USART2IRQ-32)))
#define NVIC_IRQ39_USART3_Disable 			(NVIC->ICER[1] |=(1<<(USART3IRQ-32)))

#endif /*	INC_STM32F103X6_H_	*/

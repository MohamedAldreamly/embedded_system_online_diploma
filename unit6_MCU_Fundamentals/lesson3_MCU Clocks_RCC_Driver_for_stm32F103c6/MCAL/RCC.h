/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    RCC.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 14 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 

#ifndef RCC_H_
#define RCC_H_

#include "BIT_MATH.h"
#include "STD_TYPES.h"

typedef enum{
	RCC_DONE,
	RCC_ERROR
}RCC_STATE_t;

/* Register Definitions  */

typedef struct{
	volatile uint32 RCC_CR;
	volatile uint32 RCC_CFGR;
	volatile uint32 RCC_CIR;
	volatile uint32 RCC_APB2RSTR;
	volatile uint32 RCC_APB1RSTR;
	volatile uint32 RCC_AHBENR;
	volatile uint32 RCC_APB2ENR;
	volatile uint32 RCC_APB1ENR;
	volatile uint32 RCC_BDCR;
}RCC_t;

#define RCC_BASE_ADDRESS  0x40021000UL
#define RCC ((volatile RCC_t*)RCC_BASE_ADDRESS)

//APIs definations

RCC_STATE_t RCC_enumiInit();
RCC_STATE_t RCC_enumEnablePeripheralClock	( uint8 Copy_u8BusId , uint8 Copy_u8PeripheralId );
RCC_STATE_t RCC_enumResetPeripheralClock	(uint8 Copy_u8BusId, uint8 Copy_u8PeripheralId);
RCC_STATE_t RCC_enumDisablePeripheralClock			( uint8 Copy_u8BusId , uint8 Copy_u8PeripheralId );


//==============================================================================================================
/*	Configuration Clock System	*/

#define RCC_CLOCK_TYPE      	RCC_PLL
#define RCC_PLL_SOURCE      	RCC_PLL_HSI_DIVIDED_BY_2
#define RCC_PLL_MUL          	PLL_CLOCK_MULTIPLE_BY_8
#define RCC_AHB_PRESCALER    	SYSCLK_NOT_DIVIDED
#define RCC_APB1_PRESCALER   	HCLK_DIVIDED_BY_2
#define RCC_APB2_PRESCALER   	HCLK_DIVIDED_BY_4

//==============================================================================================================

/*==========	RCC_CLOCK_TYPE	Configuration	===========*/

#define RCC_HSI         0
#define RCC_HSE         1
#define RCC_PLL         2
#define RCC_HSE_BYPASS  3


#define RCC_SW_HSI 0
#define RCC_SW_HSE 1
#define RCC_SW_PLL 2
/*	RCC_PLL_SOURCE	Configuration*/

#define RCC_PLL_HSI_DIVIDED_BY_2  0
#define RCC_PLL_HSE               1
#define RCC_PLL_HSE_DIVIDED_BY_2  2

/*	RCC_PLL_MUL	Configuration*/

#define NO_CLOCK_FACTOR                                0b0000
#define PLL_CLOCK_MULTIPLE_BY_2                        0b0000
#define PLL_CLOCK_MULTIPLE_BY_3					       0b0001
#define PLL_CLOCK_MULTIPLE_BY_4                        0b0010
#define PLL_CLOCK_MULTIPLE_BY_5                        0b0011
#define PLL_CLOCK_MULTIPLE_BY_6                        0b0100
#define PLL_CLOCK_MULTIPLE_BY_7                        0b0101
#define PLL_CLOCK_MULTIPLE_BY_8                        0b0110
#define PLL_CLOCK_MULTIPLE_BY_9                        0b0111
#define PLL_CLOCK_MULTIPLE_BY_10                       0b1000
#define PLL_CLOCK_MULTIPLE_BY_11                       0b1001
#define PLL_CLOCK_MULTIPLE_BY_12                       0b1010
#define PLL_CLOCK_MULTIPLE_BY_13                       0b1011
#define PLL_CLOCK_MULTIPLE_BY_14                       0b1100
#define PLL_CLOCK_MULTIPLE_BY_15                       0b1101
#define PLL_CLOCK_MULTIPLE_BY_16                       0b1110

/*	RCC_AHB_PRESCALER	Configuration	*/

#define SYSCLK_NOT_DIVIDED          0b0000
#define SYSCLK_DIVIDED_BY_2         0b1000
#define SYSCLK_DIVIDED_BY_4			0b1001
#define SYSCLK_DIVIDED_BY_8         0b1010
#define SYSCLK_DIVIDED_BY_16        0b1011
#define SYSCLK_DIVIDED_BY_64        0b1100
#define SYSCLK_DIVIDED_BY_128		0b1101
#define SYSCLK_DIVIDED_BY_256       0b1110
#define SYSCLK_DIVIDED_BY_512       0b1111


/* 	RCC_APB1&2_PRESCALER Configuration	*/
#define HCLK_NOT_DIVIDED                         0b000
#define HCLK_DIVIDED_BY_2                        0b100
#define HCLK_DIVIDED_BY_4					     0b101
#define HCLK_DIVIDED_BY_8                        0b110
#define HCLK_DIVIDED_BY_16                       0b111

//==============================================================================================================

/*======	RCC Registers Bits	========*/ 

/*		RCC_CR Registers Bits		*/

#define RCC_HSION                        0
#define RCC_HSIRDY                       1
#define RCC_HSEON                        16
#define RCC_HSERDY                       17
#define RCC_HSEBYP                       18
#define RCC_CSSON                        19
#define RCC_PLLON                        24
#define RCC_PLLRDY                       25 

/*		RCC_CFGR Registers Bits		*/

#define RCC_SW_POSITION  0
#define RCC_SWS_POSITION 2
#define RCC_HPRE_POSITION 4
#define RCC_PPRES1_POSITION 8
#define RCC_PPRES2_POSITION 11
#define RCC_ADCPRE_POSITION 14
#define RCC_PLLSRC_POSITION 16
#define RCC_PLLXTPRE_POSITION 17
#define RCC_PLLMUL_POSITION 18

//==============================================================================================================

/*		Peripheral Clock Enable For AHB Bus		*/

#define    DMA1_RCC         0
#define    DMA2_RCC         1
#define    SRAM_RCC         2
#define    FLITF_RCC        4
#define    CRC_RCC          6
#define    FSMC_RCC         8
#define    SGPOI_RCC         10

/*		Peripheral Clock Enable For APB2 Bus		*/

#define    AFIO_RCC         0
#define    GPOIA_RCC        2
#define    GPOIB_RCC        3
#define    GPOIC_RCC        4
#define    GPOID_RCC        5
#define    GPOIE_RCC        6
#define    GPOIF_RCC        7
#define    GPOIG_RCC        8
#define    ADC1_RCC         9
#define    ADC2_RCC         10
#define    TIM1_RCC         11
#define    SPI1_RCC         12
#define    TIM8_RCC         13
#define    USART1_RCC       14
#define    ADC3_RCC         15
#define    TIM9_RCC         19
#define    TIM10_RCC        20
#define    TIM11_RCC        21

/*		Peripheral Clock Enable For APB1 Bus		*/

#define    TIM2_RCC         0
#define    TIM3_RCC         1
#define    TIM4_RCC         2
#define    TIM5_RCC         3
#define    TIM6_RCC         4
#define    TIM7_RCC         5
#define    TIM12_RCC        6
#define    TIM13_RCC        7
#define    TIM14_RCC        8
#define    WWDG_RCC         11
#define    SPI2_RCC         14
#define    SPI3_RCC         15
#define    USART2_RCC       17
#define    USART3_RCC       18
#define    UART4_RCC        19
#define    UART5_RCC        20
#define    I2C1_RCC         21
#define    I2C2_RCC         22
#define    USB_RCC          23
#define    CAN_RCC          25
#define    BKP_RCC          27
#define    PWR_RCC          28
#define    DAC_RCC          29

/*======	RCC BUS		========*/ 

#define    AHB_BUS       0
#define    APB1_BUS      1
#define    APB2_BUS      2



#endif /*	RCC_H_	*/
 
 

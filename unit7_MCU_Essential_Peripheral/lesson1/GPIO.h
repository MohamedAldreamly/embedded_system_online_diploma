/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    GPIO.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 

#ifndef GPIO_H_
#define GPIO_H_

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "RCC.h"


/* GPIOs Base Address */
#define GPIOA_BASE_ADDRESS 		0x40010800UL
#define GPIOB_BASE_ADDRESS 		0x40010C00UL
#define GPIOC_BASE_ADDRESS 		0x40011000UL
#define GPIOD_BASE_ADDRESS 		0x40011400UL

typedef struct {
	volatile uint32 CRL;
	volatile uint32 CRH;
	volatile uint32 IDR;
	volatile uint32 ODR;
	volatile uint32 BSRR;
	volatile uint32 BRR;
	volatile uint32 LCKR;
} GPIO_t;

#define GPIOA ((volatile GPIO_t *)GPIOA_BASE_ADDRESS)
#define GPIOB ((volatile GPIO_t *)GPIOB_BASE_ADDRESS)
#define GPIOC ((volatile GPIO_t *)GPIOC_BASE_ADDRESS)
#define GPIOD ((volatile GPIO_t *)GPIOD_BASE_ADDRESS)

/*	 In input mode (MODE[1:0]=00)	 */
#define GPIO_CNF_INPUT_ANALOG           0b00
#define GPIO_CNF_INPUT_FLOATING         0b01
#define GPIO_CNF_INPUT_PULL_UP_DOWN     0b10
 
/*	In output mode (MODE[1:0] >00)		*/
 
#define GPIO_CNF_OUTPUT_PUSH_PULL       0b00
#define GPIO_CNF_OUTPUT_OPEN_DRAIN      0b01
#define GPIO_CNF_AF_PUSH_PULL           0b10
#define GPIO_CNF_AF_OPEN_DRAIN          0b11


 /*	 MODEy[1:0]: Port x mode bits	*/

#define GPIO_MODE_INPUT          		0b00
#define GPIO_MODE_OUTPUT_10MHZ   		0b01
#define GPIO_MODE_OUTPUT_2MHZ    		0b10
#define GPIO_MODE_OUTPUT_50MHZ   		0b11

#define GPIO_INPUT_PULLUP 1
#define GPIO_INPUT_PULLDOWN 0


#define PIN0 	0b0000
#define PIN1 	0b0001
#define PIN2 	0b0010
#define PIN3 	0b0011
#define PIN4 	0b0100
#define PIN5 	0b0101
#define PIN6 	0b0110
#define PIN7 	0b0111
#define PIN8 	0b1000
#define PIN9 	0b1001
#define PIN10 	0b1010
#define PIN11 	0b1011
#define PIN12 	0b1100
#define PIN13 	0b1101
#define PIN14 	0b1110
#define PIN15 	0b1111

#define GPIO_PIN_LOW 	0
#define GPIO_PIN_HIGH 	1

void GPIO_voidInit			(void);
void GPIO_voidSetMode		(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8PortMode, uint8 Copy_u8Mode);
uint8 GPIO_u8ReadPin		(volatile GPIO_t *Copy_Port, uint8 Copy_u8Pin);
void GPIO_voidWritePin		(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8Value);
void GPIO_voidSetPullMode	(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin, uint8 Copy_u8PullMode);
void GPIO_voidResetPin		(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin);
void GPIO_voidSetPin		(volatile GPIO_t* Copy_Port, uint8 Copy_u8Pin);


#endif /*	GPIO_H_		*/

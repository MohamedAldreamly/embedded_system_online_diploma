/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    main.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *	Date	: SEP, 15 2026
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
 //Learn-in-depth
 
#include "RCC.h"
#include "EXTI.h"
#include "NVIC.h"


/* GPIOA Registers */
#define GPIOA_BASE      0x40010800

#define GPIOA_CRL       (*(volatile uint32 *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH       (*(volatile uint32 *)(GPIOA_BASE + 0x04))
#define GPIOA_IDR       (*(volatile uint32 *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32 *)(GPIOA_BASE + 0x0C))


/*==========================================================
 * EXTI0 Callback
 * Every interrupt toggles PA1
 *==========================================================*/
void GPIO_TOGGLE_LED(void)
{
    GPIOA_ODR ^= (1U << 1);
}


int main(void)
{
    /*================ RCC =================*/

    RCC_enumiInit();

    /* Enable GPIOA Clock */
    RCC_enumEnablePeripheralClock(APB2_BUS, GPOIA_RCC);


    /*======================================================
     * PA1 -> OUTPUT
     *
     * CRL bits for PA1 = bits 7:4
     *
     * MODE1 = 10  -> Output 2 MHz
     * CNF1  = 00  -> General Purpose Push-Pull
     *
     * 0010 = 0x2
     *======================================================*/

    GPIOA_CRL &= ~(0xFU << 4);
    GPIOA_CRL |=  (0x2U << 4);


    /*======================================================
     * PA0 -> INPUT Pull-Down
     *
     * CRL bits for PA0 = bits 3:0
     *
     * MODE0 = 00
     * CNF0  = 10
     *
     * 1000 = 0x8
     *======================================================*/

    GPIOA_CRL &= ~(0xFU << 0);
    GPIOA_CRL |=  (0x8U << 0);

    /*
     * Input Pull-Up/Pull-Down mode:
     *
     * ODR0 = 0 -> Pull-Down
     * ODR0 = 1 -> Pull-Up
     */

    GPIOA_ODR &= ~(1U << 0);


    /*======================================================
     * EXTI0 Initialization
     *
     * EXTI Line : 0
     * Port      : GPIOA
     * Pin       : PA0
     * Trigger   : Rising Edge
     * Callback  : GPIO_TOGGLE_LED
     *======================================================*/

    EXTI_enumInit(
        EXTI_LINE_0,
        EXTI_PORTA,
        Rising_Edge,
        GPIO_TOGGLE_LED
    );


    while (1)
    {

    }
}


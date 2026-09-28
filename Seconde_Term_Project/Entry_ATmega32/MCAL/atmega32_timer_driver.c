/******************************************************************************
 * @file        atmega32_timer_driver.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      Timer0
 *
 * @path        Entry_ATmega32/MCAL/Src/atmega32_timer_driver.c
 *
 * @brief
 * Timer0 implementation providing a 1 ms system time base.
 ******************************************************************************/

#include "atmega32_timer_driver.h"

#include <avr/io.h>
#include <avr/interrupt.h>


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static volatile uint32_t Global_Milliseconds = 0UL;


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void MCAL_Timer0_Init(void)
{
    Global_Milliseconds = 0UL;


    /*
     * Timer0 CTC Mode.
     *
     * WGM01 = 1
     * WGM00 = 0
     */
    TCCR0 = (1U << WGM01);


    /*
     * 8 MHz / 64 = 125 kHz
     *
     * 125 timer counts = 1 ms
     *
     * OCR0 = 125 - 1 = 124
     */
    OCR0 = 124U;


    /*
     * Enable Timer0 Compare Match Interrupt.
     */
    TIMSK |= (1U << OCIE0);


    /*
     * Start Timer0 with prescaler = 64.
     *
     * CS01 = 1
     * CS00 = 1
     */
    TCCR0 |=
        (1U << CS01) |
        (1U << CS00);
}


uint32_t MCAL_Timer0_GetMillis(void)
{
    uint32_t Local_Time;

    uint8_t Local_SREG;


    /*
     * uint32_t access is not atomic on an
     * 8-bit AVR, so protect the read.
     */
    Local_SREG = SREG;

    cli();

    Local_Time = Global_Milliseconds;

    SREG = Local_SREG;


    return Local_Time;
}


/******************************************************************************
 * Timer0 Compare Match ISR
 ******************************************************************************/

ISR(TIMER0_COMP_vect)
{
    Global_Milliseconds++;
}
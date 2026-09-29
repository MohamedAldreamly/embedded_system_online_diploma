/******************************************************************************
 * @file        main.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       Application
 * @module      Main
 *
 * @path        Entry_ATmega32/App/main.c
 *
 * @brief
 * Entry ECU system entry point and cooperative scheduler.
 *
 * @responsibility
 * - Initialize the Entry ECU application.
 * - Enable global interrupts.
 * - Execute the Entry ECU state machine continuously.
 ******************************************************************************/

#include "entry_app.h"

#include <avr/interrupt.h>


int main(void)
{
    /*
     * Initialize:
     * - UART communication
     * - Keypad
     * - RFID
     * - Gate
     * - IR sensor
     * - LCD
     * - Timer0
     * - Entry state machine
     */
    EntryApp_Init();


    /*
     * Required for:
     * - UART RX interrupt
     * - Timer0 1 ms interrupt
     */
    sei();


    while (1)
    {
        EntryApp_Update();
    }


    return 0;
}
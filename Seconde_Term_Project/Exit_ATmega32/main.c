/******************************************************************************
 * @file        main.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Exit ECU
 * @mcu         ATmega32
 * @layer       Application
 * @module      Main
 *
 * @path        Exit_ATmega32/App/main.c
 *
 * @brief
 * Exit ECU system entry point and cooperative scheduler.
 *
 * @responsibility
 * - Initialize the Exit ECU application.
 * - Enable global interrupts.
 * - Execute the Exit ECU state machine continuously.
 ******************************************************************************/

#include "exit_app.h"

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
     * - Exit state machine
     */
    ExitApp_Init();


    /*
     * Required for:
     * - UART RX interrupt
     * - Timer0 1 ms interrupt
     */
    sei();


    while (1)
    {
        ExitApp_Update();
    }


    return 0;
}
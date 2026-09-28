/******************************************************************************
 * @file        main.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       Application
 *
 * @path        Entry_ATmega32/main.c
 *
 * @brief
 * Main entry point for the Smart Parking System Entry ECU.
 ******************************************************************************/

#include "entry_app.h"

#include <avr/interrupt.h>


int main(void)
{
    /*
     * Initialize the Entry application and
     * all required modules.
     */
    EntryApp_Init();


    /*
     * Enable global interrupts.
     *
     * UART reception depends on the
     * USART RX Complete interrupt.
     */
    sei();


    while (1)
    {
        /*
         * Cooperative application scheduler.
         */
        EntryApp_Update();
    }


    return 0;
}
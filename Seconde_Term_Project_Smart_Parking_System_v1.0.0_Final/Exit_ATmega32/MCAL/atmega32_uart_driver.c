/******************************************************************************
 * @file        atmega32_uart_driver.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      UART
 *
 * @brief
 * UART driver implementation for the ATmega32 Entry and Exit ECUs.
 *
 * UART configuration used by the Smart Parking System:
 * - CPU Clock : 8 MHz Internal RC
 * - Mode      : Asynchronous
 * - Data      : 8 bits
 * - Parity    : None
 * - Stop bits : 1
 * - U2X       : Enabled
 ******************************************************************************/

#include "atmega32_uart_driver.h"

#include <avr/io.h>
#include <avr/interrupt.h>


/******************************************************************************
 * Private Configuration
 ******************************************************************************/

#define F_CPU_HZ    8000000UL


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static UART_Config_t *Global_UART_Config = 0;


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void MCAL_UART_Init(UART_Config_t *UART_Config)
{
    uint16_t Local_UBRR;


    if (UART_Config == 0)
    {
        return;
    }


    Global_UART_Config = UART_Config;


    /*
     * Enable Double Speed mode.
     *
     * Baud Rate:
     *
     * UBRR = (F_CPU / (8 * BaudRate)) - 1
     */
    UCSRA |= (1U << U2X);


    Local_UBRR =
        (uint16_t)
        ((F_CPU_HZ / (8UL * UART_Config->BaudRate)) - 1UL);


    UBRRH = (uint8_t)(Local_UBRR >> 8);

    UBRRL = (uint8_t)Local_UBRR;


    /*
     * Enable Receiver and Transmitter.
     */
    UCSRB =
        (1U << RXEN) |
        (1U << TXEN);


    /*
     * Enable RX Complete Interrupt only when
     * a callback function is provided.
     */
    if (UART_Config->P_IRQ_CallBack != 0)
    {
        UCSRB |= (1U << RXCIE);
    }


    /*
     * UART Frame:
     *
     * Asynchronous
     * No parity
     * 1 stop bit
     * 8 data bits
     *
     * URSEL must be written as 1 when accessing UCSRC
     * on ATmega32.
     */
    UCSRC =
        (1U << URSEL) |
        (1U << UCSZ1) |
        (1U << UCSZ0);
}


void MCAL_UART_SendData(uint8_t data)
{
    /*
     * Wait until transmit buffer becomes empty.
     */
    while ((UCSRA & (1U << UDRE)) == 0U)
    {
        /* Wait */
    }


    UDR = data;
}


uint8_t MCAL_UART_ReceiveData(void)
{
    /*
     * Wait until data is received.
     */
    while ((UCSRA & (1U << RXC)) == 0U)
    {
        /* Wait */
    }


    return UDR;
}


/******************************************************************************
 * UART RX Interrupt
 ******************************************************************************/

ISR(USART_RXC_vect)
{
    if ((Global_UART_Config != 0) &&
        (Global_UART_Config->P_IRQ_CallBack != 0))
    {
        Global_UART_Config->P_IRQ_CallBack();
    }
}
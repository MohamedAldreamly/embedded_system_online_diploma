/******************************************************************************
 * @file        atmega32_soft_uart_rx.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      Software UART RX
 *
 * @path        Entry_ATmega32/MCAL/Src/atmega32_soft_uart_rx.c
 *
 * @brief
 * Minimal software UART RX implementation for the RFID reader.
 *
 * @note
 * This module is intentionally RX-only. The hardware USART remains
 * dedicated to communication with the STM32 Central ECU.
 ******************************************************************************/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "atmega32_soft_uart_rx.h"
#include "atmega32_gpio_driver.h"

#include <util/delay.h>
#include <stddef.h>


/******************************************************************************
 * Configuration
 ******************************************************************************/

#define SOFT_UART_RX_PORT          GPIO_PORT_D
#define SOFT_UART_RX_PIN           GPIO_PIN_2

#define SOFT_UART_BIT_TIME_US      104U
#define SOFT_UART_HALF_BIT_US      52U


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void MCAL_SoftUART_RX_Init(void)
{
    /*
     * UART line is HIGH while idle.
     */
    MCAL_GPIO_InitPin(SOFT_UART_RX_PORT,
                      SOFT_UART_RX_PIN,
                      GPIO_MODE_INPUT_PULLUP);
}


bool MCAL_SoftUART_RX_ReadByte(uint8_t *data)
{
    uint8_t Local_Bit;

    uint8_t Local_Data = 0U;


    if (data == NULL)
    {
        return false;
    }


    /*
     * No start bit detected.
     */
    if (MCAL_GPIO_ReadPin(SOFT_UART_RX_PORT,
                          SOFT_UART_RX_PIN) == GPIO_HIGH)
    {
        return false;
    }


    /*
     * Move to approximately the middle
     * of the start bit.
     */
    _delay_us(SOFT_UART_HALF_BIT_US);


    /*
     * Validate start bit.
     */
    if (MCAL_GPIO_ReadPin(SOFT_UART_RX_PORT,
                          SOFT_UART_RX_PIN) != GPIO_LOW)
    {
        return false;
    }


    /*
     * Move to the middle of data bit 0.
     */
    _delay_us(SOFT_UART_BIT_TIME_US);


    /*
     * Receive 8 data bits.
     * UART transfers LSB first.
     */
    for (Local_Bit = 0U;
         Local_Bit < 8U;
         Local_Bit++)
    {
        if (MCAL_GPIO_ReadPin(SOFT_UART_RX_PORT,
                              SOFT_UART_RX_PIN) == GPIO_HIGH)
        {
            Local_Data |= (1U << Local_Bit);
        }


        _delay_us(SOFT_UART_BIT_TIME_US);
    }


    /*
     * Stop bit must be HIGH.
     */
    if (MCAL_GPIO_ReadPin(SOFT_UART_RX_PORT,
                          SOFT_UART_RX_PIN) != GPIO_HIGH)
    {
        return false;
    }


    *data = Local_Data;


    return true;
}
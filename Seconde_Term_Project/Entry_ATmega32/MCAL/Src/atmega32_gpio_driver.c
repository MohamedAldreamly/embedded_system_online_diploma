/******************************************************************************
 * @file        atmega32_gpio_driver.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      GPIO
 *
 * @path        Entry_ATmega32/MCAL/Src/atmega32_gpio_driver.c
 *
 * @brief
 * GPIO driver implementation for ATmega32.
 ******************************************************************************/

#include "atmega32_gpio_driver.h"

#include <avr/io.h>
#include <stddef.h>


/******************************************************************************
 * Private Helpers
 ******************************************************************************/

static volatile uint8_t *GPIO_GetDDR(GPIO_Port_t port);

static volatile uint8_t *GPIO_GetPORT(GPIO_Port_t port);

static volatile uint8_t *GPIO_GetPIN(GPIO_Port_t port);


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void MCAL_GPIO_InitPin(GPIO_Port_t port,
                       uint8_t pin,
                       uint8_t mode)
{
    volatile uint8_t *Local_DDR;
    volatile uint8_t *Local_PORT;


    if (pin > GPIO_PIN_7)
    {
        return;
    }


    Local_DDR = GPIO_GetDDR(port);

    Local_PORT = GPIO_GetPORT(port);


    if ((Local_DDR == NULL) ||
        (Local_PORT == NULL))
    {
        return;
    }


    switch (mode)
    {
        case GPIO_MODE_OUTPUT:

            /*
             * DDR bit = 1
             * Pin configured as output.
             */
            *Local_DDR |= (1U << pin);

            break;


        case GPIO_MODE_INPUT_PULLUP:

            /*
             * DDR bit  = 0
             * PORT bit = 1
             *
             * Pin configured as input with
             * internal pull-up resistor enabled.
             */
            *Local_DDR &= ~(1U << pin);

            *Local_PORT |= (1U << pin);

            break;


        case GPIO_MODE_INPUT_FLOATING:

            /*
             * DDR bit  = 0
             * PORT bit = 0
             *
             * Pin configured as floating input.
             */
            *Local_DDR &= ~(1U << pin);

            *Local_PORT &= ~(1U << pin);

            break;


        default:

            break;
    }
}


void MCAL_GPIO_WritePin(GPIO_Port_t port,
                        uint8_t pin,
                        uint8_t value)
{
    volatile uint8_t *Local_PORT;


    if (pin > GPIO_PIN_7)
    {
        return;
    }


    Local_PORT = GPIO_GetPORT(port);


    if (Local_PORT == NULL)
    {
        return;
    }


    if (value == GPIO_HIGH)
    {
        *Local_PORT |= (1U << pin);
    }
    else
    {
        *Local_PORT &= ~(1U << pin);
    }
}


uint8_t MCAL_GPIO_ReadPin(GPIO_Port_t port,
                          uint8_t pin)
{
    volatile uint8_t *Local_PIN;


    if (pin > GPIO_PIN_7)
    {
        return GPIO_LOW;
    }


    Local_PIN = GPIO_GetPIN(port);


    if (Local_PIN == NULL)
    {
        return GPIO_LOW;
    }


    if ((*Local_PIN & (1U << pin)) != 0U)
    {
        return GPIO_HIGH;
    }


    return GPIO_LOW;
}


void MCAL_GPIO_TogglePin(GPIO_Port_t port,
                         uint8_t pin)
{
    volatile uint8_t *Local_PORT;


    if (pin > GPIO_PIN_7)
    {
        return;
    }


    Local_PORT = GPIO_GetPORT(port);


    if (Local_PORT == NULL)
    {
        return;
    }


    *Local_PORT ^= (1U << pin);
}


/******************************************************************************
 * Private Helpers
 ******************************************************************************/

static volatile uint8_t *GPIO_GetDDR(GPIO_Port_t port)
{
    switch (port)
    {
        case GPIO_PORT_A:
            return &DDRA;

        case GPIO_PORT_B:
            return &DDRB;

        case GPIO_PORT_C:
            return &DDRC;

        case GPIO_PORT_D:
            return &DDRD;

        default:
            return NULL;
    }
}


static volatile uint8_t *GPIO_GetPORT(GPIO_Port_t port)
{
    switch (port)
    {
        case GPIO_PORT_A:
            return &PORTA;

        case GPIO_PORT_B:
            return &PORTB;

        case GPIO_PORT_C:
            return &PORTC;

        case GPIO_PORT_D:
            return &PORTD;

        default:
            return NULL;
    }
}


static volatile uint8_t *GPIO_GetPIN(GPIO_Port_t port)
{
    switch (port)
    {
        case GPIO_PORT_A:
            return &PINA;

        case GPIO_PORT_B:
            return &PINB;

        case GPIO_PORT_C:
            return &PINC;

        case GPIO_PORT_D:
            return &PIND;

        default:
            return NULL;
    }
}
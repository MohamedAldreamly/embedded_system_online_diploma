/******************************************************************************
 * @file        atmega32_gpio_driver.h
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
 * @path        Entry_ATmega32/MCAL/Inc/atmega32_gpio_driver.h
 *
 * @brief
 * GPIO driver interface for ATmega32 digital input/output pins.
 ******************************************************************************/

#ifndef ATMEGA32_GPIO_DRIVER_H_
#define ATMEGA32_GPIO_DRIVER_H_

#include <stdint.h>


/******************************************************************************
 * GPIO Port Type
 ******************************************************************************/

typedef enum
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D

} GPIO_Port_t;


/******************************************************************************
 * GPIO Pin Numbers
 ******************************************************************************/

#define GPIO_PIN_0     0U
#define GPIO_PIN_1     1U
#define GPIO_PIN_2     2U
#define GPIO_PIN_3     3U
#define GPIO_PIN_4     4U
#define GPIO_PIN_5     5U
#define GPIO_PIN_6     6U
#define GPIO_PIN_7     7U


/******************************************************************************
 * GPIO Modes
 ******************************************************************************/

#define GPIO_MODE_INPUT_FLOATING     0U
#define GPIO_MODE_INPUT_PULLUP       1U
#define GPIO_MODE_OUTPUT             2U


/******************************************************************************
 * GPIO States
 ******************************************************************************/

#define GPIO_LOW       0U
#define GPIO_HIGH      1U


/******************************************************************************
 * Public APIs
 ******************************************************************************/

void MCAL_GPIO_InitPin(GPIO_Port_t port,
                       uint8_t pin,
                       uint8_t mode);

void MCAL_GPIO_WritePin(GPIO_Port_t port,
                        uint8_t pin,
                        uint8_t value);

uint8_t MCAL_GPIO_ReadPin(GPIO_Port_t port,
                          uint8_t pin);

void MCAL_GPIO_TogglePin(GPIO_Port_t port,
                         uint8_t pin);


#endif /* ATMEGA32_GPIO_DRIVER_H_ */
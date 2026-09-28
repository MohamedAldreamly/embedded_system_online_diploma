/******************************************************************************
 * @file        hal_ir.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       HAL
 * @module      IR Sensor
 *
 * @path        Entry_ATmega32/HAL/Src/hal_ir.c
 *
 * @brief
 * Entry gate IR sensor implementation.
 *
 * The sensor provides a digital signal used by the Entry ECU to
 * determine whether a vehicle is currently blocking the gate path.
 ******************************************************************************/

#include "hal_ir.h"
#include "atmega32_gpio_driver.h"


/******************************************************************************
 * IR Sensor Configuration
 ******************************************************************************/

#define IR_SENSOR_PORT        GPIO_PORT_D
#define IR_SENSOR_PIN         GPIO_PIN_5


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void HAL_IR_Init(void)
{
    MCAL_GPIO_InitPin(IR_SENSOR_PORT,
                      IR_SENSOR_PIN,
                      GPIO_MODE_INPUT_PULLUP);
}


bool HAL_IR_IsBlocked(void)
{
    /*
     * Active LOW:
     *
     * LOW  = Vehicle detected / path blocked
     * HIGH = Path clear
     */
    if (MCAL_GPIO_ReadPin(IR_SENSOR_PORT,
                          IR_SENSOR_PIN) == GPIO_LOW)
    {
        return true;
    }


    return false;
}
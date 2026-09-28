/******************************************************************************
 * @file        hal_gate.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       HAL
 * @module      Gate
 *
 * @path        Entry_ATmega32/HAL/Src/hal_gate.c
 *
 * @brief
 * Gate actuator implementation for the Entry ECU.
 *
 * The gate is controlled using two GPIO signals suitable for a
 * bidirectional motor driver such as an H-bridge.
 ******************************************************************************/

#include "hal_gate.h"
#include "atmega32_gpio_driver.h"


/******************************************************************************
 * Gate Configuration
 ******************************************************************************/

#define GATE_PORT          GPIO_PORT_D

#define GATE_MOTOR_IN1     GPIO_PIN_3
#define GATE_MOTOR_IN2     GPIO_PIN_4


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void HAL_Gate_Init(void)
{
    MCAL_GPIO_InitPin(GATE_PORT,
                      GATE_MOTOR_IN1,
                      GPIO_MODE_OUTPUT);

    MCAL_GPIO_InitPin(GATE_PORT,
                      GATE_MOTOR_IN2,
                      GPIO_MODE_OUTPUT);


    /*
     * Gate motor stopped initially.
     */
    HAL_Gate_Stop();
}


void HAL_Gate_Open(void)
{
    /*
     * Motor direction: OPEN.
     */
    MCAL_GPIO_WritePin(GATE_PORT,
                       GATE_MOTOR_IN1,
                       GPIO_HIGH);

    MCAL_GPIO_WritePin(GATE_PORT,
                       GATE_MOTOR_IN2,
                       GPIO_LOW);
}


void HAL_Gate_Close(void)
{
    /*
     * Motor direction: CLOSE.
     */
    MCAL_GPIO_WritePin(GATE_PORT,
                       GATE_MOTOR_IN1,
                       GPIO_LOW);

    MCAL_GPIO_WritePin(GATE_PORT,
                       GATE_MOTOR_IN2,
                       GPIO_HIGH);
}


void HAL_Gate_Stop(void)
{
    MCAL_GPIO_WritePin(GATE_PORT,
                       GATE_MOTOR_IN1,
                       GPIO_LOW);

    MCAL_GPIO_WritePin(GATE_PORT,
                       GATE_MOTOR_IN2,
                       GPIO_LOW);
}
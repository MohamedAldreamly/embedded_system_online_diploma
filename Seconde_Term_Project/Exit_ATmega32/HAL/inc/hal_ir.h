/******************************************************************************
 * @file        hal_ir.h
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
 * @path        Entry_ATmega32/HAL/Inc/hal_ir.h
 *
 * @brief
 * Entry gate IR sensor interface used to detect vehicle passage.
 ******************************************************************************/

#ifndef HAL_IR_H_
#define HAL_IR_H_

#include <stdbool.h>


void HAL_IR_Init(void);

bool HAL_IR_IsBlocked(void);


#endif /* HAL_IR_H_ */
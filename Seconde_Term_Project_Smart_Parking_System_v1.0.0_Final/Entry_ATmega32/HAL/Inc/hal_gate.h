/******************************************************************************
 * @file        hal_gate.h
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
 * @path        Entry_ATmega32/HAL/Inc/hal_gate.h
 *
 * @brief
 * Gate actuator interface for the Entry ECU.
 ******************************************************************************/

#ifndef HAL_GATE_H_
#define HAL_GATE_H_


void HAL_Gate_Init(void);

void HAL_Gate_Open(void);

void HAL_Gate_Close(void);

void HAL_Gate_Stop(void);


#endif /* HAL_GATE_H_ */
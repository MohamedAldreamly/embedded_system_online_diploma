/******************************************************************************
 * @file        atmega32_timer_driver.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      Timer0
 *
 * @path        Entry_ATmega32/MCAL/Inc/atmega32_timer_driver.h
 *
 * @brief
 * Provides a 1 ms system time base using Timer0 in CTC mode.
 ******************************************************************************/

#ifndef ATMEGA32_TIMER_DRIVER_H_
#define ATMEGA32_TIMER_DRIVER_H_

#include <stdint.h>


void MCAL_Timer0_Init(void);

uint32_t MCAL_Timer0_GetMillis(void);


#endif /* ATMEGA32_TIMER_DRIVER_H_ */
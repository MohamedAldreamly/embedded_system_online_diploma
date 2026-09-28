/******************************************************************************
 * @file        hal_rfid.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       HAL
 * @module      RFID
 *
 * @path        Entry_ATmega32/HAL/Inc/hal_rfid.h
 *
 * @brief
 * RFID reader interface used by the Entry ECU.
 ******************************************************************************/

#ifndef HAL_RFID_H_
#define HAL_RFID_H_

#include <stdint.h>
#include <stdbool.h>


void HAL_RFID_Init(void);

bool HAL_RFID_ReadUID(uint32_t *uid);


#endif /* HAL_RFID_H_ */
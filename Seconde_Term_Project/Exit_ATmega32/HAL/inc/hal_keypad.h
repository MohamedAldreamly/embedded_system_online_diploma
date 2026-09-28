/******************************************************************************
 * @file        hal_keypad.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       HAL
 * @module      Keypad
 *
 * @path        Entry_ATmega32/HAL/Inc/hal_keypad.h
 *
 * @brief
 * 4x4 matrix keypad interface used by the Entry ECU.
 ******************************************************************************/

#ifndef HAL_KEYPAD_H_
#define HAL_KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 * Key Definitions
 ******************************************************************************/

#define KEYPAD_KEY_NONE       '\0'
#define KEYPAD_KEY_CONFIRM    '#'
#define KEYPAD_KEY_CLEAR      '*'


/******************************************************************************
 * Public APIs
 ******************************************************************************/

void HAL_Keypad_Init(void);

char HAL_Keypad_GetKey(void);


#endif /* HAL_KEYPAD_H_ */
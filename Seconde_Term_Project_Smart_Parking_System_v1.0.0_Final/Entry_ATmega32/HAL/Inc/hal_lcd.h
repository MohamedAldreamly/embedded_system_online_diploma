/******************************************************************************
 * @file        hal_lcd.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       HAL
 * @module      LCD
 *
 * @path        Entry_ATmega32/HAL/Inc/hal_lcd.h
 *
 * @brief
 * 16x2 LCD interface for the Entry ECU.
 ******************************************************************************/

#ifndef HAL_LCD_H_
#define HAL_LCD_H_

#include <stdint.h>


void HAL_LCD_Init(void);

void HAL_LCD_Clear(void);

void HAL_LCD_SetCursor(uint8_t row,
                       uint8_t column);

void HAL_LCD_WriteChar(char character);

void HAL_LCD_WriteString(const char *string);

void HAL_LCD_WriteNumber(uint32_t number);


#endif /* HAL_LCD_H_ */
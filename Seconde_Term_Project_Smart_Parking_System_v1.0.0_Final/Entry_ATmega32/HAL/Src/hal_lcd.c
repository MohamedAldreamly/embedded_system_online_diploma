/******************************************************************************
 * @file        hal_lcd.c
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
 * @path        Entry_ATmega32/HAL/Src/hal_lcd.c
 *
 * @brief
 * Implements a 16x2 HD44780-compatible LCD in 4-bit mode.
 ******************************************************************************/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "hal_lcd.h"
#include "atmega32_gpio_driver.h"

#include <util/delay.h>
#include <stddef.h>


/******************************************************************************
 * LCD Configuration
 ******************************************************************************/

#define LCD_PORT            GPIO_PORT_A

#define LCD_RS_PIN          GPIO_PIN_0
#define LCD_EN_PIN          GPIO_PIN_1

#define LCD_D4_PIN          GPIO_PIN_2
#define LCD_D5_PIN          GPIO_PIN_3
#define LCD_D6_PIN          GPIO_PIN_4
#define LCD_D7_PIN          GPIO_PIN_5


/******************************************************************************
 * LCD Commands
 ******************************************************************************/

#define LCD_CMD_CLEAR           0x01U
#define LCD_CMD_HOME            0x02U
#define LCD_CMD_4BIT_2LINE      0x28U
#define LCD_CMD_DISPLAY_ON      0x0CU
#define LCD_CMD_ENTRY_MODE      0x06U


/******************************************************************************
 * Private Functions
 ******************************************************************************/

static void LCD_SendNibble(uint8_t nibble);

static void LCD_SendCommand(uint8_t command);

static void LCD_PulseEnable(void);


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void HAL_LCD_Init(void)
{
    MCAL_GPIO_InitPin(LCD_PORT,
                      LCD_RS_PIN,
                      GPIO_MODE_OUTPUT);

    MCAL_GPIO_InitPin(LCD_PORT,
                      LCD_EN_PIN,
                      GPIO_MODE_OUTPUT);

    MCAL_GPIO_InitPin(LCD_PORT,
                      LCD_D4_PIN,
                      GPIO_MODE_OUTPUT);

    MCAL_GPIO_InitPin(LCD_PORT,
                      LCD_D5_PIN,
                      GPIO_MODE_OUTPUT);

    MCAL_GPIO_InitPin(LCD_PORT,
                      LCD_D6_PIN,
                      GPIO_MODE_OUTPUT);

    MCAL_GPIO_InitPin(LCD_PORT,
                      LCD_D7_PIN,
                      GPIO_MODE_OUTPUT);


    MCAL_GPIO_WritePin(LCD_PORT,
                       LCD_RS_PIN,
                       GPIO_LOW);

    MCAL_GPIO_WritePin(LCD_PORT,
                       LCD_EN_PIN,
                       GPIO_LOW);


    /*
     * LCD power-up delay.
     */
    _delay_ms(20);


    /*
     * HD44780 initialization sequence.
     */
    LCD_SendNibble(0x03U);
    _delay_ms(5);

    LCD_SendNibble(0x03U);
    _delay_us(150);

    LCD_SendNibble(0x03U);

    LCD_SendNibble(0x02U);


    /*
     * 4-bit interface.
     * 2 display lines.
     */
    LCD_SendCommand(LCD_CMD_4BIT_2LINE);


    /*
     * Display ON.
     * Cursor OFF.
     * Blink OFF.
     */
    LCD_SendCommand(LCD_CMD_DISPLAY_ON);


    /*
     * Increment cursor automatically.
     */
    LCD_SendCommand(LCD_CMD_ENTRY_MODE);


    HAL_LCD_Clear();
}


void HAL_LCD_Clear(void)
{
    LCD_SendCommand(LCD_CMD_CLEAR);

    _delay_ms(2);
}


void HAL_LCD_SetCursor(uint8_t row,
                       uint8_t column)
{
    uint8_t Local_Address;


    if (column > 15U)
    {
        column = 15U;
    }


    if (row == 0U)
    {
        Local_Address = column;
    }
    else
    {
        Local_Address = 0x40U + column;
    }


    LCD_SendCommand(0x80U | Local_Address);
}


void HAL_LCD_WriteChar(char character)
{
    MCAL_GPIO_WritePin(LCD_PORT,
                       LCD_RS_PIN,
                       GPIO_HIGH);


    LCD_SendNibble(
        ((uint8_t)character >> 4U) & 0x0FU);


    LCD_SendNibble(
        (uint8_t)character & 0x0FU);
}


void HAL_LCD_WriteString(const char *string)
{
    if (string == NULL)
    {
        return;
    }


    while (*string != '\0')
    {
        HAL_LCD_WriteChar(*string);

        string++;
    }
}


void HAL_LCD_WriteNumber(uint32_t number)
{
    char Local_Buffer[11];

    uint8_t Local_Index = 0U;


    if (number == 0UL)
    {
        HAL_LCD_WriteChar('0');

        return;
    }


    while ((number > 0UL) &&
           (Local_Index < 10U))
    {
        Local_Buffer[Local_Index] =
            (char)('0' + (number % 10UL));

        number /= 10UL;

        Local_Index++;
    }


    while (Local_Index > 0U)
    {
        Local_Index--;

        HAL_LCD_WriteChar(
            Local_Buffer[Local_Index]);
    }
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

static void LCD_SendNibble(uint8_t nibble)
{
    MCAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D4_PIN,
        ((nibble & (1U << 0U)) != 0U) ?
        GPIO_HIGH : GPIO_LOW);


    MCAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D5_PIN,
        ((nibble & (1U << 1U)) != 0U) ?
        GPIO_HIGH : GPIO_LOW);


    MCAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D6_PIN,
        ((nibble & (1U << 2U)) != 0U) ?
        GPIO_HIGH : GPIO_LOW);


    MCAL_GPIO_WritePin(
        LCD_PORT,
        LCD_D7_PIN,
        ((nibble & (1U << 3U)) != 0U) ?
        GPIO_HIGH : GPIO_LOW);


    LCD_PulseEnable();
}


static void LCD_SendCommand(uint8_t command)
{
    MCAL_GPIO_WritePin(LCD_PORT,
                       LCD_RS_PIN,
                       GPIO_LOW);


    LCD_SendNibble(
        (command >> 4U) & 0x0FU);


    LCD_SendNibble(
        command & 0x0FU);


    _delay_us(50);
}


static void LCD_PulseEnable(void)
{
    MCAL_GPIO_WritePin(LCD_PORT,
                       LCD_EN_PIN,
                       GPIO_HIGH);

    _delay_us(1);


    MCAL_GPIO_WritePin(LCD_PORT,
                       LCD_EN_PIN,
                       GPIO_LOW);

    _delay_us(1);
}
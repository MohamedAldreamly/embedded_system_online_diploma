/******************************************************************************
 * @file        hal_keypad.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       HAL
 * @module      Keypad
 *
 * @path        Entry_ATmega32/HAL/Src/hal_keypad.c
 *
 * @brief
 * 4x4 matrix keypad driver used to read user input at the entry gate.
 *
 * Each physical key press is reported only once. The key must be released
 * before another key event can be generated.
 ******************************************************************************/

#include "hal_keypad.h"
#include "atmega32_gpio_driver.h"


/******************************************************************************
 * Keypad Configuration
 ******************************************************************************/

#define KEYPAD_PORT       GPIO_PORT_C

#define KEYPAD_ROW_0      GPIO_PIN_0
#define KEYPAD_ROW_1      GPIO_PIN_1
#define KEYPAD_ROW_2      GPIO_PIN_2
#define KEYPAD_ROW_3      GPIO_PIN_3

#define KEYPAD_COL_0      GPIO_PIN_4
#define KEYPAD_COL_1      GPIO_PIN_5
#define KEYPAD_COL_2      GPIO_PIN_6
#define KEYPAD_COL_3      GPIO_PIN_7

#define KEYPAD_ROWS       4U
#define KEYPAD_COLS       4U


/******************************************************************************
 * Private Data
 ******************************************************************************/

static const uint8_t KeypadRows[KEYPAD_ROWS] =
{
    KEYPAD_ROW_0,
    KEYPAD_ROW_1,
    KEYPAD_ROW_2,
    KEYPAD_ROW_3
};


static const uint8_t KeypadColumns[KEYPAD_COLS] =
{
    KEYPAD_COL_0,
    KEYPAD_COL_1,
    KEYPAD_COL_2,
    KEYPAD_COL_3
};


/*
 * Proteus calculator keypad physical layout:
 *
 *      7   8   9   /
 *      4   5   6   X
 *      1   2   3   -
 *    ON/C  0   =   +
 *
 * Application mapping:
 *      /    -> 'A'  (User Mode)
 *      X    -> 'B'  (Add User Mode)
 *      -    -> 'C'
 *      +    -> 'D'
 *      ON/C -> '*'  (Clear)
 *      =    -> '#'  (Confirm / OK)
 */
static const char KeypadMap[KEYPAD_ROWS][KEYPAD_COLS] =
{
    {'7', '8', '9', 'A'},
    {'4', '5', '6', 'B'},
    {'1', '2', '3', 'C'},
    {'*', '0', '#', 'D'}
};


/*
 * Used to generate only one key event for each
 * physical key press.
 */
static bool KeyReleased = true;


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void HAL_Keypad_Init(void)
{
    uint8_t Local_Index;


    /*
     * Rows are outputs.
     * Idle state = HIGH.
     */
    for (Local_Index = 0U;
         Local_Index < KEYPAD_ROWS;
         Local_Index++)
    {
        MCAL_GPIO_InitPin(KEYPAD_PORT,
                          KeypadRows[Local_Index],
                          GPIO_MODE_OUTPUT);

        MCAL_GPIO_WritePin(KEYPAD_PORT,
                           KeypadRows[Local_Index],
                           GPIO_HIGH);
    }


    /*
     * Columns are inputs with internal pull-up resistors.
     */
    for (Local_Index = 0U;
         Local_Index < KEYPAD_COLS;
         Local_Index++)
    {
        MCAL_GPIO_InitPin(KEYPAD_PORT,
                          KeypadColumns[Local_Index],
                          GPIO_MODE_INPUT_PULLUP);
    }


    KeyReleased = true;
}


char HAL_Keypad_GetKey(void)
{
    uint8_t Local_Row;
    uint8_t Local_Column;

    char Local_Key = KEYPAD_KEY_NONE;


    /*
     * Scan each row separately.
     */
    for (Local_Row = 0U;
         Local_Row < KEYPAD_ROWS;
         Local_Row++)
    {
        /*
         * Activate current row.
         */
        MCAL_GPIO_WritePin(KEYPAD_PORT,
                           KeypadRows[Local_Row],
                           GPIO_LOW);


        /*
         * Check all columns.
         */
        for (Local_Column = 0U;
             Local_Column < KEYPAD_COLS;
             Local_Column++)
        {
            if (MCAL_GPIO_ReadPin(KEYPAD_PORT,
                                  KeypadColumns[Local_Column]) ==
                GPIO_LOW)
            {
                Local_Key =
                    KeypadMap[Local_Row][Local_Column];

                break;
            }
        }


        /*
         * Restore current row to idle state.
         */
        MCAL_GPIO_WritePin(KEYPAD_PORT,
                           KeypadRows[Local_Row],
                           GPIO_HIGH);


        if (Local_Key != KEYPAD_KEY_NONE)
        {
            break;
        }
    }


    /*
     * No key is currently pressed.
     *
     * Allow the next physical key press
     * to generate a new event.
     */
    if (Local_Key == KEYPAD_KEY_NONE)
    {
        KeyReleased = true;

        return KEYPAD_KEY_NONE;
    }


    /*
     * The same key is still being held.
     */
    if (KeyReleased == false)
    {
        return KEYPAD_KEY_NONE;
    }


    /*
     * First detection of a new physical press.
     */
    KeyReleased = false;


    return Local_Key;
}
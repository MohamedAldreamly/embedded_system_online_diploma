/******************************************************************************
 * @file        hal_rfid.c
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
 * @path        Entry_ATmega32/HAL/Src/hal_rfid.c
 *
 * @brief
 * RFID reader driver for the Entry ECU.
 *
 * The module receives RFID serial data through the software UART
 * interface and exposes the card UID to the Entry application.
 ******************************************************************************/

#include "hal_rfid.h"
#include "atmega32_soft_uart_rx.h"

#include <stddef.h>


/******************************************************************************
 * Configuration
 ******************************************************************************/

#define RFID_UID_SIZE     4U


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static uint8_t RFID_Buffer[RFID_UID_SIZE];

static uint8_t RFID_Index = 0U;


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void HAL_RFID_Init(void)
{
    MCAL_SoftUART_RX_Init();

    RFID_Index = 0U;
}


bool HAL_RFID_ReadUID(uint32_t *uid)
{
    uint8_t Local_Data;


    if (uid == NULL)
    {
        return false;
    }


    /*
     * No new serial byte available.
     */
    if (MCAL_SoftUART_RX_ReadByte(&Local_Data) == false)
    {
        return false;
    }


    RFID_Buffer[RFID_Index] = Local_Data;

    RFID_Index++;


    /*
     * Wait until the complete UID is received.
     */
    if (RFID_Index < RFID_UID_SIZE)
    {
        return false;
    }


    /*
     * Convert the four UID bytes into
     * the uint32_t format used by the system.
     */
    *uid =
        ((uint32_t)RFID_Buffer[0] << 24U) |
        ((uint32_t)RFID_Buffer[1] << 16U) |
        ((uint32_t)RFID_Buffer[2] << 8U)  |
        ((uint32_t)RFID_Buffer[3]);


    /*
     * Prepare for the next card.
     */
    RFID_Index = 0U;


    return true;
}
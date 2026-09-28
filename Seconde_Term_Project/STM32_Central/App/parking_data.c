/******************************************************************************
 * @file        parking_data.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         STM32 Central ECU
 * @layer       Application Layer
 * @module      Parking Data Manager
 *
 * @path        STM32_Central/App/parking_data.c
 *
 * @brief
 * Implementation of the Parking Data Manager.
 *
 * This module connects the Application Layer to the EEPROM HAL and
 * provides persistent registered-user data services.
 ******************************************************************************/

#include "parking_data.h"

#include "hal_eeprom.h"

#include <stddef.h>


/******************************************************************************
 * Public Functions
 ******************************************************************************/

bool ParkingData_GetUser(uint32_t userID,
                         User_t *user)
{
    if (user == NULL)
    {
        return false;
    }

    return HAL_EEPROM_GetUser(userID,
                              user);
}


bool ParkingData_AddUser(const User_t *user)
{
    if (user == NULL)
    {
        return false;
    }

    return HAL_EEPROM_AddUser(user);
}


bool ParkingData_UpdateUser(const User_t *user)
{
    if (user == NULL)
    {
        return false;
    }

    return HAL_EEPROM_UpdateUser(user);
}
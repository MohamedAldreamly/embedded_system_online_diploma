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

uint8_t ParkingData_GetOccupancy(void)
{
    uint8_t Local_u8UserCount;
    uint8_t Local_u8Index;
    uint8_t Local_u8Occupancy = 0U;

    User_t Local_User;


    Local_u8UserCount =
            HAL_EEPROM_GetUserCount();


    for (Local_u8Index = 0U;
         Local_u8Index < Local_u8UserCount;
         Local_u8Index++)
    {
        if (HAL_EEPROM_GetUserByIndex(Local_u8Index,
                                      &Local_User) == true)
        {
            if (Local_User.state == USER_INSIDE)
            {
                Local_u8Occupancy++;
            }
        }
    }


    return Local_u8Occupancy;
}
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

bool ParkingData_RFIDExists(uint32_t rfidUID)
{
    uint8_t Local_UserCount;
    uint8_t Local_Index;
    User_t Local_User;


    Local_UserCount =
        HAL_EEPROM_GetUserCount();


    for (Local_Index = 0U;
         Local_Index < Local_UserCount;
         Local_Index++)
    {
        if (HAL_EEPROM_GetUserByIndex(Local_Index,
                                      &Local_User) == true)
        {
            if (Local_User.rfidUID == rfidUID)
            {
                return true;
            }
        }
    }


    return false;
}

bool ParkingData_SeedDefaultUsers(void)
{
    static const User_t Local_DefaultUsers[] =
    {
        {1111UL, 0x31313131UL, USER_OUTSIDE},
        {2222UL, 0x32323232UL, USER_OUTSIDE},
        {3333UL, 0x33333333UL, USER_OUTSIDE}
    };

    uint8_t Local_Index;

    /* Preserve any database that has already been initialized. */
    if (HAL_EEPROM_GetUserCount() != 0U)
    {
        return true;
    }

    for (Local_Index = 0U;
         Local_Index < (uint8_t)(sizeof(Local_DefaultUsers) / sizeof(Local_DefaultUsers[0]));
         Local_Index++)
    {
        if (HAL_EEPROM_AddUser(&Local_DefaultUsers[Local_Index]) == false)
        {
            return false;
        }
    }

    return true;
}


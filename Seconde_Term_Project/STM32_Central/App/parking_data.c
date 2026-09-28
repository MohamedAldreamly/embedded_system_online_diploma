/******************************************************************************
 * @file        parking_data.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed
 * @date        28 September 2026
 *
 * @ecu         STM32 Central ECU
 * @layer       Application Layer
 * @module      Parking Data Manager
 *
 * @path        Smart_Parking_System/STM32_Central/App/parking_data.c
 *
 * @brief
 * Implementation of the Parking Data Manager.
 *
 * This module provides application-level access to persistent user records.
 *
 * The actual EEPROM access is handled by the HAL layer.
 *
 * This module does not contain an internal user database and does not know
 * EEPROM addresses, memory organization, or communication protocol details.
 *
 * Authentication and access decisions are handled by Access Control.
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "parking_data.h"

/*
 * TODO:
 * This HAL header will be implemented during HAL development.
 *
 * The Application Layer already defines the EEPROM services it requires.
 */
#include "hal_eeprom.h"


/******************************************************************************
 * Public Functions
 ******************************************************************************/

/******************************************************************************
 * ParkingData_GetUser
 ******************************************************************************/

bool ParkingData_GetUser(uint32_t userID, User_t *user)
{
    if (user == NULL)
    {
        return false;
    }

    /*
     * Request the complete user record from EEPROM through the HAL.
     *
     * ParkingDataManager only knows the requested User ID.
     *
     * The EEPROM HAL is responsible for locating the corresponding record
     * and returning the complete User_t structure.
     */
    return HAL_EEPROM_GetUser(userID, user);
}


/******************************************************************************
 * ParkingData_AddUser
 ******************************************************************************/

bool ParkingData_AddUser(const User_t *user)
{
    if (user == NULL)
    {
        return false;
    }

    /*
     * Store a new user record through the EEPROM HAL.
     */
    return HAL_EEPROM_AddUser(user);
}


/******************************************************************************
 * ParkingData_UpdateUser
 ******************************************************************************/

bool ParkingData_UpdateUser(const User_t *user)
{
    if (user == NULL)
    {
        return false;
    }

    /*
     * Update an existing persistent user record through the EEPROM HAL.
     */
    return HAL_EEPROM_UpdateUser(user);
}
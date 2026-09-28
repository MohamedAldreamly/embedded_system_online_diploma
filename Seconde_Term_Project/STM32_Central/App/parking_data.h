/******************************************************************************
 * @file        parking_data.h
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
 * @path        Smart_Parking_System/STM32_Central/App/parking_data.h
 *
 * @brief
 * Public interface of the Parking Data Manager.
 *
 * The Parking Data Manager provides application-level access to persistent
 * parking user data.
 *
 * User records are physically stored in EEPROM and accessed through the
 * EEPROM HAL interface.
 *
 * This module does NOT maintain a local user database and does NOT perform
 * authentication or access-control decisions.
 *
 * Access Control is responsible for processing the returned user data and
 * making authentication and parking-access decisions.
 *
 ******************************************************************************/

#ifndef PARKING_DATA_H_
#define PARKING_DATA_H_

/******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "system_types.h"


/******************************************************************************
 * Public Function Prototypes
 ******************************************************************************/

/**
 * @brief Retrieve a user record using the User ID.
 *
 * The user record is obtained from persistent storage through the EEPROM HAL.
 *
 * @param userID  ID of the requested user.
 * @param user    Destination structure for the retrieved user data.
 *
 * @return true   User was found and retrieved successfully.
 * @return false  User was not found or storage access failed.
 */
bool ParkingData_GetUser(uint32_t userID, User_t *user);


/**
 * @brief Add a new user record to persistent storage.
 *
 * @param user Pointer to the new user record.
 *
 * @return true  User stored successfully.
 * @return false Operation failed.
 */
bool ParkingData_AddUser(const User_t *user);


/**
 * @brief Update an existing user record in persistent storage.
 *
 * @param user Pointer to the updated user record.
 *
 * @return true  User updated successfully.
 * @return false Operation failed.
 */
bool ParkingData_UpdateUser(const User_t *user);


#endif /* PARKING_DATA_H_ */
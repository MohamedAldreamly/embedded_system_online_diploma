/******************************************************************************
 * @file        parking_data.h
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
 * @path        STM32_Central/App/parking_data.h
 *
 * @brief
 * Public interface for persistent parking user data.
 *
 * This module provides the Application Layer with simple operations
 * for reading, adding and updating registered users without exposing
 * EEPROM implementation details.
 ******************************************************************************/

#ifndef PARKING_DATA_H_
#define PARKING_DATA_H_

#include <stdint.h>
#include <stdbool.h>

#include "system_types.h"


/******************************************************************************
 * Public APIs
 ******************************************************************************/

/**
 * @brief Get a registered user using the User ID.
 *
 * @param[in]  userID User ID to search for.
 * @param[out] user   Pointer to receive the complete user record.
 *
 * @return true  User found successfully.
 * @return false User not found or data read failed.
 */
bool ParkingData_GetUser(uint32_t userID,
                         User_t *user);


/**
 * @brief Add a new registered user.
 *
 * @param[in] user Pointer to the new user record.
 *
 * @return true  User added successfully.
 * @return false User could not be added.
 */
bool ParkingData_AddUser(const User_t *user);


/**
 * @brief Update an existing registered user.
 *
 * @param[in] user Pointer to the updated user record.
 *
 * @return true  User updated successfully.
 * @return false User not found or update failed.
 */
bool ParkingData_UpdateUser(const User_t *user);


#endif /* PARKING_DATA_H_ */
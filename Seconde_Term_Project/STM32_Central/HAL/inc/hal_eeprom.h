/******************************************************************************
 * @file        hal_eeprom.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed
 * @date        28 September 2026
 *
 * @ecu         STM32 Central ECU
 * @layer       Hardware Abstraction Layer (HAL)
 * @module      SPI EEPROM
 *
 * @path        Smart_Parking_System/STM32_Central/HAL/hal_eeprom.h
 *
 * @brief
 * Public interface for the external SPI EEPROM.
 *
 * This module provides high-level EEPROM services required by the
 * Application Layer.
 *
 * The EEPROM is connected to the STM32 through the SPI peripheral.
 *
 * This HAL module is responsible for:
 * - Mapping user records to EEPROM locations.
 * - Searching stored user records.
 * - Reading complete User_t records.
 * - Adding new User_t records.
 * - Updating existing User_t records.
 *
 * Low-level SPI communication is delegated to the STM32 MCAL SPI driver.
 *
 ******************************************************************************/

#ifndef HAL_EEPROM_H_
#define HAL_EEPROM_H_

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
 * @brief Initialize the external SPI EEPROM interface.
 */
void HAL_EEPROM_Init(void);


/**
 * @brief Retrieve a complete user record using its User ID.
 *
 * @param userID  User ID to search for.
 * @param user    Destination User_t structure.
 *
 * @return true   User found and read successfully.
 * @return false  User not found or EEPROM access failed.
 */
bool HAL_EEPROM_GetUser(uint32_t userID, User_t *user);


/**
 * @brief Add a new user record to EEPROM.
 *
 * @param user Pointer to the User_t record.
 *
 * @return true  User stored successfully.
 * @return false Operation failed or database is full.
 */
bool HAL_EEPROM_AddUser(const User_t *user);


/**
 * @brief Update an existing user record.
 *
 * @param user Pointer to the updated User_t record.
 *
 * @return true  Record updated successfully.
 * @return false User not found or EEPROM access failed.
 */
bool HAL_EEPROM_UpdateUser(const User_t *user);


#endif /* HAL_EEPROM_H_ */
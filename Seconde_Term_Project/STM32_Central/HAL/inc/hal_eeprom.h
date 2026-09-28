/**
 ******************************************************************************
 * @file           : hal_eeprom.h
 * @project        : Smart Car Parking System
 * @version        : 1.0
 * @author         : Eng. Abdalqader Abueta
 * @date           : 2026
 * @layer          : HAL
 * @module         : EEPROM
 * @brief          : 25LC256 EEPROM Driver using SPI1
 ******************************************************************************
 */

#ifndef INC_HAL_EEPROM_H_
#define INC_HAL_EEPROM_H_


/* ========================================================================== */
/*                                  Includes                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdbool.h>
#include "system_types.h"


/* ========================================================================== */
/*                              EEPROM Commands                               */
/* ========================================================================== */

#define EEPROM_CMD_READ          0x03U
#define EEPROM_CMD_WRITE         0x02U
#define EEPROM_CMD_WREN          0x06U
#define EEPROM_CMD_RDSR          0x05U


/* ========================================================================== */
/*                           EEPROM Status Register                           */
/* ========================================================================== */

#define EEPROM_STATUS_WIP        0x01U


/* ========================================================================== */
/*                               EEPROM Size                                  */
/* ========================================================================== */

#define EEPROM_SIZE_BYTES        32768U


/* ========================================================================== */
/*                         APIs Functions Definitions                         */
/* ========================================================================== */

/**
 * @Fn
 * @brief  Initialize EEPROM driver.
 *
 * @retval true  : Initialization successful.
 * @retval false : Initialization failed.
 */
bool HAL_EEPROM_Init(void);


/**
 * @Fn
 * @brief  Write one byte to EEPROM.
 *
 * @param[in] address : EEPROM memory address.
 * @param[in] data    : Byte to be written.
 *
 * @retval true  : Write successful.
 * @retval false : Write failed.
 */
bool HAL_EEPROM_WriteByte(uint16_t address,
                          uint8_t data);


/**
 * @Fn
 * @brief  Read one byte from EEPROM.
 *
 * @param[in]  address : EEPROM memory address.
 * @param[out] data    : Pointer to store received byte.
 *
 * @retval true  : Read successful.
 * @retval false : Read failed.
 */
bool HAL_EEPROM_ReadByte(uint16_t address,
                         uint8_t *data);


/**
 * @Fn
 * @brief  Write multiple bytes to EEPROM.
 *
 * @param[in] address : Start EEPROM address.
 * @param[in] data    : Pointer to data buffer.
 * @param[in] length  : Number of bytes to write.
 *
 * @retval true  : Write successful.
 * @retval false : Write failed.
 *
 * @Note
 * Current implementation writes byte-by-byte.
 * Page write optimization is not used yet.
 */
bool HAL_EEPROM_WriteBytes(uint16_t address,
                           const uint8_t *data,
                           uint16_t length);


/**
 * @Fn
 * @brief  Read multiple bytes from EEPROM.
 *
 * @param[in]  address : Start EEPROM address.
 * @param[out] data    : Pointer to receive buffer.
 * @param[in]  length  : Number of bytes to read.
 *
 * @retval true  : Read successful.
 * @retval false : Read failed.
 */
bool HAL_EEPROM_ReadBytes(uint16_t address,
                          uint8_t *data,
                          uint16_t length);



/**
 * @Fn
 * @brief  Find and read a registered user from EEPROM.
 *
 * @param[in]  userID : User ID to search for.
 * @param[out] user   : Pointer to receive the complete user record.
 *
 * @retval true  : User found and read successfully.
 * @retval false : User not found or read failed.
 */
bool HAL_EEPROM_GetUser(uint32_t userID,
                        User_t *user);


/**
 * @Fn
 * @brief  Add a new registered user to EEPROM.
 *
 * @param[in] user : Pointer to the user record.
 *
 * @retval true  : User added successfully.
 * @retval false : Invalid user, duplicate ID, database full, or write failed.
 */
bool HAL_EEPROM_AddUser(const User_t *user);


/**
 * @Fn
 * @brief  Update an existing registered user in EEPROM.
 *
 * @param[in] user : Pointer to the updated user record.
 *
 * @retval true  : User updated successfully.
 * @retval false : User not found or write failed.
 */
bool HAL_EEPROM_UpdateUser(const User_t *user);

/**
 * @brief Get the number of registered users stored in EEPROM.
 *
 * @return Number of registered users.
 */
uint8_t HAL_EEPROM_GetUserCount(void);


/**
 * @brief Read a registered user using its database index.
 *
 * @param[in]  index User index in EEPROM database.
 * @param[out] user  Pointer to receive user data.
 *
 * @return true  User read successfully.
 * @return false Invalid index or read failure.
 */
bool HAL_EEPROM_GetUserByIndex(uint8_t index,
                               User_t *user);

#endif /* INC_HAL_EEPROM_H_ */
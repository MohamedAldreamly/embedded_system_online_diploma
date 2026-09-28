/******************************************************************************
 * @file        hal_eeprom.c
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
 * @path        Smart_Parking_System/STM32_Central/HAL/hal_eeprom.c
 *
 * @brief
 * Implementation of the external SPI EEPROM abstraction.
 *
 * This module translates high-level user database operations into EEPROM
 * memory operations.
 *
 * Physical SPI transfers are delegated to the STM32 SPI MCAL driver.
 *
 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "hal_eeprom.h"

/*
 * TODO:
 * This MCAL header will be implemented in the next development layer.
 */
#include "stm32_spi.h"


/******************************************************************************
 * Private Definitions
 ******************************************************************************/

/*
 * Logical EEPROM database layout.
 *
 * Exact physical addresses may be adjusted after selecting the
 * external SPI EEPROM device.
 */
#define EEPROM_USER_COUNT_ADDRESS       0x0000U

#define EEPROM_DATABASE_START_ADDRESS   0x0010U


/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static bool EEPROM_ReadUserCount(uint32_t *userCount);

static bool EEPROM_WriteUserCount(uint32_t userCount);

static bool EEPROM_ReadUserRecord(uint32_t index, User_t *user);

static bool EEPROM_WriteUserRecord(uint32_t index, const User_t *user);

static int32_t EEPROM_FindUserIndex(uint32_t userID);


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void HAL_EEPROM_Init(void)
{
    /*
     * EEPROM-specific initialization will be completed after
     * defining the required MCAL SPI services.
     */
}


/******************************************************************************
 * HAL_EEPROM_GetUser
 ******************************************************************************/

bool HAL_EEPROM_GetUser(uint32_t userID, User_t *user)
{
    int32_t userIndex;

    if (user == NULL)
    {
        return false;
    }

    userIndex = EEPROM_FindUserIndex(userID);

    if (userIndex < 0)
    {
        return false;
    }

    return EEPROM_ReadUserRecord((uint32_t)userIndex, user);
}


/******************************************************************************
 * HAL_EEPROM_AddUser
 ******************************************************************************/

bool HAL_EEPROM_AddUser(const User_t *user)
{
    uint32_t userCount;

    if (user == NULL)
    {
        return false;
    }

    /*
     * Prevent duplicate User IDs.
     */
    if (EEPROM_FindUserIndex(user->userID) >= 0)
    {
        return false;
    }

    if (EEPROM_ReadUserCount(&userCount) == false)
    {
        return false;
    }

    if (userCount >= MAX_REGISTERED_USERS)
    {
        return false;
    }

    /*
     * Store the new user at the next available record.
     */
    if (EEPROM_WriteUserRecord(userCount, user) == false)
    {
        return false;
    }

    userCount++;

    if (EEPROM_WriteUserCount(userCount) == false)
    {
        return false;
    }

    return true;
}


/******************************************************************************
 * HAL_EEPROM_UpdateUser
 ******************************************************************************/

bool HAL_EEPROM_UpdateUser(const User_t *user)
{
    int32_t userIndex;

    if (user == NULL)
    {
        return false;
    }

    userIndex = EEPROM_FindUserIndex(user->userID);

    if (userIndex < 0)
    {
        return false;
    }

    return EEPROM_WriteUserRecord((uint32_t)userIndex, user);
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

static int32_t EEPROM_FindUserIndex(uint32_t userID)
{
    uint32_t userCount;
    uint32_t index;
    User_t currentUser;

    if (EEPROM_ReadUserCount(&userCount) == false)
    {
        return -1;
    }

    if (userCount > MAX_REGISTERED_USERS)
    {
        return -1;
    }

    for (index = 0U; index < userCount; index++)
    {
        if (EEPROM_ReadUserRecord(index, &currentUser) == false)
        {
            return -1;
        }

        if (currentUser.userID == userID)
        {
            return (int32_t)index;
        }
    }

    return -1;
}
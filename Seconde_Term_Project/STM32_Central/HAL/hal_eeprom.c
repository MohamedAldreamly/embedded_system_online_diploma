/**
 ******************************************************************************
 * @file           : hal_eeprom.c
 * @project        : Smart Car Parking System
 * @version        : 1.0
 * @author         : Eng. Abdalqader Abueta
 * @date           : 2026
 * @layer          : HAL
 * @module         : EEPROM
 * @brief          : 25LC256 EEPROM Driver using SPI1
 ******************************************************************************
 */


/* ========================================================================== */
/*                                  Includes                                  */
/* ========================================================================== */

#include "hal_eeprom.h"
#include "Stm32_F103C6_SPI_driver.h"
#include "Stm32_F103C6_GPIO_driver.h"

#include <stddef.h>


/* ========================================================================== */
/*                              EEPROM CS Pin                                 */
/* ========================================================================== */

/*
 * Change these definitions according to
 * the actual EEPROM CS connection.
 */

#define EEPROM_CS_PORT           GPIOA
#define EEPROM_CS_PIN            GPIO_PIN_4



/* ========================================================================== */
/*                         User Database Memory Map                           */
/* ========================================================================== */

#define EEPROM_USER_COUNT_ADDRESS       0x0000U
#define EEPROM_USER_DATA_START          0x0010U
#define EEPROM_USER_RECORD_SIZE         9U


/* ========================================================================== */
/*                           Private Function Prototypes                       */
/* ========================================================================== */

static void EEPROM_Select(void);

static void EEPROM_Deselect(void);

static uint8_t EEPROM_SPI_Transfer(uint8_t data);

static void EEPROM_WriteEnable(void);

static uint8_t EEPROM_ReadStatus(void);

static void EEPROM_WaitUntilReady(void);

static bool EEPROM_ReadUserCount(uint8_t *count);
static bool EEPROM_WriteUserCount(uint8_t count);
static bool EEPROM_ReadUserRecord(uint8_t index, User_t *user);
static bool EEPROM_WriteUserRecord(uint8_t index, const User_t *user);
static int16_t EEPROM_FindUserIndex(uint32_t userID);


/* ========================================================================== */
/*                             Private Functions                              */
/* ========================================================================== */


/**
 * @Fn
 * @brief  Activate EEPROM Chip Select.
 */
static void EEPROM_Select(void)
{
    MCAL_GPIO_WritePin(EEPROM_CS_PORT,
                       EEPROM_CS_PIN,
                       GPIO_PIN_RESET);
}


/**
 * @Fn
 * @brief  Deactivate EEPROM Chip Select.
 */
static void EEPROM_Deselect(void)
{
    MCAL_GPIO_WritePin(EEPROM_CS_PORT,
                       EEPROM_CS_PIN,
                       GPIO_PIN_SET);
}


/**
 * @Fn
 * @brief  Send and receive one byte through SPI1.
 *
 * @param[in] data : Byte to transmit.
 *
 * @retval Received byte.
 */
static uint8_t EEPROM_SPI_Transfer(uint8_t data)
{
    uint16_t Local_u16Data;


    Local_u16Data = (uint16_t)data;


    MCAL_SPI_TX_RX(SPI1,
                   &Local_u16Data,
                   Pollingenable);


    return (uint8_t)Local_u16Data;
}


/**
 * @Fn
 * @brief  Enable EEPROM write operation.
 */
static void EEPROM_WriteEnable(void)
{
    EEPROM_Select();


    EEPROM_SPI_Transfer(EEPROM_CMD_WREN);


    EEPROM_Deselect();
}


/**
 * @Fn
 * @brief  Read EEPROM status register.
 *
 * @retval EEPROM status register value.
 */
static uint8_t EEPROM_ReadStatus(void)
{
    uint8_t Local_u8Status;


    EEPROM_Select();


    EEPROM_SPI_Transfer(EEPROM_CMD_RDSR);


    Local_u8Status =
            EEPROM_SPI_Transfer(0xFFU);


    EEPROM_Deselect();


    return Local_u8Status;
}


/**
 * @Fn
 * @brief  Wait until EEPROM internal write cycle finishes.
 */
static void EEPROM_WaitUntilReady(void)
{
    while ((EEPROM_ReadStatus() &
            EEPROM_STATUS_WIP) != 0U)
    {
        /* Wait until WIP bit becomes zero */
    }
}


/* ========================================================================== */
/*                              Public Functions                              */
/* ========================================================================== */


/**
 * @Fn
 * @brief  Initialize EEPROM driver.
 *
 * @retval true : Initialization successful.
 */
bool HAL_EEPROM_Init(void)
{
    /*
     * EEPROM CS must stay HIGH
     * when EEPROM is not selected.
     */

    uint8_t Local_u8UserCount;


    EEPROM_Deselect();


    /*
     * A new / erased EEPROM normally reads 0xFF.
     * If the stored count is outside the supported range,
     * initialize the user database as empty.
     */
    if (EEPROM_ReadUserCount(&Local_u8UserCount) == true)
    {
        if (Local_u8UserCount > MAX_REGISTERED_USERS)
        {
            Local_u8UserCount = 0U;

            if (EEPROM_WriteUserCount(Local_u8UserCount) == false)
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }


    return true;
}


/**
 * @Fn
 * @brief  Write one byte to EEPROM.
 *
 * @param[in] address : EEPROM memory address.
 * @param[in] data    : Byte to be written.
 *
 * @retval true  : Write successful.
 * @retval false : Invalid address.
 */
bool HAL_EEPROM_WriteByte(uint16_t address,
                          uint8_t data)
{
    uint8_t Local_u8AddressHigh;
    uint8_t Local_u8AddressLow;


    /* Validate address */

    if (address >= EEPROM_SIZE_BYTES)
    {
        return false;
    }


    /*
     * Split 16-bit EEPROM address
     * into two 8-bit values.
     */

    Local_u8AddressHigh =
            (uint8_t)(address >> 8);

    Local_u8AddressLow =
            (uint8_t)(address);


    /*
     * EEPROM requires WREN command
     * before every write operation.
     */

    EEPROM_WriteEnable();


    /* Start SPI transaction */

    EEPROM_Select();


    /* Send WRITE command */

    EEPROM_SPI_Transfer(EEPROM_CMD_WRITE);


    /* Send EEPROM address */

    EEPROM_SPI_Transfer(Local_u8AddressHigh);

    EEPROM_SPI_Transfer(Local_u8AddressLow);


    /* Send data byte */

    EEPROM_SPI_Transfer(data);


    /* Finish SPI transaction */

    EEPROM_Deselect();


    /*
     * Wait until EEPROM completes
     * its internal write operation.
     */

    EEPROM_WaitUntilReady();


    return true;
}


/**
 * @Fn
 * @brief  Read one byte from EEPROM.
 *
 * @param[in]  address : EEPROM memory address.
 * @param[out] data    : Pointer to store received data.
 *
 * @retval true  : Read successful.
 * @retval false : Invalid parameter.
 */
bool HAL_EEPROM_ReadByte(uint16_t address,
                         uint8_t *data)
{
    uint8_t Local_u8AddressHigh;
    uint8_t Local_u8AddressLow;


    /* Validate parameters */

    if ((data == NULL) ||
        (address >= EEPROM_SIZE_BYTES))
    {
        return false;
    }


    /*
     * Split 16-bit EEPROM address
     * into two 8-bit values.
     */

    Local_u8AddressHigh =
            (uint8_t)(address >> 8);

    Local_u8AddressLow =
            (uint8_t)(address);


    /* Start SPI transaction */

    EEPROM_Select();


    /* Send READ command */

    EEPROM_SPI_Transfer(EEPROM_CMD_READ);


    /* Send EEPROM address */

    EEPROM_SPI_Transfer(Local_u8AddressHigh);

    EEPROM_SPI_Transfer(Local_u8AddressLow);


    /*
     * Send dummy byte to generate
     * SPI clock and receive EEPROM data.
     */

    *data =
        EEPROM_SPI_Transfer(0xFFU);


    /* Finish SPI transaction */

    EEPROM_Deselect();


    return true;
}


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
 * Data is currently written byte-by-byte.
 */
bool HAL_EEPROM_WriteBytes(uint16_t address,
                           const uint8_t *data,
                           uint16_t length)
{
    uint16_t Local_u16Index;


    /* Validate parameters */

    if ((data == NULL) ||
        (length == 0U))
    {
        return false;
    }


    /* Validate memory range */

    if (((uint32_t)address +
         (uint32_t)length) >
        EEPROM_SIZE_BYTES)
    {
        return false;
    }


    /*
     * Write data byte-by-byte.
     *
     * Page write is intentionally
     * not used at this stage.
     */

    for (Local_u16Index = 0U;
         Local_u16Index < length;
         Local_u16Index++)
    {
        if (HAL_EEPROM_WriteByte(
                (uint16_t)(address +
                           Local_u16Index),
                data[Local_u16Index]) == false)
        {
            return false;
        }
    }


    return true;
}


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
                          uint16_t length)
{
    uint16_t Local_u16Index;


    /* Validate parameters */

    if ((data == NULL) ||
        (length == 0U))
    {
        return false;
    }


    /* Validate memory range */

    if (((uint32_t)address +
         (uint32_t)length) >
        EEPROM_SIZE_BYTES)
    {
        return false;
    }


    /* Read data byte-by-byte */

    for (Local_u16Index = 0U;
         Local_u16Index < length;
         Local_u16Index++)
    {
        if (HAL_EEPROM_ReadByte(
                (uint16_t)(address +
                           Local_u16Index),
                &data[Local_u16Index]) == false)
        {
            return false;
        }
    }


    return true;
}


/* ========================================================================== */
/*                         User Database Private Functions                    */
/* ========================================================================== */

static bool EEPROM_ReadUserCount(uint8_t *count)
{
    if (count == NULL)
    {
        return false;
    }

    return HAL_EEPROM_ReadByte(EEPROM_USER_COUNT_ADDRESS,
                               count);
}


static bool EEPROM_WriteUserCount(uint8_t count)
{
    return HAL_EEPROM_WriteByte(EEPROM_USER_COUNT_ADDRESS,
                                count);
}


static bool EEPROM_WriteUserRecord(uint8_t index,
                                   const User_t *user)
{
    uint16_t Local_u16Address;
    uint8_t Local_u8Buffer[EEPROM_USER_RECORD_SIZE];

    if ((user == NULL) ||
        (index >= MAX_REGISTERED_USERS))
    {
        return false;
    }

    Local_u16Address =
        (uint16_t)(EEPROM_USER_DATA_START +
        ((uint16_t)index * EEPROM_USER_RECORD_SIZE));

    Local_u8Buffer[0] = (uint8_t)(user->userID >> 24);
    Local_u8Buffer[1] = (uint8_t)(user->userID >> 16);
    Local_u8Buffer[2] = (uint8_t)(user->userID >> 8);
    Local_u8Buffer[3] = (uint8_t)(user->userID);

    Local_u8Buffer[4] = (uint8_t)(user->rfidUID >> 24);
    Local_u8Buffer[5] = (uint8_t)(user->rfidUID >> 16);
    Local_u8Buffer[6] = (uint8_t)(user->rfidUID >> 8);
    Local_u8Buffer[7] = (uint8_t)(user->rfidUID);

    Local_u8Buffer[8] = (uint8_t)user->state;

    return HAL_EEPROM_WriteBytes(Local_u16Address,
                                 Local_u8Buffer,
                                 EEPROM_USER_RECORD_SIZE);
}


static bool EEPROM_ReadUserRecord(uint8_t index,
                                  User_t *user)
{
    uint16_t Local_u16Address;
    uint8_t Local_u8Buffer[EEPROM_USER_RECORD_SIZE];

    if ((user == NULL) ||
        (index >= MAX_REGISTERED_USERS))
    {
        return false;
    }

    Local_u16Address =
        (uint16_t)(EEPROM_USER_DATA_START +
        ((uint16_t)index * EEPROM_USER_RECORD_SIZE));

    if (HAL_EEPROM_ReadBytes(Local_u16Address,
                             Local_u8Buffer,
                             EEPROM_USER_RECORD_SIZE) == false)
    {
        return false;
    }

    user->userID =
        ((uint32_t)Local_u8Buffer[0] << 24) |
        ((uint32_t)Local_u8Buffer[1] << 16) |
        ((uint32_t)Local_u8Buffer[2] << 8)  |
        ((uint32_t)Local_u8Buffer[3]);

    user->rfidUID =
        ((uint32_t)Local_u8Buffer[4] << 24) |
        ((uint32_t)Local_u8Buffer[5] << 16) |
        ((uint32_t)Local_u8Buffer[6] << 8)  |
        ((uint32_t)Local_u8Buffer[7]);

    user->state = (UserState_t)Local_u8Buffer[8];

    return true;
}


static int16_t EEPROM_FindUserIndex(uint32_t userID)
{
    uint8_t Local_u8UserCount;
    uint8_t Local_u8Index;
    User_t Local_User;

    if (EEPROM_ReadUserCount(&Local_u8UserCount) == false)
    {
        return -1;
    }

    if (Local_u8UserCount > MAX_REGISTERED_USERS)
    {
        return -1;
    }

    for (Local_u8Index = 0U;
         Local_u8Index < Local_u8UserCount;
         Local_u8Index++)
    {
        if (EEPROM_ReadUserRecord(Local_u8Index,
                                  &Local_User) == false)
        {
            return -1;
        }

        if (Local_User.userID == userID)
        {
            return (int16_t)Local_u8Index;
        }
    }

    return -1;
}


/* ========================================================================== */
/*                         User Database Public APIs                          */
/* ========================================================================== */

bool HAL_EEPROM_GetUser(uint32_t userID,
                        User_t *user)
{
    int16_t Local_s16Index;

    if (user == NULL)
    {
        return false;
    }

    Local_s16Index = EEPROM_FindUserIndex(userID);

    if (Local_s16Index < 0)
    {
        return false;
    }

    return EEPROM_ReadUserRecord((uint8_t)Local_s16Index,
                                 user);
}


bool HAL_EEPROM_AddUser(const User_t *user)
{
    uint8_t Local_u8UserCount;

    if (user == NULL)
    {
        return false;
    }

    if (EEPROM_FindUserIndex(user->userID) >= 0)
    {
        return false;
    }

    if (EEPROM_ReadUserCount(&Local_u8UserCount) == false)
    {
        return false;
    }

    if (Local_u8UserCount >= MAX_REGISTERED_USERS)
    {
        return false;
    }

    if (EEPROM_WriteUserRecord(Local_u8UserCount,
                               user) == false)
    {
        return false;
    }

    Local_u8UserCount++;

    return EEPROM_WriteUserCount(Local_u8UserCount);
}


bool HAL_EEPROM_UpdateUser(const User_t *user)
{
    int16_t Local_s16Index;

    if (user == NULL)
    {
        return false;
    }

    Local_s16Index = EEPROM_FindUserIndex(user->userID);

    if (Local_s16Index < 0)
    {
        return false;
    }

    return EEPROM_WriteUserRecord((uint8_t)Local_s16Index,
                                  user);
}

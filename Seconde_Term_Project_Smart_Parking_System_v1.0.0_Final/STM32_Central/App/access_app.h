/******************************************************************************
 * @file        access_app.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         STM32 Central ECU
 * @mcu         STM32F103C6
 * @layer       Application Layer
 * @module      Access Control
 *
 * @path        STM32_Central/App/access_app.h
 *
 * @brief
 * Public interface for the Smart Parking access-control logic.
 *
 * Responsibilities:
 * - Validate registered User IDs.
 * - Verify RFID ownership.
 * - Check entry and exit eligibility.
 * - Register new users.
 * - Update the persistent parking state after completed passage.
 ******************************************************************************/

#ifndef ACCESS_APP_H_
#define ACCESS_APP_H_


#include <stdint.h>
#include <stdbool.h>

#include "system_types.h"


/******************************************************************************
 * Add User Result
 ******************************************************************************/

typedef enum
{
    ADD_USER_OK = 0,
    ADD_USER_ALREADY_EXISTS,
    ADD_USER_RFID_ALREADY_EXISTS,
    ADD_USER_DATABASE_FULL,
    ADD_USER_FAILED

} AddUserResult_t;


/******************************************************************************
 * Public APIs
 ******************************************************************************/

/**
 * @brief Validate that a User ID is registered.
 *
 * @param[in] userID User ID received from Entry or Exit ECU.
 *
 * @return ACCESS_ID_VALID if registered.
 * @return ACCESS_INVALID_ID if not registered.
 */
AccessResult_t Access_ValidateID(uint32_t userID);


/**
 * @brief Validate RFID and determine whether access is allowed.
 *
 * @param[in] userID  Previously validated User ID.
 * @param[in] rfidUID RFID UID received from the gate ECU.
 * @param[in] source  Request source: Entry or Exit ECU.
 *
 * @return Access decision.
 */
AccessResult_t Access_ValidateRFID(uint32_t userID,
                                  uint32_t rfidUID,
                                  ECU_Source_t source);


/**
 * @brief Complete a successful entry or exit operation.
 *
 * Called only after the vehicle has actually passed the gate.
 *
 * @param[in] userID User whose parking state must be updated.
 * @param[in] source Entry or Exit ECU.
 *
 * @return true if the persistent user state was updated successfully.
 */
bool Access_CompleteTransaction(uint32_t userID,
                                ECU_Source_t source);


/**
 * @brief Register a new parking user.
 *
 * The new user is initially stored as USER_OUTSIDE.
 *
 * @param[in] userID  New User ID.
 * @param[in] rfidUID RFID UID assigned to the new user.
 *
 * @return Result of the registration operation.
 */
AddUserResult_t Access_AddUser(uint32_t userID,
                               uint32_t rfidUID);


#endif /* ACCESS_APP_H_ */
/******************************************************************************
 * @file        access_app.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         STM32 Central ECU
 * @layer       Application Layer
 * @module      Access Control
 *
 * @path        STM32_Central/App/access_app.h
 *
 * @brief
 * Public interface for the Smart Parking access-control logic.
 *
 * This module validates registered users, verifies RFID ownership,
 * checks entry/exit eligibility and updates the persistent parking state.
 ******************************************************************************/

#ifndef ACCESS_APP_H_
#define ACCESS_APP_H_

#include <stdint.h>

#include "system_types.h"


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


#endif /* ACCESS_APP_H_ */
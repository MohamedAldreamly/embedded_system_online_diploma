/******************************************************************************
 * @file        access_app.c
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
 * @path        STM32_Central/App/access_app.c
 *
 * @brief
 * Implementation of the Smart Parking access-control logic.
 *
 * This module is responsible for:
 * - Validating registered User IDs.
 * - Verifying RFID ownership.
 * - Checking entry and exit eligibility.
 * - Checking parking capacity before entry.
 * - Updating the user's parking state after a completed passage.
 ******************************************************************************/

#include "access_app.h"
#include "parking_data.h"


/******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Validate a registered User ID.
 *
 * @param[in] userID User ID received from Entry or Exit ECU.
 *
 * @return ACCESS_ID_VALID if the user is registered.
 * @return ACCESS_INVALID_ID if the user is not registered.
 */
AccessResult_t Access_ValidateID(uint32_t userID)
{
    User_t Local_User;


    if (ParkingData_GetUser(userID,
                            &Local_User) == false)
    {
        return ACCESS_INVALID_ID;
    }


    return ACCESS_ID_VALID;
}


/**
 * @brief Validate RFID and determine entry or exit eligibility.
 *
 * The function first verifies that the RFID belongs to the supplied
 * User ID, then applies the required entry or exit rules.
 *
 * RFID retry counting is handled by the Entry/Exit ECU session logic,
 * not by the central Access Control module.
 *
 * @param[in] userID  Previously entered User ID.
 * @param[in] rfidUID RFID UID received from the gate ECU.
 * @param[in] source  ECU requesting access (Entry or Exit).
 *
 * @return Access decision represented by AccessResult_t.
 */
AccessResult_t Access_ValidateRFID(uint32_t userID,
                                  uint32_t rfidUID,
                                  ECU_Source_t source)
{
    User_t Local_User;
	

    /* Get the registered user data. */

    if (ParkingData_GetUser(userID,
                            &Local_User) == false)
    {
        return ACCESS_INVALID_ID;
    }


    /* Verify that the RFID belongs to the supplied User ID. */

    if (Local_User.rfidUID != rfidUID)
    {
        return ACCESS_WRONG_CARD;
		
    }
	


    /* ========================= ENTRY REQUEST ========================= */

    if (source == SOURCE_ENTRY)
    {
        /*
         * A user already inside the parking area
         * cannot perform another entry.
         */
        if (Local_User.state == USER_INSIDE)
        {
            return ACCESS_ALREADY_INSIDE;
        }


        /* Check available parking capacity. */

        if (ParkingData_GetOccupancy() >= PARKING_CAPACITY)
        {
            return ACCESS_PARKING_FULL;
        }


        return ACCESS_GRANTED;
    }


    /* ========================== EXIT REQUEST ========================= */

    if (source == SOURCE_EXIT)
    {
        /*
         * A user currently outside the parking area
         * cannot perform an exit.
         */
        if (Local_User.state == USER_OUTSIDE)
        {
            return ACCESS_INVALID_EXIT;
        }


        return ACCESS_GRANTED;
    }


    /* Reject requests from an invalid ECU source. */

    return ACCESS_DENIED;
}


/**
 * @brief Complete a successful entry or exit transaction.
 *
 * This function must be called only after the vehicle has actually
 * completed passage through the gate.
 *
 * For Entry:
 *     USER_OUTSIDE -> USER_INSIDE
 *
 * For Exit:
 *     USER_INSIDE -> USER_OUTSIDE
 *
 * @param[in] userID User whose state must be updated.
 * @param[in] source ECU that completed the transaction.
 *
 * @return true  User state updated successfully.
 * @return false User not found, invalid source, or EEPROM update failed.
 */
bool Access_CompleteTransaction(uint32_t userID,
                                ECU_Source_t source)
{
    User_t Local_User;


    /* Get the current persistent user data. */

    if (ParkingData_GetUser(userID,
                            &Local_User) == false)
    {
        return false;
    }


    /* Update parking state according to transaction source. */

    if (source == SOURCE_ENTRY)
    {
        Local_User.state = USER_INSIDE;
    }
    else if (source == SOURCE_EXIT)
    {
        Local_User.state = USER_OUTSIDE;
    }
    else
    {
        return false;
    }


    /* Save the updated user state in persistent memory. */

    return ParkingData_UpdateUser(&Local_User);
}
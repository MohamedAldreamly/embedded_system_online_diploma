/******************************************************************************
 * @file        access_app.c
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
 * @path        STM32_Central/App/access_app.c
 *
 * @brief
 * Implementation of the Smart Parking access-control logic.
 *
 * Responsibilities:
 * - Validate registered User IDs.
 * - Verify RFID ownership.
 * - Check entry and exit eligibility.
 * - Check parking capacity before entry.
 * - Register new users.
 * - Update the user's parking state after completed passage.
 ******************************************************************************/

#include "access_app.h"
#include "parking_data.h"


/******************************************************************************
 * Validate User ID
 ******************************************************************************/

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


/******************************************************************************
 * Validate RFID And Access Eligibility
 ******************************************************************************/

AccessResult_t Access_ValidateRFID(uint32_t userID,
                                  uint32_t rfidUID,
                                  ECU_Source_t source)
{
    User_t Local_User;


    /*
     * Get the registered user data.
     */
    if (ParkingData_GetUser(userID,
                            &Local_User) == false)
    {
        return ACCESS_INVALID_ID;
    }


    /*
     * Verify that the RFID belongs to the supplied User ID.
     */
    if (Local_User.rfidUID != rfidUID)
    {
        return ACCESS_WRONG_CARD;
    }


    /* =====================================================
     * ENTRY REQUEST
     * ===================================================== */

    if (source == SOURCE_ENTRY)
    {
        /*
         * User already inside cannot enter again.
         */
        if (Local_User.state == USER_INSIDE)
        {
            return ACCESS_ALREADY_INSIDE;
        }


        /*
         * Check available parking capacity.
         */
        if (ParkingData_GetOccupancy() >= PARKING_CAPACITY)
        {
            return ACCESS_PARKING_FULL;
        }


        return ACCESS_GRANTED;
    }


    /* =====================================================
     * EXIT REQUEST
     * ===================================================== */

    if (source == SOURCE_EXIT)
    {
        /*
         * User already outside cannot perform an exit.
         */
        if (Local_User.state == USER_OUTSIDE)
        {
            return ACCESS_INVALID_EXIT;
        }


        return ACCESS_GRANTED;
    }


    /*
     * Invalid ECU source.
     */
    return ACCESS_DENIED;
}


/******************************************************************************
 * Complete Entry / Exit Transaction
 ******************************************************************************/

bool Access_CompleteTransaction(uint32_t userID,
                                ECU_Source_t source)
{
    User_t Local_User;


    /*
     * Get current persistent user data.
     */
    if (ParkingData_GetUser(userID,
                            &Local_User) == false)
    {
        return false;
    }


    /*
     * Update user parking state.
     */
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


    /*
     * Save updated user state to EEPROM.
     */
    return ParkingData_UpdateUser(&Local_User);
}


/******************************************************************************
 * Register New User
 ******************************************************************************/

AddUserResult_t Access_AddUser(uint32_t userID,
                               uint32_t rfidUID)
{
    User_t Local_User;


    /*
     * Do not allow an invalid zero ID.
     */
    if (userID == 0UL)
    {
        return ADD_USER_FAILED;
    }


    /*
     * Do not allow an invalid zero RFID UID.
     */
    if (rfidUID == 0UL)
    {
        return ADD_USER_FAILED;
    }


    /*
     * User ID must be unique.
     *
     * If ParkingData_GetUser() succeeds, the ID is already
     * registered in EEPROM.
     */
    if (ParkingData_GetUser(userID,
                            &Local_User) == true)
    {
        return ADD_USER_ALREADY_EXISTS;
    }
	
	
	if (ParkingData_RFIDExists(rfidUID) == true)
	{
		return ADD_USER_RFID_ALREADY_EXISTS;
	}

    /*
     * Build the new persistent user record.
     *
     * Every newly registered user starts OUTSIDE the parking
     * area. The state changes to USER_INSIDE only after a real
     * successful Entry transaction is completed.
     */
    Local_User.userID = userID;
    Local_User.rfidUID = rfidUID;
    Local_User.state = USER_OUTSIDE;


    /*
     * Store the new user through Parking Data Manager.
     */
    if (ParkingData_AddUser(&Local_User) == false)
    {
        /*
         * At the current abstraction level, AddUser returning
         * false means the record could not be stored.
         *
         * This includes a full database/storage failure.
         */
        return ADD_USER_DATABASE_FULL;
    }


    return ADD_USER_OK;
}
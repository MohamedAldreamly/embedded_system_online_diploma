/******************************************************************************
 * @file        entry_app.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       Application Layer
 * @module      Entry Application
 *
 * @path        Entry_ATmega32/App/entry_app.c
 *
 * @brief
 * Entry ECU application state machine.
 *
 * Responsibilities:
 * - Select Normal Entry or Add User mode.
 * - Protect Add User mode using an Admin PIN.
 * - Register new users through the STM32 Central ECU.
 * - Read User ID from keypad.
 * - Read RFID UID.
 * - Request access validation from STM32.
 * - Allow maximum three wrong RFID attempts.
 * - Control the entry gate.
 * - Detect vehicle passage using IR sensor.
 * - Never close the gate while the IR path is blocked.
 * - Notify STM32 only after completed vehicle passage.
 ******************************************************************************/

#include "entry_app.h"
#include "entry_communication.h"

#include "hal_keypad.h"
#include "hal_rfid.h"
#include "hal_gate.h"
#include "hal_ir.h"
#include "hal_lcd.h"

#include "atmega32_timer_driver.h"

#include "system_types.h"
#include "parking_protocol.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/******************************************************************************
 * Private Configuration
 ******************************************************************************/

#define ENTRY_MAX_ID_DIGITS       9U

#define ADMIN_PIN                 1234UL
#define ADMIN_PIN_MAX_DIGITS      9U

#define GATE_TIMEOUT_MS           10000UL
#define GATE_MOVEMENT_TIME_MS     2000UL


/******************************************************************************
 * State Handler Type
 ******************************************************************************/

typedef void (*EntryStateHandler_t)(void);


/******************************************************************************
 * Display Screens
 ******************************************************************************/

typedef enum
{
    ENTRY_SCREEN_NONE = 0,

    /* Mode selection */
    ENTRY_SCREEN_SELECT_MODE,

    /* Normal Entry */
    ENTRY_SCREEN_ENTER_ID,
    ENTRY_SCREEN_CHECKING_ID,
    ENTRY_SCREEN_SCAN_RFID,
    ENTRY_SCREEN_CHECKING_RFID,
    ENTRY_SCREEN_WRONG_CARD,
    ENTRY_SCREEN_ACCESS_DENIED,
    ENTRY_SCREEN_PARKING_FULL,
    ENTRY_SCREEN_ALREADY_INSIDE,
    ENTRY_SCREEN_ACCESS_GRANTED,

    /* Registration */
    ENTRY_SCREEN_ADMIN_PIN,
    ENTRY_SCREEN_INVALID_ADMIN_PIN,
    ENTRY_SCREEN_NEW_USER_ID,
    ENTRY_SCREEN_NEW_USER_RFID,
    ENTRY_SCREEN_ADDING_USER,
    ENTRY_SCREEN_USER_ADDED,
    ENTRY_SCREEN_USER_EXISTS,
    ENTRY_SCREEN_RFID_EXISTS,
    ENTRY_SCREEN_DATABASE_FULL,

    /* Gate */
    ENTRY_SCREEN_WAIT_VEHICLE,
    ENTRY_SCREEN_WAIT_CLEAR,
    ENTRY_SCREEN_TIMEOUT,
    ENTRY_SCREEN_CLOSING,
    ENTRY_SCREEN_COMPLETE

} EntryScreen_t;


/******************************************************************************
 * Private State Handlers
 ******************************************************************************/

static void Entry_StateIdle(void);

static void Entry_StateSelectMode(void);


/* Normal Entry */

static void Entry_StateWaitID(void);

static void Entry_StateWaitIDResponse(void);

static void Entry_StateWaitRFID(void);

static void Entry_StateWaitRFIDResponse(void);


/* Add User */

static void Entry_StateAdminPIN(void);

static void Entry_StateNewUserID(void);

static void Entry_StateNewUserRFID(void);

static void Entry_StateWaitAddUserResponse(void);


/* Gate */

static void Entry_StateOpenGate(void);

static void Entry_StateWaitVehicle(void);

static void Entry_StateWaitClear(void);

static void Entry_StateCloseGate(void);

static void Entry_StateComplete(void);


/******************************************************************************
 * Private Helper Functions
 ******************************************************************************/

static void Entry_ResetSession(void);

static void Entry_SendIDRequest(void);

static void Entry_SendRFIDRequest(void);

static void Entry_SendAddUserRequest(void);

static void Entry_SendCompletion(void);

static void Entry_DisplayScreen(EntryScreen_t screen);

static void Entry_DisplayCurrentID(void);


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static EntryStateHandler_t CurrentState = NULL;


/* Current user data. */

static uint32_t CurrentUserID = 0UL;

static uint32_t CurrentRFIDUID = 0UL;


/* User ID input. */

static uint8_t IDDigitCount = 0U;


/* RFID attempts. */

static uint8_t RFIDAttempts = 0U;


/* Admin PIN input. */

static uint32_t AdminPINValue = 0UL;

static uint8_t AdminPINDigitCount = 0U;


/* Gate timing. */

static uint32_t GateStartTime = 0UL;


/*
 * True only after actual vehicle passage:
 *
 * BLOCKED -> CLEAR
 */
static bool EntryCompleted = false;


/*
 * True while the gate motor is being driven.
 */
static bool GateMovementStarted = false;


/*
 * Last rendered LCD screen.
 */
static EntryScreen_t CurrentScreen = ENTRY_SCREEN_NONE;


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void EntryApp_Init(void)
{
    ECU_Communication_Init();

    HAL_Keypad_Init();

    HAL_RFID_Init();

    HAL_Gate_Init();

    HAL_IR_Init();

    HAL_LCD_Init();


    /*
     * Timer0 provides the 1 ms time base.
     *
     * main() must enable global interrupts using sei().
     */
    MCAL_Timer0_Init();


    Entry_ResetSession();


    CurrentScreen = ENTRY_SCREEN_NONE;

    CurrentState = Entry_StateIdle;
}


void EntryApp_Update(void)
{
    if (CurrentState != NULL)
    {
        CurrentState();
    }
}


/******************************************************************************
 * State: Idle
 ******************************************************************************/

static void Entry_StateIdle(void)
{
    Entry_ResetSession();


    Entry_DisplayScreen(
        ENTRY_SCREEN_SELECT_MODE);


    CurrentState =
        Entry_StateSelectMode;
}


/******************************************************************************
 * State: Select Mode
 ******************************************************************************/

static void Entry_StateSelectMode(void)
{
    char Local_Key;


    Local_Key =
        HAL_Keypad_GetKey();


    if (Local_Key == KEYPAD_KEY_NONE)
    {
        return;
    }


    /*
     * '*' = Normal Entry Mode.
     *
     * The Entry ECU uses a 4x3 keypad in Proteus.
     * In this state only, '*' selects normal entry mode.
     */
    if (Local_Key == KEYPAD_KEY_CLEAR)
    {
        CurrentUserID = 0UL;

        IDDigitCount = 0U;


        Entry_DisplayScreen(
            ENTRY_SCREEN_ENTER_ID);


        CurrentState =
            Entry_StateWaitID;


        return;
    }


    /*
     * '#' = Add User Mode.
     *
     * The Entry ECU uses a 4x3 keypad in Proteus.
     * In this state only, '#' selects Add User mode.
     */
    if (Local_Key == KEYPAD_KEY_CONFIRM)
    {
        AdminPINValue = 0UL;

        AdminPINDigitCount = 0U;


        Entry_DisplayScreen(
            ENTRY_SCREEN_ADMIN_PIN);


        CurrentState =
            Entry_StateAdminPIN;


        return;
    }
}


/******************************************************************************
 * State: Wait User ID
 ******************************************************************************/

static void Entry_StateWaitID(void)
{
    char Local_Key;


    Local_Key =
        HAL_Keypad_GetKey();


    if (Local_Key == KEYPAD_KEY_NONE)
    {
        return;
    }


    /*
     * Numeric key.
     */
    if ((Local_Key >= '0') &&
        (Local_Key <= '9'))
    {
        if (IDDigitCount <
            ENTRY_MAX_ID_DIGITS)
        {
            CurrentUserID =
                (CurrentUserID * 10UL) +
                (uint32_t)
                (Local_Key - '0');


            IDDigitCount++;


            Entry_DisplayCurrentID();
        }


        return;
    }


    /*
     * Clear ID.
     */
    if (Local_Key ==
        KEYPAD_KEY_CLEAR)
    {
        CurrentUserID = 0UL;

        IDDigitCount = 0U;


        Entry_DisplayCurrentID();


        return;
    }


    /*
     * Confirm ID.
     */
    if (Local_Key ==
        KEYPAD_KEY_CONFIRM)
    {
        if (IDDigitCount == 0U)
        {
            return;
        }


        Entry_DisplayScreen(
            ENTRY_SCREEN_CHECKING_ID);


        Entry_SendIDRequest();


        CurrentState =
            Entry_StateWaitIDResponse;


        return;
    }
}


/******************************************************************************
 * State: Wait ID Response
 ******************************************************************************/

static void Entry_StateWaitIDResponse(void)
{
    ParkingPacket_t Local_Response;


    if (ECU_Communication_GetPacket(
            &Local_Response) == false)
    {
        return;
    }


    switch (Local_Response.type)
    {
        case MSG_ID_VALID:

            RFIDAttempts = 0U;

            CurrentRFIDUID = 0UL;


            Entry_DisplayScreen(
                ENTRY_SCREEN_SCAN_RFID);


            CurrentState =
                Entry_StateWaitRFID;

            break;


        case MSG_INVALID_ID:

            Entry_DisplayScreen(
                ENTRY_SCREEN_ACCESS_DENIED);


            CurrentState =
                Entry_StateIdle;

            break;


        default:

            break;
    }
}


/******************************************************************************
 * State: Wait RFID
 ******************************************************************************/

static void Entry_StateWaitRFID(void)
{
    uint32_t Local_UID;


    if (HAL_RFID_ReadUID(
            &Local_UID) == false)
    {
        return;
    }


    CurrentRFIDUID =
        Local_UID;


    Entry_DisplayScreen(
        ENTRY_SCREEN_CHECKING_RFID);


    Entry_SendRFIDRequest();


    CurrentState =
        Entry_StateWaitRFIDResponse;
}


/******************************************************************************
 * State: Wait RFID Response
 ******************************************************************************/

static void Entry_StateWaitRFIDResponse(void)
{
    ParkingPacket_t Local_Response;


    if (ECU_Communication_GetPacket(
            &Local_Response) == false)
    {
        return;
    }


    switch (Local_Response.type)
    {
        case MSG_ACCESS_GRANTED:

            EntryCompleted = false;

            GateMovementStarted = false;


            Entry_DisplayScreen(
                ENTRY_SCREEN_ACCESS_GRANTED);


            CurrentState =
                Entry_StateOpenGate;

            break;


        case MSG_WRONG_CARD:

            RFIDAttempts++;


            if (RFIDAttempts >=
                MAX_RFID_ATTEMPTS)
            {
                Entry_DisplayScreen(
                    ENTRY_SCREEN_ACCESS_DENIED);


                CurrentState =
                    Entry_StateIdle;
            }
            else
            {
                CurrentRFIDUID = 0UL;


                Entry_DisplayScreen(
                    ENTRY_SCREEN_WRONG_CARD);


                CurrentState =
                    Entry_StateWaitRFID;
            }


            break;


        case MSG_PARKING_FULL:

            Entry_DisplayScreen(
                ENTRY_SCREEN_PARKING_FULL);


            CurrentState =
                Entry_StateIdle;

            break;


        case MSG_ALREADY_INSIDE:

            Entry_DisplayScreen(
                ENTRY_SCREEN_ALREADY_INSIDE);


            CurrentState =
                Entry_StateIdle;

            break;


        case MSG_ACCESS_DENIED:

            Entry_DisplayScreen(
                ENTRY_SCREEN_ACCESS_DENIED);


            CurrentState =
                Entry_StateIdle;

            break;


        default:

            break;
    }
}


/******************************************************************************
 * State: Admin PIN
 ******************************************************************************/

static void Entry_StateAdminPIN(void)
{
    char Local_Key;


    Local_Key =
        HAL_Keypad_GetKey();


    if (Local_Key == KEYPAD_KEY_NONE)
    {
        return;
    }


    /*
     * Numeric PIN input.
     */
    if ((Local_Key >= '0') &&
        (Local_Key <= '9'))
    {
        if (AdminPINDigitCount <
            ADMIN_PIN_MAX_DIGITS)
        {
            AdminPINValue =
                (AdminPINValue * 10UL) +
                (uint32_t)
                (Local_Key - '0');


            AdminPINDigitCount++;
        }


        return;
    }


    /*
     * Clear PIN.
     */
    if (Local_Key ==
        KEYPAD_KEY_CLEAR)
    {
        AdminPINValue = 0UL;

        AdminPINDigitCount = 0U;


        return;
    }


    /*
     * Confirm PIN.
     */
    if (Local_Key ==
        KEYPAD_KEY_CONFIRM)
    {
        if (AdminPINValue ==
            ADMIN_PIN)
        {
            CurrentUserID = 0UL;

            CurrentRFIDUID = 0UL;

            IDDigitCount = 0U;


            Entry_DisplayScreen(
                ENTRY_SCREEN_NEW_USER_ID);


            CurrentState =
                Entry_StateNewUserID;
        }
        else
        {
            Entry_DisplayScreen(
                ENTRY_SCREEN_INVALID_ADMIN_PIN);


            CurrentState =
                Entry_StateIdle;
        }


        return;
    }
}


/******************************************************************************
 * State: New User ID
 ******************************************************************************/

static void Entry_StateNewUserID(void)
{
    char Local_Key;


    Local_Key =
        HAL_Keypad_GetKey();


    if (Local_Key == KEYPAD_KEY_NONE)
    {
        return;
    }


    /*
     * Numeric User ID.
     */
    if ((Local_Key >= '0') &&
        (Local_Key <= '9'))
    {
        if (IDDigitCount <
            ENTRY_MAX_ID_DIGITS)
        {
            CurrentUserID =
                (CurrentUserID * 10UL) +
                (uint32_t)
                (Local_Key - '0');


            IDDigitCount++;


            Entry_DisplayCurrentID();
        }


        return;
    }


    /*
     * Clear new ID.
     */
    if (Local_Key ==
        KEYPAD_KEY_CLEAR)
    {
        CurrentUserID = 0UL;

        IDDigitCount = 0U;


        Entry_DisplayCurrentID();


        return;
    }


    /*
     * Confirm new ID.
     */
    if (Local_Key ==
        KEYPAD_KEY_CONFIRM)
    {
        if (IDDigitCount == 0U)
        {
            return;
        }


        CurrentRFIDUID = 0UL;


        Entry_DisplayScreen(
            ENTRY_SCREEN_NEW_USER_RFID);


        CurrentState =
            Entry_StateNewUserRFID;


        return;
    }
}


/******************************************************************************
 * State: Read New User RFID
 ******************************************************************************/

static void Entry_StateNewUserRFID(void)
{
    uint32_t Local_UID;


    if (HAL_RFID_ReadUID(
            &Local_UID) == false)
    {
        return;
    }


    CurrentRFIDUID =
        Local_UID;


    Entry_DisplayScreen(
        ENTRY_SCREEN_ADDING_USER);


    Entry_SendAddUserRequest();


    CurrentState =
        Entry_StateWaitAddUserResponse;
}


/******************************************************************************
 * State: Wait Add User Response
 ******************************************************************************/

static void Entry_StateWaitAddUserResponse(void)
{
    ParkingPacket_t Local_Response;


    if (ECU_Communication_GetPacket(
            &Local_Response) == false)
    {
        return;
    }


    switch (Local_Response.type)
    {
        case MSG_USER_ADDED:

            Entry_DisplayScreen(
                ENTRY_SCREEN_USER_ADDED);


            CurrentState =
                Entry_StateIdle;

            break;


        case MSG_USER_ALREADY_EXISTS:

            Entry_DisplayScreen(
                ENTRY_SCREEN_USER_EXISTS);


            CurrentState =
                Entry_StateIdle;

            break;


        case MSG_RFID_ALREADY_EXISTS:

            Entry_DisplayScreen(
                ENTRY_SCREEN_RFID_EXISTS);


            CurrentState =
                Entry_StateIdle;

            break;


        case MSG_DATABASE_FULL:

            Entry_DisplayScreen(
                ENTRY_SCREEN_DATABASE_FULL);


            CurrentState =
                Entry_StateIdle;

            break;


        case MSG_ACCESS_DENIED:

            Entry_DisplayScreen(
                ENTRY_SCREEN_ACCESS_DENIED);


            CurrentState =
                Entry_StateIdle;

            break;


        default:

            break;
    }
}


/******************************************************************************
 * State: Open Gate
 ******************************************************************************/

static void Entry_StateOpenGate(void)
{
    uint32_t Local_Now;


    Entry_DisplayScreen(
        ENTRY_SCREEN_ACCESS_GRANTED);


    if (GateMovementStarted == false)
    {
        HAL_Gate_Open();


        GateStartTime =
            MCAL_Timer0_GetMillis();


        GateMovementStarted = true;


        return;
    }


    Local_Now =
        MCAL_Timer0_GetMillis();


    if ((Local_Now - GateStartTime) >=
        GATE_MOVEMENT_TIME_MS)
    {
        HAL_Gate_Stop();


        GateMovementStarted = false;


        /*
         * Start vehicle arrival timeout.
         */
        GateStartTime =
            Local_Now;


        Entry_DisplayScreen(
            ENTRY_SCREEN_WAIT_VEHICLE);


        CurrentState =
            Entry_StateWaitVehicle;
    }
}


/******************************************************************************
 * State: Wait Vehicle
 ******************************************************************************/

static void Entry_StateWaitVehicle(void)
{
    uint32_t Local_Now;


    /*
     * Vehicle reached the gate.
     */
    if (HAL_IR_IsBlocked() == true)
    {
        Entry_DisplayScreen(
            ENTRY_SCREEN_WAIT_CLEAR);


        CurrentState =
            Entry_StateWaitClear;


        return;
    }


    Local_Now =
        MCAL_Timer0_GetMillis();


    /*
     * No vehicle arrived.
     */
    if ((Local_Now - GateStartTime) >=
        GATE_TIMEOUT_MS)
    {
        EntryCompleted = false;

        GateMovementStarted = false;


        Entry_DisplayScreen(
            ENTRY_SCREEN_TIMEOUT);


        CurrentState =
            Entry_StateCloseGate;
    }
}


/******************************************************************************
 * State: Wait IR Clear
 ******************************************************************************/

static void Entry_StateWaitClear(void)
{
    Entry_DisplayScreen(
        ENTRY_SCREEN_WAIT_CLEAR);


    /*
     * Never move the closing direction while vehicle
     * is blocking the IR sensor.
     */
    if (HAL_IR_IsBlocked() == true)
    {
        HAL_Gate_Stop();


        GateMovementStarted = false;


        return;
    }


    /*
     * BLOCKED -> CLEAR confirms vehicle passage.
     */
    EntryCompleted = true;

    GateMovementStarted = false;


    Entry_DisplayScreen(
        ENTRY_SCREEN_CLOSING);


    CurrentState =
        Entry_StateCloseGate;
}


/******************************************************************************
 * State: Close Gate
 ******************************************************************************/

static void Entry_StateCloseGate(void)
{
    uint32_t Local_Now;


    /*
     * SAFETY:
     * Never close while IR is blocked.
     */
    if (HAL_IR_IsBlocked() == true)
    {
        HAL_Gate_Stop();


        GateMovementStarted = false;


        Entry_DisplayScreen(
            ENTRY_SCREEN_WAIT_CLEAR);


        CurrentState =
            Entry_StateWaitClear;


        return;
    }


    /*
     * Start closing.
     */
    if (GateMovementStarted == false)
    {
        Entry_DisplayScreen(
            ENTRY_SCREEN_CLOSING);


        HAL_Gate_Close();


        GateStartTime =
            MCAL_Timer0_GetMillis();


        GateMovementStarted = true;


        return;
    }


    Local_Now =
        MCAL_Timer0_GetMillis();


    /*
     * Closing completed.
     */
    if ((Local_Now - GateStartTime) >=
        GATE_MOVEMENT_TIME_MS)
    {
        HAL_Gate_Stop();


        GateMovementStarted = false;


        if (EntryCompleted == true)
        {
            CurrentState =
                Entry_StateComplete;
        }
        else
        {
            /*
             * Timeout:
             * no ENTRY_COMPLETED message.
             */
            CurrentState =
                Entry_StateIdle;
        }
    }
}


/******************************************************************************
 * State: Entry Complete
 ******************************************************************************/

static void Entry_StateComplete(void)
{
    Entry_DisplayScreen(
        ENTRY_SCREEN_COMPLETE);


    /*
     * Notify Central ECU only after actual vehicle passage.
     */
    Entry_SendCompletion();


    CurrentState =
        Entry_StateIdle;
}


/******************************************************************************
 * Reset Current Session
 ******************************************************************************/

static void Entry_ResetSession(void)
{
    CurrentUserID = 0UL;

    CurrentRFIDUID = 0UL;


    IDDigitCount = 0U;

    RFIDAttempts = 0U;


    AdminPINValue = 0UL;

    AdminPINDigitCount = 0U;


    GateStartTime = 0UL;


    EntryCompleted = false;

    GateMovementStarted = false;
}


/******************************************************************************
 * Send ID Authentication Request
 ******************************************************************************/

static void Entry_SendIDRequest(void)
{
    ParkingPacket_t Local_Request;


    Local_Request.type =
        MSG_AUTH_ID_REQ;


    Local_Request.source =
        SOURCE_ENTRY;


    Local_Request.userID =
        CurrentUserID;


    Local_Request.rfidUID =
        0UL;


    (void)ECU_Communication_SendPacket(
        &Local_Request);
}


/******************************************************************************
 * Send RFID Authentication Request
 ******************************************************************************/

static void Entry_SendRFIDRequest(void)
{
    ParkingPacket_t Local_Request;


    Local_Request.type =
        MSG_AUTH_RFID_REQ;


    Local_Request.source =
        SOURCE_ENTRY;


    Local_Request.userID =
        CurrentUserID;


    Local_Request.rfidUID =
        CurrentRFIDUID;


    (void)ECU_Communication_SendPacket(
        &Local_Request);
}


/******************************************************************************
 * Send Add User Request
 ******************************************************************************/

static void Entry_SendAddUserRequest(void)
{
    ParkingPacket_t Local_Request;


    Local_Request.type =
        MSG_ADD_USER_REQ;


    Local_Request.source =
        SOURCE_ENTRY;


    Local_Request.userID =
        CurrentUserID;


    Local_Request.rfidUID =
        CurrentRFIDUID;


    (void)ECU_Communication_SendPacket(
        &Local_Request);
}


/******************************************************************************
 * Send Entry Completion
 ******************************************************************************/

static void Entry_SendCompletion(void)
{
    ParkingPacket_t Local_Request;


    Local_Request.type =
        MSG_ENTRY_COMPLETED;


    Local_Request.source =
        SOURCE_ENTRY;


    Local_Request.userID =
        CurrentUserID;


    Local_Request.rfidUID =
        CurrentRFIDUID;


    (void)ECU_Communication_SendPacket(
        &Local_Request);
}


/******************************************************************************
 * LCD Screen Renderer
 ******************************************************************************/

static void Entry_DisplayScreen(EntryScreen_t screen)
{
    /*
     * Prevent continuous LCD clear/rewrite.
     */
    if (CurrentScreen == screen)
    {
        return;
    }


    CurrentScreen = screen;


    HAL_LCD_Clear();


    switch (screen)
    {
        /* =================================================
         * MODE SELECTION
         * ================================================= */

        case ENTRY_SCREEN_SELECT_MODE:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "*:Enter #:Add");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Select Mode");

            break;


        /* =================================================
         * NORMAL ENTRY
         * ================================================= */

        case ENTRY_SCREEN_ENTER_ID:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Enter User ID");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "ID:");

            break;


        case ENTRY_SCREEN_CHECKING_ID:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Checking ID...");

            break;


        case ENTRY_SCREEN_SCAN_RFID:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Scan RFID Card");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Waiting...");

            break;


        case ENTRY_SCREEN_CHECKING_RFID:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Checking Card");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Please Wait");

            break;


        case ENTRY_SCREEN_WRONG_CARD:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Wrong Card");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Try ");


            HAL_LCD_WriteNumber(
                (uint32_t)
                (RFIDAttempts + 1U));


            HAL_LCD_WriteString("/");


            HAL_LCD_WriteNumber(
                (uint32_t)
                MAX_RFID_ATTEMPTS);

            break;


        case ENTRY_SCREEN_ACCESS_DENIED:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Access Denied");

            break;


        case ENTRY_SCREEN_PARKING_FULL:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Parking Full");

            break;


        case ENTRY_SCREEN_ALREADY_INSIDE:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Already Inside");

            break;


        case ENTRY_SCREEN_ACCESS_GRANTED:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Access Granted");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Gate Opening");

            break;


        /* =================================================
         * ADMIN / ADD USER
         * ================================================= */

        case ENTRY_SCREEN_ADMIN_PIN:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Admin PIN:");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Enter + #");

            break;


        case ENTRY_SCREEN_INVALID_ADMIN_PIN:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Invalid PIN");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Access Denied");

            break;


        case ENTRY_SCREEN_NEW_USER_ID:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "New User ID");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "ID:");

            break;


        case ENTRY_SCREEN_NEW_USER_RFID:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Scan New Card");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Waiting...");

            break;


        case ENTRY_SCREEN_ADDING_USER:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Adding User...");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Please Wait");

            break;


        case ENTRY_SCREEN_USER_ADDED:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "User Added");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Successfully");

            break;


        case ENTRY_SCREEN_USER_EXISTS:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "User Exists");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Not Added");

            break;
		
        case ENTRY_SCREEN_RFID_EXISTS:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "RFID Exists");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Use New Card");

            break;


        case ENTRY_SCREEN_DATABASE_FULL:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Database Full");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Cannot Add");

            break;


        /* =================================================
         * GATE
         * ================================================= */

        case ENTRY_SCREEN_WAIT_VEHICLE:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Drive Forward");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Gate Open");

            break;


        case ENTRY_SCREEN_WAIT_CLEAR:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Please Continue");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Gate Open");

            break;


        case ENTRY_SCREEN_TIMEOUT:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Entry Timeout");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Gate Closing");

            break;


        case ENTRY_SCREEN_CLOSING:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Gate Closing");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Please Wait");

            break;


        case ENTRY_SCREEN_COMPLETE:

            HAL_LCD_SetCursor(0U, 0U);

            HAL_LCD_WriteString(
                "Entry Complete");


            HAL_LCD_SetCursor(1U, 0U);

            HAL_LCD_WriteString(
                "Welcome");

            break;


        case ENTRY_SCREEN_NONE:
        default:

            break;
    }
}


/******************************************************************************
 * Display Current User ID
 ******************************************************************************/

static void Entry_DisplayCurrentID(void)
{
    /*
     * Clear second LCD row.
     */
    HAL_LCD_SetCursor(1U, 0U);

    HAL_LCD_WriteString(
        "                ");


    /*
     * Display current ID.
     */
    HAL_LCD_SetCursor(1U, 0U);

    HAL_LCD_WriteString(
        "ID:");


    if (IDDigitCount > 0U)
    {
        HAL_LCD_WriteNumber(
            CurrentUserID);
    }
}
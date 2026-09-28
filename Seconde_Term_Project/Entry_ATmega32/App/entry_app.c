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
 * Implements the Entry ECU application state machine and user display flow.
 *
 * Responsibilities:
 * - Read and build User ID from keypad input.
 * - Request ID validation from the STM32 Central ECU.
 * - Read RFID UID and request access validation.
 * - Allow a maximum of three wrong RFID attempts per session.
 * - Drive the Entry LCD according to the active application state.
 * - Open and close the entry gate without blocking application delays.
 * - Detect actual vehicle passage using the IR sensor.
 * - Never close the gate while the IR path is blocked.
 * - Abort entry if no vehicle arrives within the configured timeout.
 * - Notify the Central ECU only after a completed vehicle passage.
 ******************************************************************************/

#include "entry_app.h"
#include "ecu_communication.h"

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

#define GATE_TIMEOUT_MS           10000UL
#define GATE_MOVEMENT_TIME_MS     2000UL


/******************************************************************************
 * State Handler Type
 ******************************************************************************/

typedef void (*EntryStateHandler_t)(void);


/******************************************************************************
 * Display Screen Type
 ******************************************************************************/

typedef enum
{
    ENTRY_SCREEN_NONE = 0,

    ENTRY_SCREEN_ENTER_ID,
    ENTRY_SCREEN_CHECKING_ID,
    ENTRY_SCREEN_SCAN_RFID,
    ENTRY_SCREEN_CHECKING_RFID,
    ENTRY_SCREEN_WRONG_CARD,
    ENTRY_SCREEN_ACCESS_DENIED,
    ENTRY_SCREEN_PARKING_FULL,
    ENTRY_SCREEN_ALREADY_INSIDE,
    ENTRY_SCREEN_ACCESS_GRANTED,
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
static void Entry_StateWaitID(void);
static void Entry_StateWaitIDResponse(void);
static void Entry_StateWaitRFID(void);
static void Entry_StateWaitRFIDResponse(void);
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
static void Entry_SendCompletion(void);

static void Entry_DisplayScreen(EntryScreen_t screen);
static void Entry_DisplayCurrentID(void);


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static EntryStateHandler_t CurrentState = NULL;

/* Current entry session data. */
static uint32_t CurrentUserID = 0UL;
static uint32_t CurrentRFIDUID = 0UL;

/* Number of User ID digits entered through the keypad. */
static uint8_t IDDigitCount = 0U;

/* Wrong RFID attempts during the current session. */
static uint8_t RFIDAttempts = 0U;

/* Time reference used for gate movement and vehicle-arrival timeout. */
static uint32_t GateStartTime = 0UL;

/*
 * True only after a vehicle has blocked the IR sensor and then completely
 * cleared it. A timeout must never set this flag.
 */
static bool EntryCompleted = false;

/*
 * Shared movement flag for gate opening and closing.
 */
static bool GateMovementStarted = false;

/*
 * Last LCD screen already rendered.
 * Prevents clearing and rewriting the LCD on every scheduler iteration.
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
     * Timer0 provides the 1 ms system time base.
     * Global interrupts shall be enabled from main() using sei().
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

    Entry_DisplayScreen(ENTRY_SCREEN_ENTER_ID);

    CurrentState = Entry_StateWaitID;
}


/******************************************************************************
 * State: Wait ID
 ******************************************************************************/

static void Entry_StateWaitID(void)
{
    char Local_Key;

    Local_Key = HAL_Keypad_GetKey();

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
        if (IDDigitCount < ENTRY_MAX_ID_DIGITS)
        {
            CurrentUserID =
                (CurrentUserID * 10UL) +
                (uint32_t)(Local_Key - '0');

            IDDigitCount++;

            Entry_DisplayCurrentID();
        }

        return;
    }


    /*
     * '*' clears the currently entered ID.
     */
    if (Local_Key == KEYPAD_KEY_CLEAR)
    {
        CurrentUserID = 0UL;
        IDDigitCount = 0U;

        Entry_DisplayCurrentID();

        return;
    }


    /*
     * '#' confirms the User ID.
     */
    if (Local_Key == KEYPAD_KEY_CONFIRM)
    {
        if (IDDigitCount == 0U)
        {
            return;
        }

        Entry_DisplayScreen(ENTRY_SCREEN_CHECKING_ID);

        Entry_SendIDRequest();

        CurrentState = Entry_StateWaitIDResponse;

        return;
    }


    /*
     * Keys A, B, C and D are unused.
     */
}


/******************************************************************************
 * State: Wait ID Response
 ******************************************************************************/

static void Entry_StateWaitIDResponse(void)
{
    ParkingPacket_t Local_Response;

    if (ECU_Communication_GetPacket(&Local_Response) == false)
    {
        return;
    }


    switch (Local_Response.type)
    {
        case MSG_ID_VALID:

            RFIDAttempts = 0U;
            CurrentRFIDUID = 0UL;

            Entry_DisplayScreen(ENTRY_SCREEN_SCAN_RFID);

            CurrentState = Entry_StateWaitRFID;

            break;


        case MSG_INVALID_ID:

            Entry_DisplayScreen(ENTRY_SCREEN_ACCESS_DENIED);

            CurrentState = Entry_StateIdle;

            break;


        default:

            /*
             * Ignore messages that are not relevant to this state.
             */
            break;
    }
}


/******************************************************************************
 * State: Wait RFID
 ******************************************************************************/

static void Entry_StateWaitRFID(void)
{
    uint32_t Local_UID;

    if (HAL_RFID_ReadUID(&Local_UID) == false)
    {
        return;
    }

    CurrentRFIDUID = Local_UID;

    Entry_DisplayScreen(ENTRY_SCREEN_CHECKING_RFID);

    Entry_SendRFIDRequest();

    CurrentState = Entry_StateWaitRFIDResponse;
}


/******************************************************************************
 * State: Wait RFID Response
 ******************************************************************************/

static void Entry_StateWaitRFIDResponse(void)
{
    ParkingPacket_t Local_Response;

    if (ECU_Communication_GetPacket(&Local_Response) == false)
    {
        return;
    }


    switch (Local_Response.type)
    {
        case MSG_ACCESS_GRANTED:

            EntryCompleted = false;
            GateMovementStarted = false;

            Entry_DisplayScreen(ENTRY_SCREEN_ACCESS_GRANTED);

            CurrentState = Entry_StateOpenGate;

            break;


        case MSG_WRONG_CARD:

            RFIDAttempts++;

            if (RFIDAttempts >= MAX_RFID_ATTEMPTS)
            {
                Entry_DisplayScreen(ENTRY_SCREEN_ACCESS_DENIED);

                CurrentState = Entry_StateIdle;
            }
            else
            {
                CurrentRFIDUID = 0UL;

                Entry_DisplayScreen(ENTRY_SCREEN_WRONG_CARD);

                CurrentState = Entry_StateWaitRFID;
            }

            break;


        case MSG_PARKING_FULL:

            Entry_DisplayScreen(ENTRY_SCREEN_PARKING_FULL);

            CurrentState = Entry_StateIdle;

            break;


        case MSG_ALREADY_INSIDE:

            Entry_DisplayScreen(ENTRY_SCREEN_ALREADY_INSIDE);

            CurrentState = Entry_StateIdle;

            break;


        case MSG_ACCESS_DENIED:

            Entry_DisplayScreen(ENTRY_SCREEN_ACCESS_DENIED);

            CurrentState = Entry_StateIdle;

            break;


        default:

            /*
             * Ignore messages that are not relevant to this state.
             */
            break;
    }
}


/******************************************************************************
 * State: Open Gate
 ******************************************************************************/

static void Entry_StateOpenGate(void)
{
    uint32_t Local_Now;

    Entry_DisplayScreen(ENTRY_SCREEN_ACCESS_GRANTED);


    if (GateMovementStarted == false)
    {
        HAL_Gate_Open();

        GateStartTime = MCAL_Timer0_GetMillis();

        GateMovementStarted = true;

        return;
    }


    Local_Now = MCAL_Timer0_GetMillis();


    if ((Local_Now - GateStartTime) >= GATE_MOVEMENT_TIME_MS)
    {
        HAL_Gate_Stop();

        GateMovementStarted = false;

        /*
         * Start the vehicle-arrival timeout after opening is complete.
         */
        GateStartTime = Local_Now;

        Entry_DisplayScreen(ENTRY_SCREEN_WAIT_VEHICLE);

        CurrentState = Entry_StateWaitVehicle;
    }
}


/******************************************************************************
 * State: Wait Vehicle
 ******************************************************************************/

static void Entry_StateWaitVehicle(void)
{
    uint32_t Local_Now;


    if (HAL_IR_IsBlocked() == true)
    {
        Entry_DisplayScreen(ENTRY_SCREEN_WAIT_CLEAR);

        CurrentState = Entry_StateWaitClear;

        return;
    }


    Local_Now = MCAL_Timer0_GetMillis();


    if ((Local_Now - GateStartTime) >= GATE_TIMEOUT_MS)
    {
        /*
         * No vehicle reached the gate.
         * Close without reporting entry completion.
         */
        EntryCompleted = false;
        GateMovementStarted = false;

        Entry_DisplayScreen(ENTRY_SCREEN_TIMEOUT);

        CurrentState = Entry_StateCloseGate;
    }
}


/******************************************************************************
 * State: Wait Clear
 ******************************************************************************/

static void Entry_StateWaitClear(void)
{
    Entry_DisplayScreen(ENTRY_SCREEN_WAIT_CLEAR);


    /*
     * The gate must remain stopped/open while the path is blocked.
     */
    if (HAL_IR_IsBlocked() == true)
    {
        HAL_Gate_Stop();

        GateMovementStarted = false;

        return;
    }


    /*
     * Reaching this point after WaitClear means a BLOCKED -> CLEAR
     * sequence occurred, confirming vehicle passage.
     */
    EntryCompleted = true;
    GateMovementStarted = false;

    Entry_DisplayScreen(ENTRY_SCREEN_CLOSING);

    CurrentState = Entry_StateCloseGate;
}


/******************************************************************************
 * State: Close Gate
 ******************************************************************************/

static void Entry_StateCloseGate(void)
{
    uint32_t Local_Now;


    /*
     * Safety rule:
     * NEVER close the gate while the IR path is blocked.
     *
     * This check is performed before starting the motor and on every
     * scheduler iteration while the gate is closing.
     */
    if (HAL_IR_IsBlocked() == true)
    {
        HAL_Gate_Stop();

        GateMovementStarted = false;

        Entry_DisplayScreen(ENTRY_SCREEN_WAIT_CLEAR);

        CurrentState = Entry_StateWaitClear;

        return;
    }


    if (GateMovementStarted == false)
    {
        Entry_DisplayScreen(ENTRY_SCREEN_CLOSING);

        HAL_Gate_Close();

        GateStartTime = MCAL_Timer0_GetMillis();

        GateMovementStarted = true;

        return;
    }


    Local_Now = MCAL_Timer0_GetMillis();


    if ((Local_Now - GateStartTime) >= GATE_MOVEMENT_TIME_MS)
    {
        HAL_Gate_Stop();

        GateMovementStarted = false;


        if (EntryCompleted == true)
        {
            CurrentState = Entry_StateComplete;
        }
        else
        {
            /*
             * Timeout path:
             * gate is closed and no MSG_ENTRY_COMPLETED is sent.
             */
            CurrentState = Entry_StateIdle;
        }
    }
}


/******************************************************************************
 * State: Complete
 ******************************************************************************/

static void Entry_StateComplete(void)
{
    Entry_DisplayScreen(ENTRY_SCREEN_COMPLETE);

    /*
     * Notify the STM32 only after real vehicle passage and gate closure.
     */
    Entry_SendCompletion();

    /*
     * The Central ECU can now persist USER_INSIDE.
     */
    CurrentState = Entry_StateIdle;
}


/******************************************************************************
 * Helper: Reset Session
 ******************************************************************************/

static void Entry_ResetSession(void)
{
    CurrentUserID = 0UL;
    CurrentRFIDUID = 0UL;

    IDDigitCount = 0U;
    RFIDAttempts = 0U;

    GateStartTime = 0UL;

    EntryCompleted = false;
    GateMovementStarted = false;
}


/******************************************************************************
 * Helper: Send ID Authentication Request
 ******************************************************************************/

static void Entry_SendIDRequest(void)
{
    ParkingPacket_t Local_Request;

    Local_Request.type = MSG_AUTH_ID_REQ;
    Local_Request.source = SOURCE_ENTRY;
    Local_Request.userID = CurrentUserID;
    Local_Request.rfidUID = 0UL;

    (void)ECU_Communication_SendPacket(&Local_Request);
}


/******************************************************************************
 * Helper: Send RFID Authentication Request
 ******************************************************************************/

static void Entry_SendRFIDRequest(void)
{
    ParkingPacket_t Local_Request;

    Local_Request.type = MSG_AUTH_RFID_REQ;
    Local_Request.source = SOURCE_ENTRY;
    Local_Request.userID = CurrentUserID;
    Local_Request.rfidUID = CurrentRFIDUID;

    (void)ECU_Communication_SendPacket(&Local_Request);
}


/******************************************************************************
 * Helper: Send Entry Completion
 ******************************************************************************/

static void Entry_SendCompletion(void)
{
    ParkingPacket_t Local_Request;

    Local_Request.type = MSG_ENTRY_COMPLETED;
    Local_Request.source = SOURCE_ENTRY;
    Local_Request.userID = CurrentUserID;
    Local_Request.rfidUID = CurrentRFIDUID;

    (void)ECU_Communication_SendPacket(&Local_Request);
}


/******************************************************************************
 * Helper: Render LCD Screen
 ******************************************************************************/

static void Entry_DisplayScreen(EntryScreen_t screen)
{
    /*
     * Do not continuously clear/rewrite the LCD while Update() repeatedly
     * executes the same state.
     */
    if (CurrentScreen == screen)
    {
        return;
    }


    CurrentScreen = screen;

    HAL_LCD_Clear();


    switch (screen)
    {
        case ENTRY_SCREEN_ENTER_ID:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Enter User ID");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("ID:");

            break;


        case ENTRY_SCREEN_CHECKING_ID:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Checking ID...");

            break;


        case ENTRY_SCREEN_SCAN_RFID:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Scan RFID Card");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Waiting...");

            break;


        case ENTRY_SCREEN_CHECKING_RFID:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Checking Card");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Please Wait");

            break;


        case ENTRY_SCREEN_WRONG_CARD:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Wrong Card");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Try ");

            HAL_LCD_WriteNumber((uint32_t)(RFIDAttempts + 1U));

            HAL_LCD_WriteString("/");

            HAL_LCD_WriteNumber((uint32_t)MAX_RFID_ATTEMPTS);

            break;


        case ENTRY_SCREEN_ACCESS_DENIED:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Access Denied");

            break;


        case ENTRY_SCREEN_PARKING_FULL:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Parking Full");

            break;


        case ENTRY_SCREEN_ALREADY_INSIDE:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Already Inside");

            break;


        case ENTRY_SCREEN_ACCESS_GRANTED:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Access Granted");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Opening");

            break;


        case ENTRY_SCREEN_WAIT_VEHICLE:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Drive Forward");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Open");

            break;


        case ENTRY_SCREEN_WAIT_CLEAR:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Please Continue");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Open");

            break;


        case ENTRY_SCREEN_TIMEOUT:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Entry Timeout");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Closing");

            break;


        case ENTRY_SCREEN_CLOSING:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Gate Closing");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Please Wait");

            break;


        case ENTRY_SCREEN_COMPLETE:

            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Entry Complete");

            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Welcome");

            break;


        case ENTRY_SCREEN_NONE:
        default:

            break;
    }
}


/******************************************************************************
 * Helper: Display Current User ID
 ******************************************************************************/

static void Entry_DisplayCurrentID(void)
{
    /*
     * Rebuild the second LCD line so deleting the ID also removes old digits.
     */
    HAL_LCD_SetCursor(1U, 0U);
    HAL_LCD_WriteString("                ");

    HAL_LCD_SetCursor(1U, 0U);
    HAL_LCD_WriteString("ID:");

    if (IDDigitCount > 0U)
    {
        HAL_LCD_WriteNumber(CurrentUserID);
    }
}

/******************************************************************************
 * @file        exit_app.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Exit ECU
 * @mcu         ATmega32
 * @layer       Application Layer
 * @module      Exit Application
 *
 * @path        Exit_ATmega32/App/exit_app.c
 *
 * @brief
 * Implements the Exit ECU application state machine.
 ******************************************************************************/

#include "exit_app.h"
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

#define EXIT_MAX_ID_DIGITS        9U
#define GATE_TIMEOUT_MS           10000UL
#define GATE_MOVEMENT_TIME_MS     2000UL

typedef void (*ExitStateHandler_t)(void);

typedef enum
{
    EXIT_SCREEN_NONE = 0,
    EXIT_SCREEN_ENTER_ID,
    EXIT_SCREEN_CHECKING_ID,
    EXIT_SCREEN_SCAN_RFID,
    EXIT_SCREEN_CHECKING_RFID,
    EXIT_SCREEN_WRONG_CARD,
    EXIT_SCREEN_ACCESS_DENIED,
    EXIT_SCREEN_INVALID_EXIT,
    EXIT_SCREEN_ACCESS_GRANTED,
    EXIT_SCREEN_WAIT_VEHICLE,
    EXIT_SCREEN_WAIT_CLEAR,
    EXIT_SCREEN_TIMEOUT,
    EXIT_SCREEN_CLOSING,
    EXIT_SCREEN_COMPLETE
} ExitScreen_t;

static void Exit_StateIdle(void);
static void Exit_StateWaitID(void);
static void Exit_StateWaitIDResponse(void);
static void Exit_StateWaitRFID(void);
static void Exit_StateWaitRFIDResponse(void);
static void Exit_StateOpenGate(void);
static void Exit_StateWaitVehicle(void);
static void Exit_StateWaitClear(void);
static void Exit_StateCloseGate(void);
static void Exit_StateComplete(void);

static void Exit_ResetSession(void);
static void Exit_SendIDRequest(void);
static void Exit_SendRFIDRequest(void);
static void Exit_SendCompletion(void);
static void Exit_DisplayScreen(ExitScreen_t screen);
static void Exit_DisplayCurrentID(void);

static ExitStateHandler_t CurrentState = NULL;
static uint32_t CurrentUserID = 0UL;
static uint32_t CurrentRFIDUID = 0UL;
static uint8_t IDDigitCount = 0U;
static uint8_t RFIDAttempts = 0U;
static uint32_t GateStartTime = 0UL;
static bool ExitCompleted = false;
static bool GateMovementStarted = false;
static ExitScreen_t CurrentScreen = EXIT_SCREEN_NONE;

void ExitApp_Init(void)
{
    ECU_Communication_Init();
    HAL_Keypad_Init();
    HAL_RFID_Init();
    HAL_Gate_Init();
    HAL_IR_Init();
    HAL_LCD_Init();
    MCAL_Timer0_Init();

    Exit_ResetSession();
    CurrentScreen = EXIT_SCREEN_NONE;
    CurrentState = Exit_StateIdle;
}

void ExitApp_Update(void)
{
    if (CurrentState != NULL)
    {
        CurrentState();
    }
}

static void Exit_StateIdle(void)
{
    Exit_ResetSession();
    Exit_DisplayScreen(EXIT_SCREEN_ENTER_ID);
    CurrentState = Exit_StateWaitID;
}

static void Exit_StateWaitID(void)
{
    char Local_Key = HAL_Keypad_GetKey();

    if (Local_Key == KEYPAD_KEY_NONE)
    {
        return;
    }

    if ((Local_Key >= '0') && (Local_Key <= '9'))
    {
        if (IDDigitCount < EXIT_MAX_ID_DIGITS)
        {
            CurrentUserID = (CurrentUserID * 10UL) +
                            (uint32_t)(Local_Key - '0');
            IDDigitCount++;
            Exit_DisplayCurrentID();
        }
        return;
    }

    if (Local_Key == KEYPAD_KEY_CLEAR)
    {
        CurrentUserID = 0UL;
        IDDigitCount = 0U;
        Exit_DisplayCurrentID();
        return;
    }

    if (Local_Key == KEYPAD_KEY_CONFIRM)
    {
        if (IDDigitCount == 0U)
        {
            return;
        }

        Exit_DisplayScreen(EXIT_SCREEN_CHECKING_ID);
        Exit_SendIDRequest();
        CurrentState = Exit_StateWaitIDResponse;
    }
}

static void Exit_StateWaitIDResponse(void)
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
            Exit_DisplayScreen(EXIT_SCREEN_SCAN_RFID);
            CurrentState = Exit_StateWaitRFID;
            break;

        case MSG_INVALID_ID:
            Exit_DisplayScreen(EXIT_SCREEN_ACCESS_DENIED);
            CurrentState = Exit_StateIdle;
            break;

        default:
            break;
    }
}

static void Exit_StateWaitRFID(void)
{
    uint32_t Local_UID;

    if (HAL_RFID_ReadUID(&Local_UID) == false)
    {
        return;
    }

    CurrentRFIDUID = Local_UID;
    Exit_DisplayScreen(EXIT_SCREEN_CHECKING_RFID);
    Exit_SendRFIDRequest();
    CurrentState = Exit_StateWaitRFIDResponse;
}

static void Exit_StateWaitRFIDResponse(void)
{
    ParkingPacket_t Local_Response;

    if (ECU_Communication_GetPacket(&Local_Response) == false)
    {
        return;
    }

    switch (Local_Response.type)
    {
        case MSG_ACCESS_GRANTED:
            ExitCompleted = false;
            GateMovementStarted = false;
            Exit_DisplayScreen(EXIT_SCREEN_ACCESS_GRANTED);
            CurrentState = Exit_StateOpenGate;
            break;

        case MSG_WRONG_CARD:
            RFIDAttempts++;

            if (RFIDAttempts >= MAX_RFID_ATTEMPTS)
            {
                Exit_DisplayScreen(EXIT_SCREEN_ACCESS_DENIED);
                CurrentState = Exit_StateIdle;
            }
            else
            {
                CurrentRFIDUID = 0UL;
                Exit_DisplayScreen(EXIT_SCREEN_WRONG_CARD);
                CurrentState = Exit_StateWaitRFID;
            }
            break;

        case MSG_INVALID_EXIT:
            Exit_DisplayScreen(EXIT_SCREEN_INVALID_EXIT);
            CurrentState = Exit_StateIdle;
            break;

        case MSG_ACCESS_DENIED:
            Exit_DisplayScreen(EXIT_SCREEN_ACCESS_DENIED);
            CurrentState = Exit_StateIdle;
            break;

        default:
            break;
    }
}

static void Exit_StateOpenGate(void)
{
    uint32_t Local_Now;

    Exit_DisplayScreen(EXIT_SCREEN_ACCESS_GRANTED);

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
        GateStartTime = Local_Now;
        Exit_DisplayScreen(EXIT_SCREEN_WAIT_VEHICLE);
        CurrentState = Exit_StateWaitVehicle;
    }
}

static void Exit_StateWaitVehicle(void)
{
    uint32_t Local_Now;

    if (HAL_IR_IsBlocked() == true)
    {
        Exit_DisplayScreen(EXIT_SCREEN_WAIT_CLEAR);
        CurrentState = Exit_StateWaitClear;
        return;
    }

    Local_Now = MCAL_Timer0_GetMillis();

    if ((Local_Now - GateStartTime) >= GATE_TIMEOUT_MS)
    {
        ExitCompleted = false;
        GateMovementStarted = false;
        Exit_DisplayScreen(EXIT_SCREEN_TIMEOUT);
        CurrentState = Exit_StateCloseGate;
    }
}

static void Exit_StateWaitClear(void)
{
    Exit_DisplayScreen(EXIT_SCREEN_WAIT_CLEAR);

    if (HAL_IR_IsBlocked() == true)
    {
        HAL_Gate_Stop();
        GateMovementStarted = false;
        return;
    }

    ExitCompleted = true;
    GateMovementStarted = false;
    Exit_DisplayScreen(EXIT_SCREEN_CLOSING);
    CurrentState = Exit_StateCloseGate;
}

static void Exit_StateCloseGate(void)
{
    uint32_t Local_Now;

    /* Never close while a vehicle blocks the IR path. */
    if (HAL_IR_IsBlocked() == true)
    {
        HAL_Gate_Stop();
        GateMovementStarted = false;
        Exit_DisplayScreen(EXIT_SCREEN_WAIT_CLEAR);
        CurrentState = Exit_StateWaitClear;
        return;
    }

    if (GateMovementStarted == false)
    {
        Exit_DisplayScreen(EXIT_SCREEN_CLOSING);
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

        if (ExitCompleted == true)
        {
            CurrentState = Exit_StateComplete;
        }
        else
        {
            CurrentState = Exit_StateIdle;
        }
    }
}

static void Exit_StateComplete(void)
{
    Exit_DisplayScreen(EXIT_SCREEN_COMPLETE);
    Exit_SendCompletion();
    CurrentState = Exit_StateIdle;
}

static void Exit_ResetSession(void)
{
    CurrentUserID = 0UL;
    CurrentRFIDUID = 0UL;
    IDDigitCount = 0U;
    RFIDAttempts = 0U;
    GateStartTime = 0UL;
    ExitCompleted = false;
    GateMovementStarted = false;
}

static void Exit_SendIDRequest(void)
{
    ParkingPacket_t Local_Request;

    Local_Request.type = MSG_AUTH_ID_REQ;
    Local_Request.source = SOURCE_EXIT;
    Local_Request.userID = CurrentUserID;
    Local_Request.rfidUID = 0UL;

    (void)ECU_Communication_SendPacket(&Local_Request);
}

static void Exit_SendRFIDRequest(void)
{
    ParkingPacket_t Local_Request;

    Local_Request.type = MSG_AUTH_RFID_REQ;
    Local_Request.source = SOURCE_EXIT;
    Local_Request.userID = CurrentUserID;
    Local_Request.rfidUID = CurrentRFIDUID;

    (void)ECU_Communication_SendPacket(&Local_Request);
}

static void Exit_SendCompletion(void)
{
    ParkingPacket_t Local_Request;

    Local_Request.type = MSG_EXIT_COMPLETED;
    Local_Request.source = SOURCE_EXIT;
    Local_Request.userID = CurrentUserID;
    Local_Request.rfidUID = CurrentRFIDUID;

    (void)ECU_Communication_SendPacket(&Local_Request);
}

static void Exit_DisplayScreen(ExitScreen_t screen)
{
    if (CurrentScreen == screen)
    {
        return;
    }

    CurrentScreen = screen;
    HAL_LCD_Clear();

    switch (screen)
    {
        case EXIT_SCREEN_ENTER_ID:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Enter User ID");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("ID:");
            break;

        case EXIT_SCREEN_CHECKING_ID:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Checking ID...");
            break;

        case EXIT_SCREEN_SCAN_RFID:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Scan RFID Card");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Waiting...");
            break;

        case EXIT_SCREEN_CHECKING_RFID:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Checking Card");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Please Wait");
            break;

        case EXIT_SCREEN_WRONG_CARD:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Wrong Card");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Try ");
            HAL_LCD_WriteNumber((uint32_t)(RFIDAttempts + 1U));
            HAL_LCD_WriteString("/");
            HAL_LCD_WriteNumber((uint32_t)MAX_RFID_ATTEMPTS);
            break;

        case EXIT_SCREEN_ACCESS_DENIED:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Access Denied");
            break;

        case EXIT_SCREEN_INVALID_EXIT:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Invalid Exit");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Not Inside");
            break;

        case EXIT_SCREEN_ACCESS_GRANTED:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Exit Granted");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Opening");
            break;

        case EXIT_SCREEN_WAIT_VEHICLE:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Drive Forward");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Open");
            break;

        case EXIT_SCREEN_WAIT_CLEAR:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Please Continue");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Open");
            break;

        case EXIT_SCREEN_TIMEOUT:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Exit Timeout");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Gate Closing");
            break;

        case EXIT_SCREEN_CLOSING:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Gate Closing");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Please Wait");
            break;

        case EXIT_SCREEN_COMPLETE:
            HAL_LCD_SetCursor(0U, 0U);
            HAL_LCD_WriteString("Exit Complete");
            HAL_LCD_SetCursor(1U, 0U);
            HAL_LCD_WriteString("Goodbye");
            break;

        case EXIT_SCREEN_NONE:
        default:
            break;
    }
}

static void Exit_DisplayCurrentID(void)
{
    HAL_LCD_SetCursor(1U, 0U);
    HAL_LCD_WriteString("                ");

    HAL_LCD_SetCursor(1U, 0U);
    HAL_LCD_WriteString("ID:");

    if (IDDigitCount > 0U)
    {
        HAL_LCD_WriteNumber(CurrentUserID);
    }
}

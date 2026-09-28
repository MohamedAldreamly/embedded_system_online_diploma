/******************************************************************************
 * @file        communication.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         STM32 Central ECU
 * @layer       Application Layer
 * @module      Communication Manager
 *
 * @path        STM32_Central/App/communication.c
 *
 * @brief
 * Communication manager between the STM32 Central ECU and the
 * Entry/Exit ATmega32 ECUs.
 ******************************************************************************/

#include "communication.h"
#include "access_app.h"

#include "Stm32_F103C6_USART_driver.h"

#include <stddef.h>


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static UART_Config_t Entry_UART_Config;
static UART_Config_t Exit_UART_Config;


/* Entry USART1 reception. */

static volatile uint8_t Entry_RxBuffer[PARKING_PROTOCOL_FRAME_SIZE];
static volatile uint8_t Entry_RxIndex = 0U;
static volatile bool Entry_FrameReady = false;


/* Exit USART2 reception. */

static volatile uint8_t Exit_RxBuffer[PARKING_PROTOCOL_FRAME_SIZE];
static volatile uint8_t Exit_RxIndex = 0U;
static volatile bool Exit_FrameReady = false;


/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void Communication_EntryRxCallback(void);
static void Communication_ExitRxCallback(void);

static void Communication_ReceiveByte(volatile USART_t *USARTx,
                                      volatile uint8_t *buffer,
                                      volatile uint8_t *index,
                                      volatile bool *frameReady);

static bool Communication_DecodeFrame(const uint8_t *frame,
                                      ParkingPacket_t *packet);

static void Communication_EncodeFrame(const ParkingPacket_t *packet,
                                      uint8_t *frame);

static uint8_t Communication_CalculateChecksum(const uint8_t *frame);

static void Communication_SendPacket(volatile USART_t *USARTx,
                                     const ParkingPacket_t *packet);

static MessageType_t Communication_MapAccessResult(AccessResult_t result);


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void Communication_Init(void)
{
    /* ================= ENTRY LINK : USART1 ================= */

    Entry_UART_Config.USART_Mode     = UART_Mode_TX_RX;
    Entry_UART_Config.BaudRate       = UART_BaudRate_115200;
    Entry_UART_Config.Payload_Length = UART_Payload_Length_8B;
    Entry_UART_Config.parity         = UART_Parity_NONE;
    Entry_UART_Config.StopBits       = UART_StopBits_1;
    Entry_UART_Config.HwFlowCtl      = UART_HwFlowCtl_NONE;
    Entry_UART_Config.IRQ_Enable     = UART_IRQ_Enable_RXNEIE;
    Entry_UART_Config.P_IRQ_CalBack  = Communication_EntryRxCallback;

    MCAL_UART_Init(USART1,
                   &Entry_UART_Config);

    MCAL_UART_GPIO_Set_Pin(USART1);


    /* ================= EXIT LINK : USART2 ================= */

    Exit_UART_Config.USART_Mode     = UART_Mode_TX_RX;
    Exit_UART_Config.BaudRate       = UART_BaudRate_115200;
    Exit_UART_Config.Payload_Length = UART_Payload_Length_8B;
    Exit_UART_Config.parity         = UART_Parity_NONE;
    Exit_UART_Config.StopBits       = UART_StopBits_1;
    Exit_UART_Config.HwFlowCtl      = UART_HwFlowCtl_NONE;
    Exit_UART_Config.IRQ_Enable     = UART_IRQ_Enable_RXNEIE;
    Exit_UART_Config.P_IRQ_CalBack  = Communication_ExitRxCallback;

    MCAL_UART_Init(USART2,
                   &Exit_UART_Config);

    MCAL_UART_GPIO_Set_Pin(USART2);
}


bool Communication_ProcessMessage(const ParkingPacket_t *request,
                                  ParkingPacket_t *response)
{
    AccessResult_t Local_Result;


    if ((request == NULL) ||
        (response == NULL))
    {
        return false;
    }


    response->source  = request->source;
    response->userID  = request->userID;
    response->rfidUID = request->rfidUID;


    switch (request->type)
    {
        case MSG_AUTH_ID_REQ:

            Local_Result =
                Access_ValidateID(request->userID);

            response->type =
                Communication_MapAccessResult(Local_Result);

            return true;


        case MSG_AUTH_RFID_REQ:

            Local_Result =
                Access_ValidateRFID(request->userID,
                                    request->rfidUID,
                                    request->source);

            response->type =
                Communication_MapAccessResult(Local_Result);

            return true;


        case MSG_ENTRY_COMPLETED:

            if (request->source != SOURCE_ENTRY)
            {
                return false;
            }

            if (Access_CompleteTransaction(request->userID,
                                           SOURCE_ENTRY) == false)
            {
                return false;
            }

            response->type = MSG_NONE;

            return true;


        case MSG_EXIT_COMPLETED:

            if (request->source != SOURCE_EXIT)
            {
                return false;
            }

            if (Access_CompleteTransaction(request->userID,
                                           SOURCE_EXIT) == false)
            {
                return false;
            }

            response->type = MSG_NONE;

            return true;


        default:

            return false;
    }
}


void Communication_Update(void)
{
    uint8_t Local_Frame[PARKING_PROTOCOL_FRAME_SIZE];

    ParkingPacket_t Local_Request;
    ParkingPacket_t Local_Response;

    uint8_t Local_Index;


    /* ======================= ENTRY ======================= */

    if (Entry_FrameReady == true)
    {
        for (Local_Index = 0U;
             Local_Index < PARKING_PROTOCOL_FRAME_SIZE;
             Local_Index++)
        {
            Local_Frame[Local_Index] =
                Entry_RxBuffer[Local_Index];
        }

        Entry_FrameReady = false;


        if (Communication_DecodeFrame(Local_Frame,
                                      &Local_Request) == true)
        {
            /*
             * USART1 physically belongs to the Entry ECU.
             * Do not trust the source byte received over UART.
             */
            Local_Request.source = SOURCE_ENTRY;


            if (Communication_ProcessMessage(&Local_Request,
                                             &Local_Response) == true)
            {
                if (Local_Response.type != MSG_NONE)
                {
                    Communication_SendPacket(USART1,
                                             &Local_Response);
                }
            }
        }
    }


    /* ======================== EXIT ======================= */

    if (Exit_FrameReady == true)
    {
        for (Local_Index = 0U;
             Local_Index < PARKING_PROTOCOL_FRAME_SIZE;
             Local_Index++)
        {
            Local_Frame[Local_Index] =
                Exit_RxBuffer[Local_Index];
        }

        Exit_FrameReady = false;


        if (Communication_DecodeFrame(Local_Frame,
                                      &Local_Request) == true)
        {
            /*
             * USART2 physically belongs to the Exit ECU.
             */
            Local_Request.source = SOURCE_EXIT;


            if (Communication_ProcessMessage(&Local_Request,
                                             &Local_Response) == true)
            {
                if (Local_Response.type != MSG_NONE)
                {
                    Communication_SendPacket(USART2,
                                             &Local_Response);
                }
            }
        }
    }
}


/******************************************************************************
 * UART Receive Callbacks
 ******************************************************************************/

static void Communication_EntryRxCallback(void)
{
    Communication_ReceiveByte(USART1,
                              Entry_RxBuffer,
                              &Entry_RxIndex,
                              &Entry_FrameReady);
}


static void Communication_ExitRxCallback(void)
{
    Communication_ReceiveByte(USART2,
                              Exit_RxBuffer,
                              &Exit_RxIndex,
                              &Exit_FrameReady);
}


/******************************************************************************
 * UART Reception
 ******************************************************************************/

static void Communication_ReceiveByte(volatile USART_t *USARTx,
                                      volatile uint8_t *buffer,
                                      volatile uint8_t *index,
                                      volatile bool *frameReady)
{
    uint16_t Local_Data;


    MCAL_UART_Receive_Data(USARTx,
                           &Local_Data,
                           disable);


    /*
     * Ignore new data while a complete frame is waiting
     * for the main application to process it.
     */
    if (*frameReady == true)
    {
        return;
    }


    /*
     * Wait for Start Of Frame.
     */
    if (*index == 0U)
    {
        if ((uint8_t)Local_Data != PARKING_PROTOCOL_SOF)
        {
            return;
        }
    }


    buffer[*index] = (uint8_t)Local_Data;

    (*index)++;


    if (*index >= PARKING_PROTOCOL_FRAME_SIZE)
    {
        *index = 0U;
        *frameReady = true;
    }
}


/******************************************************************************
 * Frame Decoder
 ******************************************************************************/

static bool Communication_DecodeFrame(const uint8_t *frame,
                                      ParkingPacket_t *packet)
{
    if ((frame == NULL) ||
        (packet == NULL))
    {
        return false;
    }


    if (frame[0] != PARKING_PROTOCOL_SOF)
    {
        return false;
    }


    if (frame[PARKING_PROTOCOL_CHECKSUM_INDEX] !=
        Communication_CalculateChecksum(frame))
    {
        return false;
    }


    packet->type =
        (MessageType_t)
        frame[PARKING_PROTOCOL_TYPE_INDEX];


    packet->source =
        (ECU_Source_t)
        frame[PARKING_PROTOCOL_SOURCE_INDEX];


    packet->userID =
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX] << 24) |
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX + 1U] << 16) |
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX + 2U] << 8) |
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX + 3U]);


    packet->rfidUID =
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX] << 24) |
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX + 1U] << 16) |
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX + 2U] << 8) |
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX + 3U]);


    return true;
}


/******************************************************************************
 * Frame Encoder
 ******************************************************************************/

static void Communication_EncodeFrame(const ParkingPacket_t *packet,
                                      uint8_t *frame)
{
    frame[0] = PARKING_PROTOCOL_SOF;

    frame[PARKING_PROTOCOL_TYPE_INDEX] =
        (uint8_t)packet->type;

    frame[PARKING_PROTOCOL_SOURCE_INDEX] =
        (uint8_t)packet->source;


    frame[PARKING_PROTOCOL_USER_ID_INDEX] =
        (uint8_t)(packet->userID >> 24);

    frame[PARKING_PROTOCOL_USER_ID_INDEX + 1U] =
        (uint8_t)(packet->userID >> 16);

    frame[PARKING_PROTOCOL_USER_ID_INDEX + 2U] =
        (uint8_t)(packet->userID >> 8);

    frame[PARKING_PROTOCOL_USER_ID_INDEX + 3U] =
        (uint8_t)(packet->userID);


    frame[PARKING_PROTOCOL_RFID_UID_INDEX] =
        (uint8_t)(packet->rfidUID >> 24);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 1U] =
        (uint8_t)(packet->rfidUID >> 16);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 2U] =
        (uint8_t)(packet->rfidUID >> 8);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 3U] =
        (uint8_t)(packet->rfidUID);


    frame[PARKING_PROTOCOL_CHECKSUM_INDEX] =
        Communication_CalculateChecksum(frame);
}


/******************************************************************************
 * Checksum
 ******************************************************************************/

static uint8_t Communication_CalculateChecksum(const uint8_t *frame)
{
    uint8_t Local_Checksum = 0U;
    uint8_t Local_Index;


    for (Local_Index = PARKING_PROTOCOL_TYPE_INDEX;
         Local_Index < PARKING_PROTOCOL_CHECKSUM_INDEX;
         Local_Index++)
    {
        Local_Checksum ^= frame[Local_Index];
    }


    return Local_Checksum;
}


/******************************************************************************
 * UART Transmission
 ******************************************************************************/

static void Communication_SendPacket(volatile USART_t *USARTx,
                                     const ParkingPacket_t *packet)
{
    uint8_t Local_Frame[PARKING_PROTOCOL_FRAME_SIZE];
    uint8_t Local_Index;
    uint16_t Local_Data;


    Communication_EncodeFrame(packet,
                              Local_Frame);


    for (Local_Index = 0U;
         Local_Index < PARKING_PROTOCOL_FRAME_SIZE;
         Local_Index++)
    {
        Local_Data = Local_Frame[Local_Index];

        MCAL_UART_Send_Data(USARTx,
                            &Local_Data,
                            enable);
    }


    MCAL_UART_WAIT_TC(USARTx);
}


/******************************************************************************
 * Access Result Mapping
 ******************************************************************************/

static MessageType_t Communication_MapAccessResult(AccessResult_t result)
{
    switch (result)
    {
        case ACCESS_ID_VALID:
            return MSG_ID_VALID;

        case ACCESS_INVALID_ID:
            return MSG_INVALID_ID;

        case ACCESS_WRONG_CARD:
            return MSG_WRONG_CARD;

        case ACCESS_GRANTED:
            return MSG_ACCESS_GRANTED;

        case ACCESS_DENIED:
            return MSG_ACCESS_DENIED;

        case ACCESS_PARKING_FULL:
            return MSG_PARKING_FULL;

        case ACCESS_ALREADY_INSIDE:
            return MSG_ALREADY_INSIDE;

        case ACCESS_INVALID_EXIT:
            return MSG_INVALID_EXIT;

        case ACCESS_RESULT_NONE:
        default:
            return MSG_NONE;
    }
}
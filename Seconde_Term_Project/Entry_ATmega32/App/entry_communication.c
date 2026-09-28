/******************************************************************************
 * @file        ecu_communication.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry / Exit ECU
 * @layer       Application Layer
 * @module      ECU Communication
 *
 * @brief
 * Implements UART packet communication between an ATmega32 gate ECU
 * and the STM32 Central ECU using the common Smart Parking protocol.
 ******************************************************************************/

#include "ecu_communication.h"
#include "atmega32_uart_driver.h"
	
#include <stddef.h>


/******************************************************************************
 * Private Variables
 ******************************************************************************/

static UART_Config_t UART_Config;

static volatile uint8_t RxBuffer[PARKING_PROTOCOL_FRAME_SIZE];

static volatile uint8_t RxIndex = 0U;

static volatile bool FrameReady = false;


/******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void ECU_Communication_RxCallback(void);

static void ECU_Communication_EncodeFrame(const ParkingPacket_t *packet,
                                          uint8_t *frame);

static bool ECU_Communication_DecodeFrame(const uint8_t *frame,
                                          ParkingPacket_t *packet);

static uint8_t ECU_Communication_CalculateChecksum(const uint8_t *frame);


/******************************************************************************
 * Public Functions
 ******************************************************************************/

void ECU_Communication_Init(void)
{
    RxIndex = 0U;
    FrameReady = false;

    UART_Config.BaudRate = UART_BAUDRATE_115200;

    UART_Config.P_IRQ_CallBack =
        ECU_Communication_RxCallback;

    MCAL_UART_Init(&UART_Config);
}


bool ECU_Communication_SendPacket(const ParkingPacket_t *packet)
{
    ParkingPacket_t Local_Packet;
    uint8_t Local_Frame[PARKING_PROTOCOL_FRAME_SIZE];
    uint8_t Local_Index;

    if (packet == NULL)
    {
        return false;
    }

    Local_Packet = *packet;

    /* Force the physical ECU identity. */
    Local_Packet.source = SOURCE_ENTRY;

    ECU_Communication_EncodeFrame(&Local_Packet,
                                  Local_Frame);

    for (Local_Index = 0U;
         Local_Index < PARKING_PROTOCOL_FRAME_SIZE;
         Local_Index++)
    {
        MCAL_UART_SendData(Local_Frame[Local_Index]);
    }

    return true;
}


bool ECU_Communication_GetPacket(ParkingPacket_t *packet)
{
    uint8_t Local_Frame[PARKING_PROTOCOL_FRAME_SIZE];

    uint8_t Local_Index;


    if (packet == NULL)
    {
        return false;
    }


    if (FrameReady == false)
    {
        return false;
    }


    /*
     * Copy the volatile RX buffer before decoding it.
     */
    for (Local_Index = 0U;
         Local_Index < PARKING_PROTOCOL_FRAME_SIZE;
         Local_Index++)
    {
        Local_Frame[Local_Index] =
            RxBuffer[Local_Index];
    }


    FrameReady = false;


    return ECU_Communication_DecodeFrame(Local_Frame,
                                         packet);
}


/******************************************************************************
 * UART RX Callback
 ******************************************************************************/

static void ECU_Communication_RxCallback(void)
{
    uint8_t Local_Data;


    /*
     * RX Complete interrupt already indicates that a byte
     * is available in the UART data register.
     */
    Local_Data = MCAL_UART_ReceiveData();


    /*
     * Do not overwrite a complete frame that has not yet
     * been processed by the application.
     */
    if (FrameReady == true)
    {
        return;
    }


    /*
     * Synchronize reception using Start Of Frame.
     */
    if (RxIndex == 0U)
    {
        if (Local_Data != PARKING_PROTOCOL_SOF)
        {
            return;
        }
    }


    RxBuffer[RxIndex] = Local_Data;

    RxIndex++;


    if (RxIndex >= PARKING_PROTOCOL_FRAME_SIZE)
    {
        RxIndex = 0U;

        FrameReady = true;
    }
}


/******************************************************************************
 * Frame Encoder
 ******************************************************************************/

static void ECU_Communication_EncodeFrame(const ParkingPacket_t *packet,
                                          uint8_t *frame)
{
    frame[0] = PARKING_PROTOCOL_SOF;


    frame[PARKING_PROTOCOL_TYPE_INDEX] =
        (uint8_t)packet->type;


    frame[PARKING_PROTOCOL_SOURCE_INDEX] =
        (uint8_t)packet->source;


    /* User ID - MSB first */

    frame[PARKING_PROTOCOL_USER_ID_INDEX] =
        (uint8_t)(packet->userID >> 24);

    frame[PARKING_PROTOCOL_USER_ID_INDEX + 1U] =
        (uint8_t)(packet->userID >> 16);

    frame[PARKING_PROTOCOL_USER_ID_INDEX + 2U] =
        (uint8_t)(packet->userID >> 8);

    frame[PARKING_PROTOCOL_USER_ID_INDEX + 3U] =
        (uint8_t)(packet->userID);


    /* RFID UID - MSB first */

    frame[PARKING_PROTOCOL_RFID_UID_INDEX] =
        (uint8_t)(packet->rfidUID >> 24);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 1U] =
        (uint8_t)(packet->rfidUID >> 16);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 2U] =
        (uint8_t)(packet->rfidUID >> 8);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 3U] =
        (uint8_t)(packet->rfidUID);


    frame[PARKING_PROTOCOL_CHECKSUM_INDEX] =
        ECU_Communication_CalculateChecksum(frame);
}


/******************************************************************************
 * Frame Decoder
 ******************************************************************************/

static bool ECU_Communication_DecodeFrame(const uint8_t *frame,
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
        ECU_Communication_CalculateChecksum(frame))
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
 * Checksum
 ******************************************************************************/

static uint8_t ECU_Communication_CalculateChecksum(const uint8_t *frame)
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
/******************************************************************************
 * @file        ecu_communication.c
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         ATmega32 Exit ECU
 * @mcu         ATmega32
 * @layer       Application Layer
 * @module      ECU Communication
 *
 * @path        Exit_ATmega32/App/ecu_communication.c
 *
 * @brief
 * Implements the fixed 12-byte UART protocol between the Exit ECU and STM32.
 ******************************************************************************/

#include "ecu_communication.h"
#include "atmega32_uart_driver.h"

#include <stdint.h>
#include <stdbool.h>

static UART_Config_t UART_Config;

static volatile uint8_t RxBuffer[PARKING_PROTOCOL_FRAME_SIZE];
static volatile uint8_t RxIndex = 0U;
static volatile bool FrameReady = false;

static void ECU_Communication_RXCallback(void);
static void ECU_Communication_EncodeFrame(const ParkingPacket_t *packet,
                                           uint8_t *frame);
static bool ECU_Communication_DecodeFrame(const uint8_t *frame,
                                           ParkingPacket_t *packet);
static uint8_t ECU_Communication_CalculateChecksum(const uint8_t *frame);

void ECU_Communication_Init(void)
{
    RxIndex = 0U;
    FrameReady = false;

    UART_Config.BaudRate = UART_BAUDRATE_115200;
    UART_Config.P_IRQ_CallBack = ECU_Communication_RXCallback;

    MCAL_UART_Init(&UART_Config);
}

bool ECU_Communication_SendPacket(const ParkingPacket_t *packet)
{
    uint8_t Local_Frame[PARKING_PROTOCOL_FRAME_SIZE];
    ParkingPacket_t Local_Packet;
    uint8_t Local_Index;

    if (packet == NULL)
    {
        return false;
    }

    Local_Packet = *packet;

    /*
     * This physical UART belongs to the Exit ECU.
     * Never trust a caller-supplied source value.
     */
    Local_Packet.source = SOURCE_EXIT;

    ECU_Communication_EncodeFrame(&Local_Packet, Local_Frame);

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

    for (Local_Index = 0U;
         Local_Index < PARKING_PROTOCOL_FRAME_SIZE;
         Local_Index++)
    {
        Local_Frame[Local_Index] = RxBuffer[Local_Index];
    }

    FrameReady = false;

    return ECU_Communication_DecodeFrame(Local_Frame, packet);
}

static void ECU_Communication_RXCallback(void)
{
    uint8_t Local_Data;

    Local_Data = MCAL_UART_ReceiveData();

    if (FrameReady == true)
    {
        return;
    }

    /*
     * Synchronize only on SOF when waiting for the beginning of a frame.
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

static void ECU_Communication_EncodeFrame(const ParkingPacket_t *packet,
                                           uint8_t *frame)
{
    frame[0] = PARKING_PROTOCOL_SOF;
    frame[PARKING_PROTOCOL_TYPE_INDEX] = (uint8_t)packet->type;
    frame[PARKING_PROTOCOL_SOURCE_INDEX] = (uint8_t)packet->source;

    frame[PARKING_PROTOCOL_USER_ID_INDEX] =
        (uint8_t)((packet->userID >> 24U) & 0xFFU);
    frame[PARKING_PROTOCOL_USER_ID_INDEX + 1U] =
        (uint8_t)((packet->userID >> 16U) & 0xFFU);
    frame[PARKING_PROTOCOL_USER_ID_INDEX + 2U] =
        (uint8_t)((packet->userID >> 8U) & 0xFFU);
    frame[PARKING_PROTOCOL_USER_ID_INDEX + 3U] =
        (uint8_t)(packet->userID & 0xFFU);

    frame[PARKING_PROTOCOL_RFID_UID_INDEX] =
        (uint8_t)((packet->rfidUID >> 24U) & 0xFFU);
    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 1U] =
        (uint8_t)((packet->rfidUID >> 16U) & 0xFFU);
    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 2U] =
        (uint8_t)((packet->rfidUID >> 8U) & 0xFFU);
    frame[PARKING_PROTOCOL_RFID_UID_INDEX + 3U] =
        (uint8_t)(packet->rfidUID & 0xFFU);

    frame[PARKING_PROTOCOL_CHECKSUM_INDEX] =
        ECU_Communication_CalculateChecksum(frame);
}

static bool ECU_Communication_DecodeFrame(const uint8_t *frame,
                                           ParkingPacket_t *packet)
{
    uint8_t Local_Checksum;

    if ((frame == NULL) || (packet == NULL))
    {
        return false;
    }

    if (frame[0] != PARKING_PROTOCOL_SOF)
    {
        return false;
    }

    Local_Checksum = ECU_Communication_CalculateChecksum(frame);

    if (Local_Checksum != frame[PARKING_PROTOCOL_CHECKSUM_INDEX])
    {
        return false;
    }

    packet->type =
        (MessageType_t)frame[PARKING_PROTOCOL_TYPE_INDEX];

    packet->source =
        (ECU_Source_t)frame[PARKING_PROTOCOL_SOURCE_INDEX];

    packet->userID =
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX] << 24U) |
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX + 1U] << 16U) |
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX + 2U] << 8U) |
        ((uint32_t)frame[PARKING_PROTOCOL_USER_ID_INDEX + 3U]);

    packet->rfidUID =
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX] << 24U) |
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX + 1U] << 16U) |
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX + 2U] << 8U) |
        ((uint32_t)frame[PARKING_PROTOCOL_RFID_UID_INDEX + 3U]);

    return true;
}

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

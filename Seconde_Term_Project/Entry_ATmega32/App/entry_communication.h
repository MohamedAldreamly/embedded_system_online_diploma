/******************************************************************************
 * @file        ecu_communication.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry 
 * @layer       Application Layer
 * @module      ECU Communication
 *
 * @brief
 * Communication interface between an ATmega32 gate ECU and the
 * STM32 Central ECU.
 ******************************************************************************/

#ifndef ECU_COMMUNICATION_H_
#define ECU_COMMUNICATION_H_

#include <stdint.h>
#include <stdbool.h>

#include "system_types.h"
#include "parking_protocol.h"


/******************************************************************************
 * Public APIs
 ******************************************************************************/

/**
 * @brief Initialize the ECU UART communication.
 */
void ECU_Communication_Init(void);


/**
 * @brief Send a logical parking packet to the STM32 Central ECU.
 *
 * @param[in] packet Packet to transmit.
 *
 * @return true if the packet was accepted for transmission.
 */
bool ECU_Communication_SendPacket(const ParkingPacket_t *packet);


/**
 * @brief Check whether a complete valid packet was received.
 *
 * @param[out] packet Decoded packet received from the STM32.
 *
 * @return true when a valid packet is available.
 * @return false otherwise.
 */
bool ECU_Communication_GetPacket(ParkingPacket_t *packet);


#endif /* ECU_COMMUNICATION_H_ */

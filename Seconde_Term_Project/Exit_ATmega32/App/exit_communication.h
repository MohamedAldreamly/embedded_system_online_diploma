/******************************************************************************
 * @file        exit_communication.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Exit ECU
 * @layer       Application Layer
 * @module      Exit Communication
 *
 * @brief
 * Communication interface between the ATmega32 Exit ECU and the
 * STM32 Central ECU.
 ******************************************************************************/

#ifndef EXIT_COMMUNICATION_H_
#define EXIT_COMMUNICATION_H_

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
void Exit_Communication_Init(void);


/**
 * @brief Send a logical parking packet to the STM32 Central ECU.
 *
 * @param[in] packet Packet to transmit.
 *
 * @return true if the packet was accepted for transmission.
 */
bool Exit_Communication_SendPacket(const ParkingPacket_t *packet);


/**
 * @brief Check whether a complete valid packet was received.
 *
 * @param[out] packet Decoded packet received from the STM32.
 *
 * @return true when a valid packet is available.
 * @return false otherwise.
 */
bool Exit_Communication_GetPacket(ParkingPacket_t *packet);


#endif /* EXIT_COMMUNICATION_H_ */

/******************************************************************************
 * @file        communication.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        29 September 2026
 *
 * @ecu         STM32 Central ECU
 * @mcu         STM32F103C6
 * @layer       Application Layer
 * @module      Communication Manager
 *
 * @path        STM32_Central/App/communication.h
 *
 * @brief
 * Central communication manager interface.
 *
 * This module manages communication between the STM32 Central ECU
 * and the Entry/Exit ATmega32 ECUs.
 ******************************************************************************/

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_


#include <stdint.h>
#include <stdbool.h>

#include "system_types.h"
#include "parking_protocol.h"


/******************************************************************************
 * Public APIs
 ******************************************************************************/

/**
 * @brief Initialize Entry and Exit communication links.
 */
void Communication_Init(void);


/**
 * @brief Process a received parking-system request.
 *
 * Supported requests:
 * - ID authentication.
 * - RFID authentication.
 * - Add new user.
 * - Entry completion.
 * - Exit completion.
 *
 * @param[in]  request  Received application packet.
 * @param[out] response Generated response packet.
 *
 * @return true if the request was processed successfully.
 * @return false otherwise.
 */
bool Communication_ProcessMessage(const ParkingPacket_t *request,
                                  ParkingPacket_t *response);


/**
 * @brief Process complete UART frames received from Entry and Exit ECUs.
 *
 * This function shall be called periodically from the main application loop.
 */
void Communication_Update(void);


#endif /* COMMUNICATION_H_ */
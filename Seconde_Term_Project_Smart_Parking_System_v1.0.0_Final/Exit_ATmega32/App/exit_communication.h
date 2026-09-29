/******************************************************************************
 * @file        ecu_communication.h
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
 * @path        Exit_ATmega32/App/ecu_communication.h
 *
 * @brief
 * Communication interface between the Exit ECU and STM32 Central ECU.
 ******************************************************************************/

#ifndef ECU_COMMUNICATION_H_
#define ECU_COMMUNICATION_H_

#include <stdint.h>
#include <stdbool.h>
#include "system_types.h"
#include "parking_protocol.h"

void ECU_Communication_Init(void);
bool ECU_Communication_SendPacket(const ParkingPacket_t *packet);
bool ECU_Communication_GetPacket(ParkingPacket_t *packet);

#endif /* ECU_COMMUNICATION_H_ */

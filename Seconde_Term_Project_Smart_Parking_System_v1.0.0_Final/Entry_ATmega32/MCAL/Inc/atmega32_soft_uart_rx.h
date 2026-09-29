/******************************************************************************
 * @file        atmega32_soft_uart_rx.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @ecu         ATmega32 Entry ECU
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      Software UART RX
 *
 * @path        Entry_ATmega32/MCAL/Inc/atmega32_soft_uart_rx.h
 *
 * @brief
 * Minimal software UART receiver used by the RFID interface.
 ******************************************************************************/

#ifndef ATMEGA32_SOFT_UART_RX_H_
#define ATMEGA32_SOFT_UART_RX_H_

#include <stdint.h>
#include <stdbool.h>


void MCAL_SoftUART_RX_Init(void);

bool MCAL_SoftUART_RX_ReadByte(uint8_t *data);


#endif /* ATMEGA32_SOFT_UART_RX_H_ */
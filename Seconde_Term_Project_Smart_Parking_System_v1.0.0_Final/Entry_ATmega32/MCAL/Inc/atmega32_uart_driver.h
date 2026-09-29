/******************************************************************************
 * @file        atmega32_uart_driver.h
 * @project     Smart Parking System
 * @version     1.0.0
 *
 * @author      Mohamed Aldreamly
 * @date        28 September 2026
 *
 * @mcu         ATmega32
 * @layer       MCAL
 * @module      UART
 *
 * @brief
 * UART driver interface for the ATmega32 Entry and Exit ECUs.
 * The project UART format is fixed to 8 data bits, no parity,
 * and one stop bit (8N1).
 ******************************************************************************/

#ifndef ATMEGA32_UART_DRIVER_H_
#define ATMEGA32_UART_DRIVER_H_

#include <stdint.h>

/******************************************************************************
 * UART Configuration
 ******************************************************************************/

typedef struct
{
    uint32_t BaudRate;

    void (*P_IRQ_CallBack)(void);

} UART_Config_t;


/******************************************************************************
 * Supported Baud Rates
 ******************************************************************************/

#define UART_BAUDRATE_9600       9600UL
#define UART_BAUDRATE_115200     115200UL


/******************************************************************************
 * Public APIs
 ******************************************************************************/

void MCAL_UART_Init(UART_Config_t *UART_Config);

void MCAL_UART_SendData(uint8_t data);

uint8_t MCAL_UART_ReceiveData(void);


#endif /* ATMEGA32_UART_DRIVER_H_ */
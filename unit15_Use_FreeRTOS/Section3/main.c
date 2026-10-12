/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Mohamed Aldreamly
 * @brief          : Main program body
 ******************************************************************************
 */


#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"

#include "Stm32_F103C6_GPIO_driver.h"
#include "Stm32_F103C6_USART_driver.h"

uint16_t volatile CMD = 0;
char pcString1[15];
char pcString2[15];

/*==========	Function prototype	==========*/
void Slave_CMDS_CallBack(void);

//Handles
TaskHandle_t xSlave_Handler = NULL;


/*	Task prototype	*/
void vSlave_Handler(void *par);

QueueHandle_t xQueueHandel = NULL;


int main(void)
{
	RCC_GPIOC_CLK_EN();
	RCC_GPIOA_CLK_EN();
	RCC_AFIO_CLK_EN();

	/*==========	USART2 Init	============*/
	//PA2 TX
	//PA3 RX
	//PA0 CTS
	//PA1 RTS

	UART_Config_t uartCFG;
	uartCFG.BaudRate = UART_BaudRate_2400;
	uartCFG.HwFlowCtl = UART_HwFlowCtl_NONE;
	uartCFG.IRQ_Enable = UART_IRQ_Enable_RXNEIE;
	uartCFG.P_IRQ_CalBack = Slave_CMDS_CallBack;
	uartCFG.Payload_Length = UART_Payload_Length_8B;
	uartCFG.StopBits = UART_StopBits_1;
	uartCFG.USART_Mode = UART_Mode_TX_RX;
	uartCFG.parity = UART_Parity_NONE;

	MCAL_UART_Init(USART2, &uartCFG);
	MCAL_UART_GPIO_Set_Pin(USART2);

	//=====================================
	//Pin 13 for LED
	GPIO_PinConfig_t PIN13CFG;
	PIN13CFG.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	PIN13CFG.GPIO_Output_Speed = GPIO_SPEED_10M;
	PIN13CFG.GPIO_PinNumber = GPIO_PIN_13;

	MCAL_GPIO_Init(GPIOC, &PIN13CFG);
	MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
	//======================================

	//init String
	strcpy(pcString1,"\nLED_ON");
	strcpy(pcString2,"\nLED_OFF");


	xTaskCreate(vSlave_Handler, "vSlave_Handler", 128, NULL, 0, &xSlave_Handler);

	xQueueHandel = xQueueCreate(10,sizeof(uint8_t));

	vTaskStartScheduler();

	while(1){

	}
	return 0;
}

void Slave_CMDS_CallBack(void){
	MCAL_UART_Receive_Data(USART2, &CMD, disable);
	xQueueSend(xQueueHandel,&CMD,(TickType_t)5);
	MCAL_UART_Send_Data(USART2, &CMD, enable);
}


void vSlave_Handler(void *par){

	char CommandRec = 0;
	while(1){
		xQueueReceive(xQueueHandel, &CommandRec, (TickType_t)5);
		if (CommandRec == 'T'){
			MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
			for(int i=0;i<15;i++){
				uint16_t ch = (uint8_t)pcString1[i];
				MCAL_UART_Send_Data(USART2, &ch, enable);
			}
			CommandRec = 0;
		}else if (CommandRec == 'F'){
			MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
			for(int i=0;i<15;i++){
				uint16_t ch = (uint8_t)pcString2[i];
				MCAL_UART_Send_Data(USART2, &ch, enable);
			}
			CommandRec = 0;
		}
		vTaskDelay(20);

	}
}




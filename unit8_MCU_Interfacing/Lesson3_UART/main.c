/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 20 2026
 *
 */

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_USART_driver.h"



void  clock_init(){
	//Enable clock GPIOA
	RCC_GPIOA_CLK_EN();
	//Enable clock GPIOB
	RCC_GPIOB_CLK_EN();
	//Enable clock AFIO
	RCC_AFIO_CLK_EN();
}


void wait_ms(uint32_t time){
	uint32_t i ,j;
	for (i=0;i<time;i++)
		for (j=0;j<255;j++);
}

uint16_t ch ;

void Mohamed_UART_Callback(void){
	MCAL_UART_Receive_Data(USART1, &ch,disable );
	MCAL_UART_Send_Data(USART1, &ch,disable);
}

int main(void)
{

	clock_init();

	UART_Config_t uartCFG;
	uartCFG.BaudRate = 115200;
	uartCFG.HwFlowCtl = UART_HwFlowCtl_NONE;
	uartCFG.IRQ_Enable = UART_IRQ_Enable_RXNEIE;
	//uartCFG.IRQ_Enable = UART_IRQ_Enable_NONE;
	//uartCFG.P_IRQ_CalBack = NULL;
	uartCFG.P_IRQ_CalBack = Mohamed_UART_Callback;
	uartCFG.Payload_Length = UART_Payload_Length_8B;
	uartCFG.StopBits = UART_StopBits_1;
	uartCFG.USART_Mode = UART_Mode_TX_RX;
	uartCFG.parity = UART_Parity_NONE;

    MCAL_UART_Init(USART1, &uartCFG);
    MCAL_UART_GPIO_Set_Pin(USART1);

f	while(1){
//		MCAL_UART_Receive_Data(USART1, &ch,enable );
//		MCAL_UART_Send_Data(USART1, &ch,enable);

    }
}

/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 22 2026
 *
 */

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_USART_driver.h"
#include "Stm32_F103C6_SPI_driver.h"

#define MCU_Act_As_Master
//#define MCU_Act_As_Slave

GPIO_PinConfig_t PinCfg_Main;

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


#ifdef MCU_Act_As_Master

	//Send to SPI
	MCAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
	MCAL_SPI_TX_RX(SPI1, &ch, Pollingenable);
	MCAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);



#endif

}

void Mohamed_SPI1_Callback(struct S_IRQ_SRC irq_src){

#ifdef MCU_Act_As_Slave

	//Send to SPI
		if(irq_src.RXNE){
			ch=0x0f;
			MCAL_SPI_TX_RX(SPI1, &ch, Pollingenable);
			MCAL_UART_Send_Data(USART1, &ch,enable);
		}


#endif

}

int main(void)
{

	clock_init();

	//==================	UART1 Init	=======================
	//PA9 TX
	//PA10 RX
	//PA11 CTS
	//PA12 RTS

	UART_Config_t uartCFG;
	uartCFG.BaudRate = UART_BaudRate_115200;
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

    //==================	SPI Init	=======================
    //PA4 : SPI1_NSS
	//PA5 : SPI1_SCK
	//PA6 : SPI1_MISO
	//PA7 : SPI1_MOSI

    SPI_Config_t spi1CFG;

    //Common Configuration
    spi1CFG.CLKPhase = SPI_CLKPhase_2EDGE_frist_capture_egde;
    spi1CFG.CLKPhase = SPI_CLKPolarity_HIGH_when_idle;
    spi1CFG.DataSize = SPI_DataSize_8BIT;
    spi1CFG.Frame_Format = SPI_Frame_Format_MSB_transmited_fisrt;
    //Assume by default pclk2 = 8MHZ
	spi1CFG.SPI_BAUDRATEPRESCALER = SPI_BAUDRATEPRESCALER_4;
	spi1CFG.Communication_Mode = SPI_DIRECTIONAL_2LINES;

#ifdef MCU_Act_As_Master
    spi1CFG.Device_Mode = SPI_Device_Mode_MASTER;
    spi1CFG.IRQ_Enable = SPI_IRQ_Enable_NONE;
	spi1CFG.NSS = SPI_NSS_Soft_NSSInternalSoft_Set;
	spi1CFG.P_IRQ_CallBack = NULL;

	//Configure SS On PA.4 By GPIO
		PinCfg_Main.GPIO_PinNumber = GPIO_PIN_4;
		PinCfg_Main.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
		PinCfg_Main.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &PinCfg_Main);

		//force the Slave Select (High) idle Mode
		MCAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 1);
#endif


#ifdef MCU_Act_As_Slave
    spi1CFG.Device_Mode = SPI_Device_Mode_SLAVE;
    spi1CFG.IRQ_Enable = SPI_IRQ_Enable_RXNEIE;
	spi1CFG.NSS = SPI_NSS_Hard_Slave;
	spi1CFG.P_IRQ_CallBack = Mohamed_SPI1_Callback;
#endif

	MCAL_SPI_Init(SPI1, &spi1CFG);
	MCAL_SPI_Set_Pin(SPI1);




	while(1){
//		MCAL_UART_Receive_Data(USART1, &ch,enable );
//		MCAL_UART_Send_Data(USART1, &ch,enable);

    }
}

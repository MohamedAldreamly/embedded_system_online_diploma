/*
 * Stm32_F103C6_USART_driver.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 17 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

//include
#include "Stm32_F103C6_USART_driver.h"

/*
 *==========================================================
 *                  	Generic Macros
 *==========================================================
 */

/*
 *===========================================================
 *					 	Generic Variable
 *===========================================================
 */

UART_Config_t* Global_UART_Config = NULL;


/*
 *===========================================================
 *					 	Generic Functions
 *===========================================================
 */


/*
 *===========================================================
 *					 	APIs Functions Definitions
 *===========================================================
 */



/**================================================================
 * @Fn				-MCAL_UART_Init
 * @brief		 	-Initializes UART (Supported feature ASYNCH. ONLY)
 * @param[in] 		-USARTx: where x can be (1..3 depending on device used)
 * @param[out] 		-UART_Config: All UART Configuration UART_Config_t
 * @retval			-none
 * Note				-Support for Now Asynch mode & Clock 8MHZ
 */

void MCAL_UART_Init(volatile USART_t* USARTx , UART_Config_t * UART_Config){

	uint32_t pclk, BRR;
/*
 *	 Procedure:
 *	1. Enable the USART by writing the UE bit in USART_CR1 register to 1.
 *	2. Program the M bit in USART_CR1 to define the word length.
 *	3. Program the number of stop bits in USART_CR2.
 * 	4. Select DMA enable (DMAR) in USART_CR3 if multibuffer communication is to take
 *	place. Configure the DMA register as explained in multibuffer communication. STEP 3
 * 	5. Select the desired baud rate using the baud rate register USART_BRR
 * 	6. Set the RE bit USART_CR1. This enables the receiver which begins searching for a
 * 	start bit.
 *
 */
	Global_UART_Config = UART_Config;
	//enable clock for given USART peripheral
	if (USARTx == USART1)
		RCC_USART1_CLK_EN();
	else if (USARTx == USART2)
			RCC_USART2_CLK_EN();
	else if (USARTx == USART3)
			RCC_USART3_CLK_EN();
	//1. Bit 13 UE: USART enable
	USARTx->CR1 |=(1<<13);
	//Enable USART TX and RX engines according to the USART_Mode configuration item

	//	USART_CR1 Bit 3 TE: Transmitter enable & Bit 2 RE: Receiver enable
	USARTx->CR1 |=UART_Config->USART_Mode;

	//2- USART_CR1 Bit 12 M: Word length
	USARTx->CR1 |=UART_Config->Payload_Length;

	//configuration of parity control bit field
	//USART_CR1 Bit 9 PS: Parity selection & Bit 10 PCE: Parity control enable
	USARTx->CR1 |=UART_Config->parity;

	//3- USART_CR2 Bits 13:12 STOP: STOP bits
	USARTx->CR2 |=UART_Config->StopBits;

	//Hardware flow control
	//USARTx->CR3 Bit 9 CTSE: CTS enable & Bit 8 RTSE: RTS enable
	USARTx->CR3 |=UART_Config->HwFlowCtl;

	//5- configuration of BRR
	//PCLK1 for USART2,3
	//PCLK2 for USART1

	if (USARTx == USART1){
		pclk = MCAL_RCC_GetPCLK2Freq();
	}else{
		pclk = MCAL_RCC_GetPCLK1Freq();
	}

	BRR =UART_BRR_Register(pclk,UART_Config->BaudRate);
	USARTx->BRR = BRR;

	//	Enable/Disable interrupt
	//USART_CR1
	if (UART_Config->IRQ_Enable != UART_IRQ_Enable_NONE){
		USARTx->CR1 |= (UART_Config->IRQ_Enable);
		//	Enable NVIC for USARTx IRQ
		if (USARTx == USART1)
			NVIC_IRQ37_USART1_Enable;
		else if (USARTx == USART2)
			NVIC_IRQ38_USART2_Enable;
		else if (USARTx == USART3)
			NVIC_IRQ39_USART3_Enable;
	}
}

/**================================================================
 * @Fn				-MCAL_UART_Deinit
 * @brief		 	-Deinitializes UART (Supported feature ASYNCH. ONLY)
 * @param[in] 		-USARTx: where x can be (1..3 depending on device used)
 * @retval			-none
 * Note				-Reset the Model By RCC
 */

void MCAL_UART_Deinit(volatile USART_t* USARTx){
	if (USARTx == USART1){
		RCC_USART1_CLK_Reset();
		NVIC_IRQ37_USART1_Disable;
	}else if (USARTx == USART2){
		 RCC_USART2_CLK_Reset();
		NVIC_IRQ38_USART2_Disable;
	}else if (USARTx == USART3){
		RCC_USART3_CLK_Reset();
		NVIC_IRQ39_USART3_Disable;
	}
}

/**================================================================
 * @Fn				-MCAL_UART_Send_Data
 * @brief		 	-Send Buffer on UART
 *
 * @param[in] 		-USARTx: where x can be (1..3 depending on device used)
 * @param[in] 		-pTxBuffer Buffer
 * @param[in] 		-PollingEn Enable pooling or dsable it
 *
 * @return			-none
 * Note				-Should initalize UART First
 *
 * 				// When transmitting with the parity enabled (PCE bit set to 1 in the USART_CR1 register),
 *				//the value written in the MSB (bit 7 or bit 8 depending on the data length) has no effect
 *				//because it is replaced by the parity.
 *				//When receiving with the parity enabled, the value read in the MSB bit is the received parity bit.
 *
 */

void MCAL_UART_Send_Data(volatile USART_t* USARTx ,uint16_t* pTxBuffer,PollingEn_t PollingEn){

	//wait until TXE falg is set in the SR
	if(PollingEn == enable)
		while (!(USARTx->SR & 1<<7));
	//Check the USART_WordLength item for 9BIT in a frame
	if(Global_UART_Config->Payload_Length == UART_Payload_Length_9B){
		USARTx->DR = (*pTxBuffer &(uint16_t)0x01FF);
	}else {
		//this is 8bit data transfer
		USARTx->DR = (*pTxBuffer &(uint16_t)0x01FF);
	}

}

void MCAL_UART_Receive_Data(volatile USART_t* USARTx ,uint16_t* pRxBuffer,PollingEn_t PollingEn){

	//wait until RXE falg is set in the SR
	if(PollingEn == enable)
		while(!(USARTx->SR & 1<<5));
	//Check the USART_WordLength item for 9BIT in a frame
	if(Global_UART_Config->Payload_Length == UART_Payload_Length_9B){
		if (Global_UART_Config->parity == UART_Parity_NONE ){
			//no parity so all 9bit are considered data
			*((uint16_t*)pRxBuffer ) = USARTx->DR &(uint16_t)0x1FF;
		}else{
			//parity is used so, 8bit will be used data and 1 bit to parity
			*((uint16_t*)pRxBuffer ) = USARTx->DR &(uint8_t)0xFF;
		}
	}
	//This is 8BIT in a frame
	else {
		if(Global_UART_Config->parity == UART_Parity_NONE){
			//no parity so all 8bit are considered data
			*((uint16_t*)pRxBuffer ) = USARTx->DR &(uint8_t)0xFF;
		}else {
			//parity is used so, 7bit will be used data and 1 bit to parity
			*((uint16_t*)pRxBuffer ) = USARTx->DR &(uint8_t)0x7F;
		}
	}
}

/**================================================================
 * @Fn				-MCAL_UART_GPIO_Set_Pin
 * @brief		 	-Initializes GPIO PINs
 * @param[in] 		-USARTx: where x can be (1..3 depending on device used)
 * @retval			-none
 * Note				-Should enable corrrsponding ALT & GPIO in RCC clock also called after MCAL_UART_Init
 */

void MCAL_UART_GPIO_Set_Pin(volatile USART_t* USARTx){

	GPIO_PinConfig_t PinCfg;

	if (USARTx == USART1){
		//PA9 TX
		//PA10 RX
		//PA11 CTS
		//PA12 RTS

		//PA9 TX
		PinCfg.GPIO_PinNumber = GPIO_PIN_9;
		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
		PinCfg.GPIO_Output_Speed =GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &PinCfg);

		//PA10 RX
		PinCfg.GPIO_PinNumber = GPIO_PIN_10;
		PinCfg.GPIO_MODE = GPIO_MODE_AF_INPUT;
		MCAL_GPIO_Init(GPIOA, &PinCfg);
	}

	if (Global_UART_Config->HwFlowCtl ==UART_HwFlowCtl_CTS || Global_UART_Config-> HwFlowCtl == UART_HwFlowCtl_RTS_CTS ){
		//PA11 CTS
		PinCfg.GPIO_PinNumber = GPIO_PIN_11;
		PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
		MCAL_GPIO_Init(GPIOA, &PinCfg);
		}
	if (Global_UART_Config->HwFlowCtl ==UART_HwFlowCtl_CTS || Global_UART_Config-> HwFlowCtl == UART_HwFlowCtl_RTS_CTS ){
		//PA12 RTS
		PinCfg.GPIO_PinNumber = GPIO_PIN_12;
		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
		PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &PinCfg);
	}

	if (USARTx == USART2){
		//PA2 TX
		//PA3 RX
		//PA0 CTS
		//PA1 RTS

		//PA2 TX
		PinCfg.GPIO_PinNumber = GPIO_PIN_2;
		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
		PinCfg.GPIO_Output_Speed =GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &PinCfg);

		//PA3 RX
		PinCfg.GPIO_PinNumber = GPIO_PIN_3;
		PinCfg.GPIO_MODE = GPIO_MODE_AF_INPUT;
		MCAL_GPIO_Init(GPIOA, &PinCfg);
	}

	if (Global_UART_Config->HwFlowCtl ==UART_HwFlowCtl_CTS || Global_UART_Config-> HwFlowCtl == UART_HwFlowCtl_RTS_CTS ){
		//PA0 CTS
		PinCfg.GPIO_PinNumber = GPIO_PIN_0;
		PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
		MCAL_GPIO_Init(GPIOA, &PinCfg);
		}
	if (Global_UART_Config->HwFlowCtl ==UART_HwFlowCtl_CTS || Global_UART_Config-> HwFlowCtl == UART_HwFlowCtl_RTS_CTS){
		//PA1 RTS
		PinCfg.GPIO_PinNumber = GPIO_PIN_1;
		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
		PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
		MCAL_GPIO_Init(GPIOA, &PinCfg);
	}


	if (USARTx == USART3){
			//PB10 TX
			//PB11 RX
			//PB13 CTS
			//PB14 RTS

			//PB10 TX
			PinCfg.GPIO_PinNumber = GPIO_PIN_10;
			PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			PinCfg.GPIO_Output_Speed =GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOB, &PinCfg);

			//PB11 RX
			PinCfg.GPIO_PinNumber = GPIO_PIN_11;
			PinCfg.GPIO_MODE = GPIO_MODE_AF_INPUT;
			MCAL_GPIO_Init(GPIOB, &PinCfg);
		}

		if (Global_UART_Config->HwFlowCtl ==UART_HwFlowCtl_CTS || Global_UART_Config-> HwFlowCtl == UART_HwFlowCtl_RTS_CTS){
			//PB13 CTS
			PinCfg.GPIO_PinNumber = GPIO_PIN_13;
			PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOB, &PinCfg);
			}
		if (Global_UART_Config->HwFlowCtl ==UART_HwFlowCtl_CTS || Global_UART_Config-> HwFlowCtl == UART_HwFlowCtl_RTS_CTS){
			//PB14 RTS
			PinCfg.GPIO_PinNumber = GPIO_PIN_14;
			PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOB, &PinCfg);
		}

}
void MCAL_UART_WAIT_TC(volatile USART_t* USARTx){
	//Bit 6 TC: Transmission complete
	while (!(USARTx->SR&1<<6));
}


/*
 *==========================================================
 *                  	ISR Functions
 *==========================================================
 */

void USART1_IRQHandler(void)
{
	Global_UART_Config->P_IRQ_CalBack();
}

void USART2_IRQHandler(void)
{
	Global_UART_Config->P_IRQ_CalBack();
}

void USART3_IRQHandler(void)
{
	Global_UART_Config->P_IRQ_CalBack();
}

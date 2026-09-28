/*
 * Stm32_F103C6_SPI_driver.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 22 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

//include
#include "Stm32_F103C6_SPI_driver.h"

/*
 *==========================================================
 *                  	Generic Macros
 *==========================================================
 */

#define SPI1_INDEX	0
#define SPI2_INDEX	1

#define SPI_SR_TXE		((uint8_t)0x02)// transmit buffer is empty
#define SPI_SR_RXNE		((uint8_t)0x01)// receive buffer is not empty


/*
 *===========================================================
 *					 	Generic Variable
 *===========================================================
 */

SPI_Config_t* Global_SPI_Config[2] = {NULL,NULL};

SPI_Config_t Global_SPI_Config1;
SPI_Config_t Global_SPI_Config2;
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
 * @Fn				-MCAL_SPI_Init
 * @brief		 	-Initializes SPI
 * @param[in] 		-SPIx: where x can be (1..2 depending on device used)
 * @param[out] 		-SPI_Config: All SPI Configuration SPI_Config_t
 * @retval			-none
 * Note				-Support for SPI Full Duplex Master/Slave only & NSS HW/Sw
 *						-in case of master you have to configure pin and drive it
 */

void MCAL_SPI_Init(volatile SPI_t *SPIx ,SPI_Config_t *SPI_Config){

	//Safty for Reg
	uint16_t tmpreg_CR1 = 0;
	uint16_t tmpreg_CR2 = 0;


	if(SPIx == SPI1){
		Global_SPI_Config1 = *SPI_Config;
		Global_SPI_Config[SPI1_INDEX] = &Global_SPI_Config1;
		RCC_SPI1_CLK_EN();

	}else if (SPIx == SPI2){
		Global_SPI_Config2 = *SPI_Config;
		Global_SPI_Config[SPI2_INDEX] = &Global_SPI_Config2;
		RCC_SPI2_CLK_EN();
	}

	//Bit 6 SPE: SPI enable
	tmpreg_CR1 |=(0x1U<<6);

	//Master Or Slave
	tmpreg_CR1 |=SPI_Config->Device_Mode;

	//SPI_Communication_Mode
	tmpreg_CR1 |=SPI_Config->Communication_Mode;

	//SPI_Frame_Format
	tmpreg_CR1 |=SPI_Config->Frame_Format;

	//SPI_DataSize
	tmpreg_CR1 |=SPI_Config->DataSize;

	//SPI_CLKPhase
	tmpreg_CR1 |=SPI_Config->CLKPhase;

	//SPI_CLKPolarity
	tmpreg_CR1 |=SPI_Config->CLKPolarity;

	//=====================	NSS	===========================
	if(SPI_Config->NSS==SPI_NSS_Hard_Master_SS_Output_enable){
		tmpreg_CR2 |=SPI_Config->NSS;

	}else if (SPI_Config->NSS==SPI_NSS_Hard_Master_SS_Output_disable){
		tmpreg_CR2 |=SPI_Config->NSS;
	}else{
		tmpreg_CR1 |=SPI_Config->NSS;
	}

	//SPI_BAUDRATEPRESCALER
	tmpreg_CR1 |=SPI_Config->SPI_BAUDRATEPRESCALER;

	if (SPI_Config->IRQ_Enable != SPI_IRQ_Enable_NONE){

		//SPI_IRQ_Enable
		tmpreg_CR2 |=SPI_Config->IRQ_Enable;

		if (SPIx == SPI1){
			NVIC_IRQ35_SPI1_Enable;
		}else if (SPIx == SPI2){
			NVIC_IRQ36_SPI2_Enable;
		}

	}

	//set config all in one time
	SPIx->CR1 |= tmpreg_CR1;
	SPIx->CR2 |= tmpreg_CR2;

}
void MCAL_SPI_Deinit(volatile SPI_t *SPIx ){

	if(SPIx == SPI1){
			RCC_SPI1_CLK_Reset();
			NVIC_IRQ35_SPI1_Disable;

		}else if (SPIx == SPI2){
			RCC_SPI2_CLK_Reset();
			NVIC_IRQ36_SPI2_Disable;
		}
}

void MCAL_SPI_SendData(volatile SPI_t *SPIx ,uint16_t *pTxBuffer, enum PollingMechism PollingEn){

	//todo receive only
	if (PollingEn == Pollingenable)
		while (!((SPIx)->SR & SPI_SR_TXE));

	SPIx->DR = *pTxBuffer;
}

void MCAL_SPI_ReceiveData(volatile SPI_t *SPIx ,uint16_t *pRxBuffer, enum PollingMechism PollingEn){

	//todo receive only
	if (PollingEn == Pollingenable)
		while (!((SPIx)->SR & SPI_SR_RXNE));

	 *pRxBuffer = SPIx->DR ;

}



void MCAL_SPI_TX_RX(volatile SPI_t *SPIx ,uint16_t *pTxBuffer, enum PollingMechism PollingEn){
	if (PollingEn == Pollingenable)
			while (!((SPIx)->SR & SPI_SR_TXE));
		SPIx->DR = *pTxBuffer;

	if (PollingEn == Pollingenable)
			while (!((SPIx)->SR & SPI_SR_RXNE));
		 *pTxBuffer = SPIx->DR ;

}

void MCAL_SPI_Set_Pin(volatile SPI_t *SPIx ){


	GPIO_PinConfig_t PinCfg;
	if(SPIx==SPI1){
		//PA4 : SPI1_NSS
		//PA5 : SPI1_SCK
		//PA6 : SPI1_MISO
		//PA7 : SPI1_MOSI

		//PA4 : SPI1_NSS

		if (Global_SPI_Config[SPI1_INDEX]->Device_Mode == SPI_Device_Mode_MASTER){
			switch(Global_SPI_Config[SPI1_INDEX]->NSS){
				case SPI_NSS_Hard_Master_SS_Output_enable:
					//Haedware master/slave Input floating
					PinCfg.GPIO_PinNumber = GPIO_PIN_4;
					PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
					MCAL_GPIO_Init(GPIOA, &PinCfg);
					break;


				case SPI_NSS_Hard_Master_SS_Output_disable:
					//Haedware master/NSS Output AL Push-Pull
					PinCfg.GPIO_PinNumber = GPIO_PIN_4;
					PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
					PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
					MCAL_GPIO_Init(GPIOA, &PinCfg);
					break;
			}

			//PA5 : SPI1_SCK
			// master AL Push-Pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_5;
			PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOA, &PinCfg);

			//PA6 : SPI1_MISO (support only full duplex)
			// full duplex / master input FLO
			PinCfg.GPIO_PinNumber = GPIO_PIN_6;
			PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &PinCfg);
			//todo to support half duplex and simplex

			//PA7 : SPI1_MOSI
			// full duplex / master AL Push-Pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_7;
			PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOA, &PinCfg);

		}else	//Slave
		{
			//PA4 : SPI1_NSS
			if (Global_SPI_Config[SPI2_INDEX]->Device_Mode == SPI_Device_Mode_MASTER){

			//Haedware master/slave Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_4;
			PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &PinCfg);

			}
			//PA5 : SPI1_SCK
			// Slave input FLO
			PinCfg.GPIO_PinNumber = GPIO_PIN_5;
			PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &PinCfg);

			//PA6 : SPI1_MISO (support only full duplex)
				// full duplex / Slave (point to point) AL Push-Pull
			//todo		full duplex / Slave (multi-slave) AL Push-Pull
			PinCfg.GPIO_PinNumber = GPIO_PIN_6;
			PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
			PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
			MCAL_GPIO_Init(GPIOA, &PinCfg);
			//todo to support half duplex and simplex

			//PA7 : SPI1_MOSI
			// full duplex / slave Input floating
			PinCfg.GPIO_PinNumber = GPIO_PIN_7;
			PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
			MCAL_GPIO_Init(GPIOA, &PinCfg);

		}


	}

		if(SPIx==SPI2){
			//PB12 : SPI2_NSS
			//PB13 : SPI2_SCK
			//PB14 : SPI2_MISO
			//PB15 : SPI2_MOSI

			//PB12 : SPI2_NSS

			if (Global_SPI_Config[SPI2_INDEX]->Device_Mode == SPI_Device_Mode_MASTER){
				switch(Global_SPI_Config[SPI1_INDEX]->NSS){
					case SPI_NSS_Hard_Master_SS_Output_enable:
						//Haedware master/slave Input floating
						PinCfg.GPIO_PinNumber = GPIO_PIN_12;
						PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
						MCAL_GPIO_Init(GPIOB, &PinCfg);
						break;


					case SPI_NSS_Hard_Master_SS_Output_disable:
						//Haedware master/NSS Output AL Push-Pull
						PinCfg.GPIO_PinNumber = GPIO_PIN_12;
						PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
						PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
						MCAL_GPIO_Init(GPIOB, &PinCfg);
						break;
				}

				//PB13 : SPI2_SCK
				// master AL Push-Pull
				PinCfg.GPIO_PinNumber = GPIO_PIN_13;
				PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
				PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
				MCAL_GPIO_Init(GPIOB, &PinCfg);

				//PA6 : SPI1_MISO (support only full duplex)
				// full duplex / master input FLO
				PinCfg.GPIO_PinNumber = GPIO_PIN_1;
				PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
				MCAL_GPIO_Init(GPIOB, &PinCfg);
				//todo to support half duplex and simplex

				//PA7 : SPI1_MOSI
				// full duplex / master AL Push-Pull
				PinCfg.GPIO_PinNumber = GPIO_PIN_7;
				PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
				PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
				MCAL_GPIO_Init(GPIOA, &PinCfg);

			}else	//Slave
			{
				//PB13 : SPI2_SCK
				if (Global_SPI_Config[SPI2_INDEX]->Device_Mode == SPI_Device_Mode_MASTER){

				//Haedware master/slave Input floating
				PinCfg.GPIO_PinNumber = GPIO_PIN_4;
				PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
				MCAL_GPIO_Init(GPIOA, &PinCfg);

				}
				//PB13 : SPI2_SCK
				// Slave input FLO
				PinCfg.GPIO_PinNumber = GPIO_PIN_5;
				PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
				MCAL_GPIO_Init(GPIOA, &PinCfg);

				//PB14 : SPI2_MISO (support only full duplex)
					// full duplex / Slave (point to point) AL Push-Pull
				//todo		full duplex / Slave (multi-slave) AL Push-Pull
				PinCfg.GPIO_PinNumber = GPIO_PIN_14;
				PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_PP;
				PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
				MCAL_GPIO_Init(GPIOB, &PinCfg);
				//todo to support half duplex and simplex

				//PB15 : SPI2_MOSI
				// full duplex / slave Input floating
				PinCfg.GPIO_PinNumber = GPIO_PIN_15;
				PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
				MCAL_GPIO_Init(GPIOB, &PinCfg);

			}


		}
}

/*
 *==========================================================
 *                  	ISR Functions
 *==========================================================
 */

void SPI1_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src;
	irq_src.TXE = ((SPI1->SR &(1<<1))>>1);
	irq_src.RXNE = ((SPI1->SR &(1<<1))>>0);
	irq_src.ERRI = ((SPI1->SR &(1<<1))>>4);


	Global_SPI_Config[SPI1_INDEX]->P_IRQ_CallBack(irq_src);
}

void SPI(void)
{
	struct S_IRQ_SRC irq_src;
	irq_src.TXE = ((SPI1->SR &(1<<1))>>1);
	irq_src.RXNE = ((SPI1->SR &(1<<1))>>0);
	irq_src.ERRI = ((SPI1->SR &(1<<1))>>4);

	Global_SPI_Config[SPI2_INDEX]->P_IRQ_CallBack(irq_src);
}



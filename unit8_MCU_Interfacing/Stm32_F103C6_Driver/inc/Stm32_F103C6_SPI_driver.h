/*
 *	Stm32F103C6_SPI_driver.h
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 20 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

#ifndef INC_STM32_F103C6_SPI_DRIVER_H_
#define INC_STM32_F103C6_SPI_DRIVER_H_

//include
#include "stm32f103x6.h"
#include "Stm32_F103C6_RCC_driver.h"
#include "Stm32_F103C6_gpio_driver.h"

struct S_IRQ_SRC {

	uint8_t TXE:1;//Tx buffer empty interrupt

	uint8_t RXNE:1;//RX buffer not empty interrupt
	uint8_t ERRI:1;//Error interrupt

	uint8_t Reserved:5;
};



//Configuration structure

typedef struct{

	uint16_t Device_Mode; // Specifies the SPI operating mode @ref SPI_Device_Mode

	uint16_t Communication_Mode; // Specifies the SPI bidirectional mode state @ref SPI_Communication_Mode

	uint16_t Frame_Format; // Specifies LSB or MSB @ref SPI_Frame_Format

	uint16_t DataSize;	// @ref SPI_DataSize

	uint16_t CLKPolarity; // @ref SPI_CLKPolarity

	uint16_t CLKPhase; //@ref SPI CLKPhase

	uint16_t NSS; 	//Specifies whether the NSS signal is managed by
					//@ref SPI_NSS
					//hardware (NSS pin) or by software using the SSI bitenable

	uint16_t SPI_BAUDRATEPRESCALER;	/*Specifies the Baud Rate prescaler value which will be
									used to configure the transmit and receive SCK clock.
									This parameter can be a value of @ref SPI BAUDRATEPRESCALER @note The communication clock is derived from the master
									clock. The slave clock does not need to be set.*/
									// Take Care you have to Configure RCC to enter the correct clock to APB2 >>> to SPI1

	uint16_t IRQ_Enable;//@ref SPI_IRQ_Enable

	void (* P_IRQ_CallBack) (struct S_IRQ_SRC irq_src);	//Set the C Function() which will be called once the IRQ Happen

}SPI_Config_t;


//==================================
//Macros Configuration References
//==================================

//@ref SPI_Device_Mode

#define SPI_Device_Mode_SLAVE		(0x00000000U)
#define SPI_Device_Mode_MASTER		(0x1U<<2) //CR1.Bit 2 MSTR: Master selection

//@ref SPI_Communication_Mode
#define SPI_DIRECTIONAL_2LINES			(0x00000000U)
#define SPI_DIRECTIONAL_2LINES_RX_ONLY	(0x1U<<10	)//CR1. Bit 10 RXONLY: Receive only
#define SPI_DIRECTIONAL_1LINES_RX_ONLY	(0x1U<<15	)// CR1.Bit 15 BIDIMODE: Bidirectional data mode enable
#define SPI_DIRECTIONAL_1LINES_TX_ONLY	((0x1U<<15)|(0x1U<<14))// CR1.Bit 15 BIDIMODE: Bidirectional data mode enable & Bit 14 BIDIOE: Output enable in bidirectional mode

//@ref SPI_Frame_Format
#define SPI_Frame_Format_MSB_transmited_fisrt	(0x00000000U)
#define SPI_Frame_Format_LSB_transmited_fisrt	(0x1U<<7	)	//CR1.Bit 7 LSBFIRST: Frame format

//@ref SPI_DataSize
#define SPI_DataSize_8BIT					(0x00000000U)//CR1.Bit 11 DFF: Data frame format
#define SPI_DataSize_16BIT					(0x1U<<11	)//CR1.Bit 11 DFF: Data frame format

//@ref SPI_CLKPolarity
#define SPI_CLKPolarity_LOW_when_idle		(0x00000000U)
#define SPI_CLKPolarity_HIGH_when_idle		(0x1U<<1	)//CR1.Bit1 CPOL: Clock polarity

// @ref SPI CLKPhase
#define SPI_CLKPhase_1EDGE_frist_capture_egde		(0x00000000U)
#define SPI_CLKPhase_2EDGE_frist_capture_egde		(0x1U<<1	)//CR1.Bit 0 CPHA: Clock phase

//@ref SPI_NSS SPI Slave Seclect Management
//Hardware
#define SPI_NSS_Hard_Slave 								(0x00000000U)//CR1.Bit 9 SSM: Software slave management
#define SPI_NSS_Hard_Master_SS_Output_enable 			(0x1U<<2	)//CR2.Bit 2 SSOE: SS output enable
#define SPI_NSS_Hard_Master_SS_Output_disable 			(0x00000000U)//CR2.Bit 2 SSOE: SS output disable

//Software
#define SPI_NSS_Soft_NSSInternalSoft_Reset 				(0x00000000U)//CR1.Bit 9 SSM: Software slave management
																	//Bit 8 SSI by defult 0
#define SPI_NSS_Soft_NSSInternalSoft_Set 				((0x1U<<9)|(0x1U<<8))//CR1.Bit 9 SSM: Software slave management & CR1.Bit 8 SSI: Internal slave select
																			//Set to Bit 8 SSI to make the output of NSS Pin is 1

//@ref SPI BAUDRATEPRESCALER 		//CR1.Bits 5:3 BR[2:0]: Baud rate control
#define SPI_BAUDRATEPRESCALER_2			(0x00000000U)
#define SPI_BAUDRATEPRESCALER_4			(0b001U << 3)
#define SPI_BAUDRATEPRESCALER_8			(0b010U << 3)
#define SPI_BAUDRATEPRESCALER_16		(0b011U << 3)
#define SPI_BAUDRATEPRESCALER_32		(0b100U << 3)
#define SPI_BAUDRATEPRESCALER_64		(0b101U << 3)
#define SPI_BAUDRATEPRESCALER_128		(0b110U << 3)
#define SPI_BAUDRATEPRESCALER_256		(0b111U << 3)

//@ref SPI_IRQ_Enable
#define SPI_IRQ_Enable_NONE				(uint32_t)(0)
#define SPI_IRQ_Enable_TXEIE			(0x1U << 7	)//CR2.Bit 7 TXEIE: Tx buffer empty interrupt enable
#define SPI_IRQ_Enable_ERRIE			(0x1U << 5	)//CR2.Bit 5 ERRIE: Error interrupt enable
#define SPI_IRQ_Enable_RXNEIE			(0x1U << 6	)//CR2.Bit 6 RXNEIE: RX buffer not empty interrupt enable

enum PollingMechism{
	Pollingenable,
	Pollingdisable
};

/*
 *===================================================================
 * 					APIs Supported by "MCAL SPI DRIVER"
 *===================================================================
 */

void MCAL_SPI_Init(volatile SPI_t *SPIx ,SPI_Config_t *SPI_Config);
void MCAL_SPI_Deinit(volatile SPI_t *SPIx );

void MCAL_SPI_Set_Pin(volatile SPI_t *SPIx );

void MCAL_SPI_SendData(volatile SPI_t *SPIx ,uint16_t *pTxBuffer, enum PollingMechism PollingEn);
void MCAL_SPI_ReceiveData(volatile SPI_t *SPIx ,uint16_t *pRxBuffer, enum PollingMechism PollingEn);


void MCAL_SPI_TX_RX(volatile SPI_t *SPIx ,uint16_t *pTxBuffer, enum PollingMechism PollingEn);


#endif /* INC STH32_F103C6_SPI_DRIVER_H_*/

/*
 *	Stm32F103C6_I2C_driver.h
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 23 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

#ifndef INC_STM32F103C6_I2C_DRIVER_H_
#define INC_STM32F103C6_I2C_DRIVER_H_

//include

#include "stm32f103x6.h"
#include "Stm32_F103C6_RCC_driver.h"
#include "Stm32_F103C6_gpio_driver.h"

struct S_I2C_Device_Address{
	uint16_t Enable_Dual_ADD ; //1- Enable 0-Disable
	uint16_t Primary_slave_address;
	uint16_t Secondary_slave_address;
	uint16_t I2C_addressing_Slave_mode;	//@ref I2C_addressing_Slave
};

typedef enum{
	I2C_EV_STOP,
	I2C_ERROR_AF,
	I2C_EV_ADDR_Matched,
	I2C_EV_DATA_REQ,	//the App layer should send the data (I2c_SlaveSendData) in this state
	I2C_EV_DATA_RCV	//the App layer should read the data (I2c_SlaveReceiveData) in this state

}Slave_State;

typedef enum {
	With_Stop,
	Without_Stop
}Stop_Condition;

typedef enum {
	Start,
	repeated_start
}Repeated_Start;

typedef enum{RESET,SET}
FlagStatus;

typedef enum{DISABLE,ENABLE}FanctionalState;

typedef enum {
	I2C_FLAG_BUSY=0,
	EV5,//EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	EV6,//EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	EV8_1,//EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
	EV8,//EV8: TxE=1, shift register not empty, .data register empty, cleared by writing DR register
	EV7,//EV7: RxNE=1 cleared by reading DR register
	MASTER_BYTE_TRANSMITTING=((uint32_t)0x00070080)/*TRE , BUSY, MSL, TXE flags*/
}Status;

typedef enum {I2C_Direction_Transmitter,I2C_Direction_Receiver}I2C_Direction;

#define I2C_EVENT_MASTER_BYTE_TRANSMITTING	((uint32_t)0x00070080)/*TRE , BUSY, MSL, TXE flags*/

//Config Stucture
typedef struct{

	uint32_t 	I2C_ClockSpeed;		 	 // specifies the clock frequence
										//This parameter must be set to @ref I2C_SCLK

	uint32_t 	StretchMode ;			//@ref I2C_StretchMode

	uint32_t 	I2C_Mode; 				 //Specifies the I2C mode
										//This paramter can be a value of @ref I2C_mode

	struct S_I2C_Device_Address	I2C_Device_Address;

	uint32_t 	I2C_ACK_Control;		//Enable and Disable the Auto ACK
										//@ref I2C_ACK_Control

	uint32_t	General_Call_Address_Detection; //@ref I2C_ENGC

	void (*P_Slave_Event_CallBack)(Slave_State state);

}I2C_InitTypeDef;

//==================================
//Macros Configuration References
//==================================

//@I2C SCLK
//Stantard Speed (up to 100kHz)
//Fast Speed (up to 400kHz)
//to configure Clock

//	I2C_CR2 FREQ[5:0] peripheral input clock
//	Configure the clock control registers befor enable the peripheral
//		Sm or Fm
// 			in Sm Thigh = CCR * TPCLK1 = TLow
//	Configure the rise time register TRISE[5:0]
//		the maximum allowed SCL rise time is 1000 ns. (1000 ns * TRISE[5:0] = res + 1)
// 	Program the I2C_CR1 register to enable the peripheral
//	Set the START bit in the I2C_CR1 register to generate a Start condition

#define I2C_SCLK_SM_50K			50000U
#define I2C_SCLK_SM_100K		100000U
#define I2C_SCLK_FM_200K		200000U //Fm not supported
#define I2C_SCLK_FM_400K		400000U

//@ref I2C_StretchMode
//Bit 7 NOSTRETCH: Clock stretching disable (Slave mode)
//	0: Clock stretching enabled
//	1: Clock stretching disabled

#define I2C_StretchMode_Disabled 	I2C_CR1_NOSTRETCH
#define I2C_StretchMode_Enabled		0x00000000UL

//@ref I2C_mode
//CR1.Bit 1
//0: I2C mode
//1: SMBus mode

#define I2C_mode_I2C 		0
#define I2C_mode_SMBus 		I2C_CR1_SMBUS

//@ref I2C_addressing_Slave
//Own address register 1 (I2C_OAR1)
//	Bit 15 ADDMODE Addressing mode (slave mode)
//	0: 7-bit slave address (10-bit address not acknowledged)
//	1: 10-bit slave address (7-bit address not acknowledged)
#define I2C_addressing_Slave_mode_7Bit	0x00000000UL
#define I2C_addressing_Slave_mode_10Bit	(uint16_t)(1<<15)

//@ref I2C_ACK_Control
#define I2C_ACK_Control_Enable 		I2C_CR1_ACK
#define I2C_ACK_Control_Disable 	0x00000000

//@ref I2C_ENGC
//Bit 6 ENGC: General call enable
//0: General call disabled. Address 00h is NACKed.
//1: General call enabled. Address 00h is ACKed.

#define I2C_ENGC_Enable 	I2C_CR1_ENGC
#define I2C_ENGC_Disable 	0x00000000


/*
 *===================================================================
 * 					APIs Supported by "MCAL I2C DRIVER"
 *===================================================================
 */


void MCAL_I2C_Init(volatile I2C_t *I2Cx,I2C_InitTypeDef *I2C_InitStruct);
void MCAL_I2C_Deinit(volatile I2C_t *I2Cx);

void MCAL_I2C_Set_Pin(volatile I2C_t *I2Cx);

//	Master Polling Mechanism
void MCAL_I2C_Master_TX(volatile I2C_t *I2Cx,uint16_t devAdd, uint8_t* dataOut,uint32_t dataLen,Stop_Condition Stop,Repeated_Start start);
void MCAL_I2C_Master_RX(volatile I2C_t *I2Cx,uint16_t devAdd, uint8_t* dataIn,uint32_t dataLen,Stop_Condition Stop,Repeated_Start start);

//Slave interrupt Mechanism
void MCAL_I2C_SlaveSendData(volatile I2C_t *I2Cx, uint8_t data);
uint8_t MCAL_I2C_SlaveReceiveData(volatile I2C_t *I2Cx);

//Generic APIs
void I2C_GenarateSTART(volatile I2C_t *I2Cx,FanctionalState NewState, Repeated_Start start);
FlagStatus I2C_GetFlagStatus(volatile I2C_t *I2Cx , Status flag);
void I2C_SetAddress(volatile I2C_t *I2Cx , uint16_t Address ,I2C_Direction Direction);
void I2C_GenerateSTOP(volatile I2C_t *I2Cx,FanctionalState NewState);
void I2C_AcknowladgeConfig(volatile I2C_t *I2Cx,FanctionalState NewState);
void Slave_States (volatile I2C_t *I2Cx,Slave_State state);
#endif /*	_I2C_DRIVER	*/

/*
 *	Stm32F103C6_I2C_driver.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 23 2026
 *  Layer   	: MCAL
 *  Target  	: STM32F103 / Cortex-M3
 *
 */

#include "Stm32_F103C6_I2C_driver.h"
#include "Stm32_F103C6_gpio_driver.h"


/*
 *==========================================================
 *                  	Generic Macros
 *==========================================================
 */

#define I2C1_INDEX	0
#define I2C2_INDEX	1

/*
 *===========================================================
 *					 	Generic Variable
 *===========================================================
 */

I2C_InitTypeDef Global_I2C_Config[2] ;

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


void MCAL_I2C_Init(volatile I2C_t *I2Cx,I2C_InitTypeDef *I2C_InitStruct){

	uint32_t freqrange = 0,tmpreg = 0;
	uint32_t pclk1 = 8000000;
	uint16_t result = 0 ;

	//RCC Enable
	if (I2Cx == I2C1){
		Global_I2C_Config[I2C1_INDEX]= *I2C_InitStruct;
		RCC_I2C1_CLK_EN();
	}else if (I2Cx == I2C2){
		Global_I2C_Config[I2C2_INDEX]= *I2C_InitStruct;
		RCC_I2C2_CLK_EN();
	}

	if(I2C_InitStruct->I2C_Mode == I2C_mode_I2C){

		/*--------------	INIT TIMING	-----------------*/

		//Bits 5:0 FREQ[5:0]: Peripheral clock frequency
		/*	Get the the I2Cx->CR2 value	*/
		tmpreg = I2Cx->CR2;
		tmpreg &= ~(I2C_CR2_FREQ_Msk);
		// get pclk1 freqeunce
		pclk1 = MCAL_RCC_GetPCLK1Freq();

		freqrange = (uint16_t)(pclk1/1000000);
		if ((freqrange >= 2U) && (freqrange <= 36U)){

			tmpreg |= freqrange;
			I2Cx->CR2 = tmpreg;
		} else{
			// ERROR Handle
		}

		//configure clock control register (CCR)
		//remmber the I2C must be disable
		I2Cx->CR1 &= ~(I2C_CR1_PE);

		tmpreg = 0;

		//configuer if you in Fm or Sm
		if ((I2C_InitStruct->I2C_ClockSpeed == I2C_SCLK_SM_50K) || (I2C_InitStruct->I2C_ClockSpeed == I2C_SCLK_SM_100K)){
			/*	standard mode speed calculate	 */

			// Tclk/2 = CCR * TPCLK1
			// CCR = Tclk/(2*TPCLK1)
			// CCR = FPCLK1/(2*I2C_clock_frequency)

			result = (uint16_t)(pclk1/(I2C_InitStruct->I2C_ClockSpeed << 1));
			 /* Minimum CCR value in Standard Mode is 4 */
			if (result < 4)
			{
				result = 4;
			}
			/* FS = 0 => Standard Mode */
			tmpreg |= (result & 0x0FFF);
			I2Cx->CCR = tmpreg;
//			-------------	I2C TRISE[5:0] Configurtion	-----------------------
//			For instance: in Sm mode, the maximum allowed SCL rise time is 1000 ns.
//			If, in the I2C_CR2 register, the value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns
//			therefore the TRISE[5:0] bits must be programmed with 09h.
//			(1000 ns / 125 ns = 8 + 1)

			I2Cx->TRISE = freqrange + 1;

		}else {
			// Fm not supported
		}

		// --------------	I2Cx->CR1 Configuration	-----------------

		// Get value of I2Cx-> CR1 to save it and update his value
		tmpreg = I2Cx->CR1;

		tmpreg |= (uint16_t)(I2C_InitStruct->I2C_ACK_Control | I2C_InitStruct->General_Call_Address_Detection | I2C_InitStruct->I2C_Mode) ;
		/* Write value to I2Cx->CR1 Register */
		I2Cx->CR1 = tmpreg;

		/*-------------	I2Cx OAR1 & I2Cx OAR2 Configuration	-------------*/
		tmpreg = 0;

		if (I2C_InitStruct->I2C_Device_Address.Enable_Dual_ADD)//Dual_ADD mode
		{
			tmpreg =I2C_OAR2_ENDUAL;
			tmpreg |= (uint16_t)(I2C_InitStruct->I2C_Device_Address.Secondary_slave_address<<I2C_OAR2_ADD2_Pos);
			I2Cx->OAR2 = tmpreg;
		}
		tmpreg = 0;
		tmpreg |= I2C_InitStruct->I2C_Device_Address.Primary_slave_address<<1;
		tmpreg |= I2C_InitStruct->I2C_Device_Address.I2C_addressing_Slave_mode;
		I2Cx->OAR1 = tmpreg;

		}else{
		//SMBUS Not Supported
		}

		// interrupt Mode (Slave Mode)
		if(I2C_InitStruct->P_Slave_Event_CallBack != NULL)// THat Enable Slave IRQ Slave Mode
		{

			I2Cx->CR2 |= (I2C_CR2_ITBUFEN);
			I2Cx->CR2 |= (I2C_CR2_ITERREN);
			I2Cx->CR2 |= (I2C_CR2_ITEVTEN);

			if(I2Cx == I2C1){
				NVIC_IRQ31_I2C1_EV_Enable ;
				NVIC_IRQ32_I2C1_ER_Enable;

			}else if(I2Cx == I2C1){
				NVIC_IRQ33_I2C2_EV_Enable;
				NVIC_IRQ34_I2C2_ER_Enable;
			}
			I2Cx->SR1 = 0 ;
			I2Cx->SR2 = 0 ;

		}
		/*	Enalbe the selected I2C preipheral	 */

		I2Cx->CR1 |= I2C_CR1_PE;

}

void MCAL_I2C_Deinit(volatile I2C_t *I2Cx){
	if(I2Cx == I2C1){
		NVIC_IRQ31_I2C1_EV_Disable ;
		NVIC_IRQ32_I2C1_ER_Disable;
		RCC_I2C1_CLK_Reset();

	}else if(I2Cx == I2C1){
		NVIC_IRQ33_I2C2_EV_Disable;
		NVIC_IRQ34_I2C2_ER_Disable;
		RCC_I2C2_CLK_Reset();
	}
}

void MCAL_I2C_Set_Pin(volatile I2C_t *I2Cx){
	// All Pins AL_OD
	GPIO_PinConfig_t PinCfg;

	if(I2Cx == I2C1){
		//PB10 I2C2_SCL
		//PB11 I2C2_SDA
		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		PinCfg.GPIO_Output_Speed =  GPIO_SPEED_10M;
		PinCfg.GPIO_PinNumber = GPIO_PIN_10 ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);


		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		PinCfg.GPIO_Output_Speed =  GPIO_SPEED_10M;
		PinCfg.GPIO_PinNumber = GPIO_PIN_11 ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);
	}else if(I2Cx == I2C1){
		//PB5 I2C1_SMBAl
		//PB6 I2C1_SCL
		//PB7 I2C1_SDA

		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		PinCfg.GPIO_Output_Speed =  GPIO_SPEED_10M;
		PinCfg.GPIO_PinNumber = GPIO_PIN_6 ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);

		PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_AF_OD;
		PinCfg.GPIO_Output_Speed =  GPIO_SPEED_10M;
		PinCfg.GPIO_PinNumber = GPIO_PIN_7 ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);
	}

}
void MCAL_I2C_Master_TX(volatile I2C_t *I2Cx,uint16_t devAdd, uint8_t* dataOut,uint32_t dataLen,Stop_Condition Stop,Repeated_Start start){

	int i = 0 ;

	//todo
	//support timeout (configure timer working for specific duration rise interrupt)
	//tiner_interrupt(){flag=1}
	//so any code exist in while (check_flag || Any polling conditions)
	//1-set the START bit in I2C_CR1 register to generate a start Condition
	I2C_GenarateSTART(I2Cx, ENABLE,  start);

	//2- wait for EV5
	//EV5: SB=1,cleared by reading SR1 register followed by writing DR register with address
	while (!I2C_GetFlagStatus(I2Cx , EV5));

	//3- Send Address
	I2C_SetAddress(I2Cx ,devAdd ,I2C_Direction_Transmitter);

	//4- wait EV6
	//EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while (!I2C_GetFlagStatus(I2Cx , EV6));

	//5-/*TRE , BUSY, MSL, TXE flags*/
	while (!I2C_GetFlagStatus(I2Cx , MASTER_BYTE_TRANSMITTING));

	for(i=0;i<dataLen;i++){
		//Write in DR register the data to be sent
		I2Cx->DR = dataOut[i];
		//5- wait EV8
		//EV8: TxE=1, shift register not empty, .data register empty, cleared by writing DR register
		while (!I2C_GetFlagStatus(I2Cx , EV8));
	}

	//6- wait EV8_2
	//EV8_2: TxE=1, BTF = 1, Program Stop request. TxE and BTF are cleared by hardware by the Stop condition
	//while (!I2C_GetFlagStatus(I2Cx , EV8_2));

	//7- Send Stop condition
	if(Stop == With_Stop){

		I2C_GenerateSTOP(I2Cx,ENABLE);
	}
}
void MCAL_I2C_Master_RX(volatile I2C_t *I2Cx,uint16_t devAdd, uint8_t* dataIn,uint32_t dataLen,Stop_Condition Stop,Repeated_Start start){

	uint8_t index = I2Cx==I2C1?I2C1_INDEX:I2C2_INDEX;
	int i = 0 ;
	//1-set the START bit in I2C_CR1 register to generate a start Condition
	I2C_GenarateSTART(I2Cx, ENABLE,  start);

	//2- wait for EV5
	//EV5: SB=1,cleared by reading SR1 register followed by writing DR register with address
	while (!I2C_GetFlagStatus(I2Cx , EV5));

	//3- Send Address
	I2C_SetAddress(I2Cx ,devAdd ,I2C_Direction_Receiver);

	I2C_AcknowladgeConfig(I2Cx,ENABLE);

	//4- wait EV6
	//EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while (!I2C_GetFlagStatus(I2Cx , EV6));

	if(dataLen){
		for(i=dataLen;i>1;i--){
			//Read in DR register the data to be sent
			I2Cx->DR = dataIn[i];
			//5- wait until Len becomes 1
			//EV7: RxNE=1 cleared by reading DR register
			while (!I2C_GetFlagStatus(I2Cx , EV7));
			//read the data from register
			*dataIn = I2Cx->DR;
			//increment the buffer
			dataIn++;
		}
		I2C_AcknowladgeConfig(I2Cx,DISABLE);
	}
	//6- Send Stop condition
	if(Stop == With_Stop){

		I2C_GenerateSTOP(I2Cx,ENABLE);
	}
	//Acknowladge re-enable
	if(Global_I2C_Config[index].I2C_ACK_Control == I2C_ACK_Control_Enable){
		I2C_AcknowladgeConfig(I2Cx,ENABLE);
	}
}

void MCAL_I2C_SlaveSendData(volatile I2C_t *I2Cx, uint8_t data){
	I2Cx->DR = data;
}
uint8_t MCAL_I2C_SlaveReceiveData(volatile I2C_t *I2Cx){
	return (uint8_t)I2Cx->DR ;
}

void I2C_GenarateSTART(volatile I2C_t *I2Cx,FanctionalState NewState, Repeated_Start start){
	if (start != repeated_start){
		//Check if the Bus is idle
		while (I2C_GetFlagStatus(I2Cx , I2C_FLAG_BUSY));
	}

//	Bit 8 START: Start generation
//	This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
//	In Master Mode:
//	0: No Start generation
//	1: Repeated start generation
//	In Slave mode:
//	0: No Start generation
//	1: Start generation when the bus is free

	if (NewState != DISABLE){
		/*	Generate a START condition	*/
			I2Cx->CR1 |= I2C_CR1_START;
	}else {
		/*	Disable the START condition	*/
		I2Cx->CR1 &= ~(I2C_CR1_START);
	}
}

FlagStatus I2C_GetFlagStatus(volatile I2C_t *I2Cx , Status flag){
	FlagStatus bitstatus =RESET;
	//volatile uint32_t dummyRead ;
	uint32_t flag1 = 0,flag2 = 0;
	uint32_t lastEvent = 0;

	switch(flag){
		case I2C_FLAG_BUSY:{
//			Bit 1 BUSY: Bus busy
//			0: No communication on the bus
//			1: Communication ongoing on the bus
//			– Set by hardware on detection of SDA or SCL low
//			– cleared by hardware on detection of a Stop condition.
//			It indicates a communication in progress on the bus. This information is still updated when
//			the interface is disabled (PE=0).
			if(I2Cx->SR2 &(I2C_SR2_BUSY)){
				bitstatus =SET;
			}else{
				bitstatus =RESET;
			}
			break;
		}
		case EV5:{
//			Bit 0 SB: Start bit (Master mode)
//			0: No Start condition
//			1: Start condition generated.
//			– Set when a Start condition generated.
//			– Cleared by software by reading the SR1 register followed by writing the DR register, or by
//			hardware when PE=0
			if(I2Cx->SR1 &(I2C_SR1_SB)){
				bitstatus =SET;
			}else{
				bitstatus =RESET;
			}
			break;
		}
		case EV6:{
//			Bit 1 ADDR: Address sent (master mode)/matched (slave mode)
//			This bit is cleared by software reading SR1 register followed reading SR2, or by hardware
//			when PE=0.
//			Address matched (Slave)
//			0: Address mismatched or not received.
//			1: Received address matched.
//			– Set by hardware as soon as the received slave address matched with the OAR registers
//			content or a general call or a SMBus Device Default Address or SMBus Host or SMBus Alert
//			is recognized. (when enabled depending on configuration).
			if(I2Cx->SR1 &(I2C_SR1_ADDR)){
				bitstatus =SET;
			}else{
				bitstatus =RESET;
//				//cleared by reading SR1 register followed by writing DR register with address
//				dummyRead = I2Cx->SR2;
			}
			break;
		}
		case MASTER_BYTE_TRANSMITTING:{
			flag1 = I2Cx->SR1;
			flag2 = I2Cx->SR2;
			flag2 = flag2<<16;
			lastEvent = (flag1|flag2)&((uint32_t)0xFFFFFFFF);

			if ((lastEvent & flag)==flag){
				bitstatus =SET;
			}else{
				/*last event is different from I2C_EVENT */
				bitstatus =RESET;
			}
			break;
		}
		case EV8_1://EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
		case EV8://EV8: TxE=1, shift register not empty, .data register empty, cleared by writing DR register
		{
//			Bit 7 TxE: Data register empty (transmitters)
//			0: Data register not empty
//			1: Data register empty
			if(I2Cx->SR1 &(I2C_SR1_TXE)){
				bitstatus =SET;
			}else{
				bitstatus =RESET;
			}
			break;
		}
		case EV7://EV7: RxNE=1 cleared by reading DR register
		{
//			Bit 6 RxNE: Data register not empty (receivers)
//			0: Data register empty
//			1: Data register not empty
//			– Set when data register is not empty in receiver mode. RxNE is not set during address phase.
//			– Cleared by software reading or writing the DR register or by hardware when PE=0.
//			RxNE is not set in case of ARLO event.

			if(I2Cx->SR1 &(I2C_SR1_RXNE)){
				bitstatus =SET;
			}else{
				bitstatus =RESET;
			}
			break;
		}
	}
	return bitstatus;
}

//Supported just 7Bit mode
void I2C_SetAddress(volatile I2C_t *I2Cx , uint16_t Address ,I2C_Direction Direction){

	Address=(Address<<1);
	if (Direction==I2C_Direction_Transmitter){
		/*Set the address bit0 for read */
		Address &= ~(1<<0);
	}else if (Direction==I2C_Direction_Receiver){
		/*Reset the address bit0 for write */
		Address |= 1<<0;
	}
	/*	Send to data register	*/
	I2Cx->DR = Address;
}

void I2C_GenerateSTOP(volatile I2C_t *I2Cx,FanctionalState NewState){

//	Bit 9 STOP: Stop generation
//	The bit is set and cleared by software, cleared by hardware when a Stop condition is
//	detected, set by hardware when a timeout error is detected.
//	In Master Mode:
//	0: No Stop generation.
//	1: Stop generation after the current byte transfer or after the current Start condition is sent.
//	In Slave mode:
//	0: No Stop generation.
//	1: Release the SCL and SDA lines after the current byte transfer.
	if (NewState==ENABLE){
		I2Cx->CR1 |= I2C_CR1_STOP;
	}else{
		I2Cx->CR1 &= ~(I2C_CR1_STOP);
	}
}

void I2C_AcknowladgeConfig(volatile I2C_t *I2Cx,FanctionalState NewState){

//	Bit 10 ACK: Acknowledge enable
//	This bit is set and cleared by software and cleared by hardware when PE=0.
//	0: No acknowledge returned
//	1: Acknowledge returned after a byte is received (matched address or data)

	if (NewState == ENABLE){
		I2Cx->CR1 |= I2C_CR1_ACK;
	}else if (NewState == DISABLE){
		I2Cx->CR1 &= ~(I2C_CR1_ACK);
	}
}

void Slave_States (volatile I2C_t *I2Cx,Slave_State state){

	uint8_t index = I2Cx == I2C1? I2C1_INDEX: I2C2_INDEX;

	switch (state){

		case I2C_ERROR_AF:{
			//make sure that the slave is really in transmitter mode
			if(I2Cx->SR2 & (I2C_SR2_TRA)){

				//Slave Shouldn't Send anything else
			}
			break;
			}

		case I2C_EV_STOP:{
			//make sure that the slave is really in transmitter mode
			if(I2Cx->SR2 & (I2C_SR2_TRA)){
				//Notify APP that the Stop Condition is sent by the master
				Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_STOP);
			}
			break;
		}


		case I2C_EV_ADDR_Matched: {
			//Notify APP that the Stop Condition is sent by the master
			Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_ADDR_Matched);
			break;
		}

		case I2C_EV_DATA_REQ:{
			//make sure that the slave is really in transmitter mode
			if(I2Cx->SR2 & (I2C_SR2_TRA)){
				//the APP layer should send the data (I2C SlaveSendData) in this state
				Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_DATA_REQ);
			}
			break;
		}

		case I2C_EV_DATA_RCV:{
			//make sure that the slave is really in receiver mode
			if(!(I2Cx->SR2 & (I2C_SR2_TRA))){

				//the APP layer should read the data (MCAT I2C SlaveReceiveData) in this state
				Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_DATA_RCV);
			}
			break;
		}
	}
}
/*
 *==========================================================
 *                  	ISR Functions
 *==========================================================
 */

void I2C1_EV_IRQHandler(void){

	volatile uint32_t dummy_read = 0;

	volatile I2C_t *I2Cx = I2C1;

	//Interrupt handling for both master and slave mode of a device

	uint32_t temp1, temp2, temp3;

	temp1 = I2Cx->CR2 & (I2C_CR2_ITEVTEN);
	temp2 = I2Cx->CR2 & (I2C_CR2_ITBUFEN);
	temp3 = I2Cx->SR1 & (I2C_SR1_STOPF);

	// Handle For interrupt generated by STOPF event
		// Note: Stop detection flag is applicable only slave mode
	if(temp1 && temp3){
		//STOF flag is set
		//Clear the STOPF (i.e 1) read SR1 2) Write to CR1)
		I2Cx->CR1 |= 0x0000;

		Slave_States (I2Cx, I2C_EV_STOP);
	}

	//----------------------------------------------------

	temp3 = I2Cx->SR1 & (I2C_SR1_ADDR);

	// Handle For interrupt generated by ADDR event
		//Note: When master mode: Address is sent.

// 		When Slave mode Address matched with own address

	if(temp1 && temp3){

		// interrupt is generated because of ADDR event
		//check for device mode

		if(I2Cx->SR2 & (I2C_SR2_MSL)){

			//master
		}else{

		//slave mode
		//clear the ADDR flag (read SR1 read SR2)

		dummy_read = I2Cx->SR1;
		Slave_States(I2Cx, I2C_EV_ADDR_Matched);
		}
	}

	dummy_read = I2Cx->SR2;
	temp3 = I2Cx->SR1 & (I2C_SR1_TXE);
	// Handle For interrupt generated by TXE event
	if(temp1 && temp2 && temp3){

		//Check for device mode

		if(I2Cx->SR2 & (I2C_SR2_MSL)){

		}else{
			//slave
			Slave_States(I2Cx, I2C_EV_DATA_REQ);
		}
	}

//---------------------------------------------------
	temp3 = I2Cx->SR1 & (I2C_SR1_RXNE);
	// Handle For interrupt generated by	SB-1, cleared by reading SR1 register follow
	if(temp1 && temp2 && temp3){
		//check device mode.
		if(I2Cx->SR2 & (I2C_SR2_MSL)){
			//The device is master

		}else{
		//slave
		Slave_States (I2Cx, I2C_EV_DATA_RCV);
		}
	}
}

void I2C1_ER_IRQHandler(void){

}

void I2C2_EV_IRQHandler(void){

}

void I2C2_ER_IRQHandler(void){

}

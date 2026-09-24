/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 24 2026
 *
 */

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_USART_driver.h"
#include "Stm32_F103C6_SPI_driver.h"
#include "Stm32_F103C6_I2C_driver.h"
#include "I2C_Slave_EEPROM.h"


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

void Mohamed_I2C1_Callback(struct S_IRQ_SRC irq_src){


}

int main(void)
{

	clock_init();

	//Test case 1
	unsigned char ch1[]={0x1,0x2,0x3,0x4,0x5,0x6,0x7};
	unsigned char ch2[7]={0};

	eeprom_init();
	eeprom_write_Nbytes(0xAf,ch1,7);
	eeprom_read_byte(0xAf,ch2,7);

	//Test case 2
	ch1[1] = 0xA;
	ch1[2] = 0xB;
	ch1[3] = 0xC;
	ch1[4] = 0xD;

	eeprom_write_Nbytes(0xFFF,ch1,7);
	eeprom_read_byte(0xFFF,ch2,7);

	while(1){

    }
}

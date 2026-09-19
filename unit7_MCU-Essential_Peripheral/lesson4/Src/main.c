/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: SEP, 17 2026
 *
 */

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_EXTI_driver.h"

#include "lcd.h"
#include "keypad.h"

unsigned int IRQ_Flag = 0 ;


void EXTI9_Callback(void){
	IRQ_Flag = 1;
	LCD_WRITE_STRING("IRQ EXTI9 is happened  _|	");
}

void  clock_init(){
	//Enable clock GPIOA
	RCC_GPIOA_CLK_EN();
	//Enable clock GPIOB
	RCC_GPIOB_CLK_EN();
	//Enable clock AFIO
	RCC_AFIO_CLK_EN();
}

void GPIO_init(){

	//PA1 input highz floating   (reset state)
	GPIO_PinConfig_t PinCfg;
	PinCfg.GPIO_PinNumber = GPIO_PIN_1;
	PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
	MCAL_GPIO_Init(GPIOA,&PinCfg);

	//PB1 (output push-pull mode)
	//01: Output mode, max speed 10MHZ
	//CNF 00: General perpose output push-pull
	PinCfg.GPIO_PinNumber = GPIO_PIN_1;
	PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
	MCAL_GPIO_Init(GPIOB,&PinCfg);

	//PA13 input highz floating   (reset state)
	PinCfg.GPIO_PinNumber = GPIO_PIN_13;
	PinCfg.GPIO_MODE = GPIO_MODE_INPUT_FLO;
	MCAL_GPIO_Init(GPIOA,&PinCfg);

	//PB13 (output push-pull mode)
	//01: Output mode, max speed 10MHZ
	//CNF 00: General perpose output push-pull
	PinCfg.GPIO_PinNumber = GPIO_PIN_13;
	PinCfg.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	PinCfg.GPIO_Output_Speed = GPIO_SPEED_10M;
	MCAL_GPIO_Init(GPIOB,&PinCfg);
}

void EXTI_init(){

	EXTI_PinConfig_t EXTI_CFG;
	EXTI_CFG.EXTI_PIN = EXTI9PB9;
	EXTI_CFG.Trigger_Case = EXTI_Trigger_RISING;
	EXTI_CFG.P_IRQ_CallBack =EXTI9_Callback;
	EXTI_CFG.IRQ_EN = EXTI_IRQ_Enable;

	MCAL_EXTI_GPIO_Init(&EXTI_CFG);
}
void wait_ms(uint32_t time){
	uint32_t i ,j;
	for (i=0;i<time;i++)
		for (j=0;j<255;j++);
}

int main(void)
{

	clock_init();
	GPIO_init();
	EXTI_init();
	LCD_INIT();

	IRQ_Flag = 1;
    while(1){
    	if(IRQ_Flag){
    		LCD_clear_screen();
    		IRQ_Flag = 0;
    		wait_ms(1);
    	}
    }
}

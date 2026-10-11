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

#include "Stm32_F103C6_I2C_driver.h"

TaskHandle_t vHelloBlinkingLed = NULL;

/*	Task prototype	*/
void vHelloBlinkingLedHandler(void *parms);
void HW_Init(void){
	GPIO_PinConfig_t PIN13CFG;
	PIN13CFG.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	PIN13CFG.GPIO_Output_Speed = GPIO_SPEED_10M;
	PIN13CFG.GPIO_PinNumber = GPIO_PIN_13;

	MCAL_GPIO_Init(GPIOC, &PIN13CFG);
}

int main(void)
{

	HW_Init();
	xTaskCreate(vHelloBlinkingLedHandler, "BlinkingLed", 128, NULL, 1, &vHelloBlinkingLed);

	vTaskStartScheduler();

	while(1){

	}
	return 0;
}

void vHelloBlinkingLedHandler(void *parms){
	while(1){
		MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
		vTaskDelay(500);
		MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		vTaskDelay(500);
	}
}

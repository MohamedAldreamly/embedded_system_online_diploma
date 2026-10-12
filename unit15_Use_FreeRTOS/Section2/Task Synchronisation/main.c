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
#include "semphr.h"

#include "Stm32_F103C6_GPIO_driver.h"

//Handles
TaskHandle_t vTaskButtonHandel = NULL;
TaskHandle_t vTaskLedHandel = NULL;

SemaphoreHandle_t xSemaphore = NULL;

//To State Moniter Button
uint8_t Button_Curr_State = 0;
uint8_t Button_Prev_State = 0;


/*	Task prototype	*/
void vTaskButtonHandle(void *par);
void vTaskLedHandle(void *par);


void HW_Init(void){
	//for LED
	GPIO_PinConfig_t PIN13CFG;
	PIN13CFG.GPIO_MODE = GPIO_MODE_OUTPUT_PP;
	PIN13CFG.GPIO_Output_Speed = GPIO_SPEED_10M;
	PIN13CFG.GPIO_PinNumber = GPIO_PIN_13;

	MCAL_GPIO_Init(GPIOC, &PIN13CFG);

	//For Button
	GPIO_PinConfig_t PIN15CFG;
	PIN15CFG.GPIO_MODE = GPIO_MODE_INPUT_PD;
	PIN15CFG.GPIO_PinNumber = GPIO_PIN_15;

	MCAL_GPIO_Init(GPIOC, &PIN15CFG);
}

int main(void)
{

	HW_Init();

	xTaskCreate(vTaskButtonHandle, "Moniter Button", 128, NULL, 1, &vTaskButtonHandel);
	xTaskCreate(vTaskLedHandle, "Led Control", 128, NULL, 2, &vTaskLedHandel);

	xSemaphore =xSemaphoreCreateBinary();

	vTaskStartScheduler();

	while(1){

	}
	return 0;
}

void vTaskButtonHandle(void *par){
	while(1){
		Button_Curr_State = MCAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
		if(Button_Curr_State != Button_Prev_State){
			xSemaphoreGive(xSemaphore);
		}
		Button_Prev_State = Button_Curr_State;
		vTaskDelay(10);
	}
}

void vTaskLedHandle(void *par){
	while(1){
		if(xSemaphoreTake(xSemaphore,(TickType_t)5) == pdTRUE){
			if (Button_Curr_State == 1){
				MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
			}else {
				MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
			}
		}
		vTaskDelay(10);
	}
}


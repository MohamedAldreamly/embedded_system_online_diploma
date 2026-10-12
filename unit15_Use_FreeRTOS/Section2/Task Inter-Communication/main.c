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
#include "queue.h"


#include "Stm32_F103C6_GPIO_driver.h"

//Handles
TaskHandle_t vTaskMasterHandel = NULL;
TaskHandle_t vTaskSlaveHandel = NULL;

QueueHandle_t xQueueHandel = NULL;

//To State Moniter Button
uint8_t Button_Curr_State = 0;
uint8_t Button_Prev_State = 0;


/*	Task prototype	*/
void vTaskMasterHandle(void *par);
void vTaskSlaveHandle(void *par);



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

	xTaskCreate(vTaskMasterHandle, "Moniter Button", 128, NULL, 1, &vTaskMasterHandel);
	xTaskCreate(vTaskSlaveHandle, "Led Control", 128, NULL, 2, &vTaskSlaveHandel);

	xQueueHandel = xQueueCreate(10,sizeof(uint8_t));

	vTaskStartScheduler();

	while(1){

	}
	return 0;
}

void vTaskMasterHandle(void *par){

	uint8_t offCommand = 0;
	uint8_t onCommand = 1;

	while(1){
		xQueueSend(xQueueHandel , &offCommand , (TickType_t) 5);
		vTaskDelay(15);
		xQueueSend(xQueueHandel , &onCommand , (TickType_t) 5);
		vTaskDelay(15);
	}
}

void vTaskSlaveHandle(void *par){

	uint8_t CommandReceive = 0;

	while(1){
		xQueueReceive(xQueueHandel, &CommandReceive, (TickType_t) 5);
		if (CommandReceive == 1){
			MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
		}else if (CommandReceive == 0) {
			MCAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
		}

		vTaskDelay(10);
	}
}


/*
 *	main.c
 *
 *  Author  	: Mohamed Aldreamly
 *	Created on	: OCT, 7 2026
 *
 */

#include "core_cm3.h"

#include "stm32f103x6.h"

#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_EXTI_driver.h"

#include "Scheduler.h"

Task_ref Task1, Task2, Task3;
void task1(){
	while(1){
		//Task1 Code
	}
}

void task2(){
	while(1){
		//Task2 Code
	}
}

void task3(){
	while(1){
		//Task3 Code
	}
}

int main(void)
{
	MYRTOS_errorID error ;
	//	HW_Init (Initialize ClockTree, ResetController)
	HW_init();

	if(MYRTOS_init()!= NOError)
	{
		while(1);
	}

	Task1.Stack_Size = 1024;
	Task1.p_TaskEntry = task1;
	Task1.priority = 3;
	strcpy(Task1.TaskName,"task_2");
	error = MYRTOS_CreateTask(&Task1);

	Task2.Stack_Size = 1024;
	Task2.p_TaskEntry = task2;
	Task2.priority = 3;
	strcpy(Task2.TaskName,"task_2");
	error += MYRTOS_CreateTask(&Task2);

	Task3.Stack_Size = 1024;
	Task3.p_TaskEntry = task3;
	Task3.priority = 3;
	strcpy(Task3.TaskName,"task_3");
	error += MYRTOS_CreateTask(&Task3);

    while(1){

    }
}

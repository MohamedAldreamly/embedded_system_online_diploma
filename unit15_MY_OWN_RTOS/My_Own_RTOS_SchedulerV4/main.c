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

Task_ref Task1, Task2, Task3, Task4;
unsigned char Task1LED,Task2LED,Task3LED, Task4LED;

void task1(){
	static int Count = 0;
	while(1){
		//Task1 Code
		Task1LED ^=1;
		Count++;
		if(Count == 30){
			MYRTOS_ActivateTask(&Task4);
			Count = 0;
		}//100ms
		MYRTOS_TaskWait(100,&Task1);
	}
}

void task2(){
	while(1){
		//Task2 Code
		Task2LED ^=1;
		//300ms
		MYRTOS_TaskWait(300,&Task2);
	}
}

void task3(){
	while(1){
		//Task3 Code
		Task3LED ^=1;
		//500ms
		MYRTOS_TaskWait(500,&Task3);
	}
}

void task4(){
	while(1){
		//Task4 Code
		Task4LED ^=1;
		MYRTOS_TaskWait(1000,&Task4);
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

	Task1.Stack_Size = 512;
	Task1.p_TaskEntry = task1;
	Task1.priority = 3;
	strcpy(Task1.TaskName,"task_1");
	error = MYRTOS_CreateTask(&Task1);

	Task2.Stack_Size = 512;
	Task2.p_TaskEntry = task2;
	Task2.priority = 3;
	strcpy(Task2.TaskName,"task_2");
	error = MYRTOS_CreateTask(&Task2);

	Task3.Stack_Size = 512;
	Task3.p_TaskEntry = task3;
	Task3.priority = 3;
	strcpy(Task3.TaskName,"task_3");
	error = MYRTOS_CreateTask(&Task3);

	Task4.Stack_Size = 512;
	Task4.p_TaskEntry = task4;
	Task4.priority = 1;
	strcpy(Task4.TaskName,"task_4");
	error = MYRTOS_CreateTask(&Task4);

	MYRTOS_ActivateTask(&Task1);
	MYRTOS_ActivateTask(&Task2);
	MYRTOS_ActivateTask(&Task3);

	MYRTOS_STARTOS();

	return error;
	while(1){

    }

}

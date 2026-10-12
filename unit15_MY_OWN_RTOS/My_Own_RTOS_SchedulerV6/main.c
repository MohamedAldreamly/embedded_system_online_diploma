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
Mutex_ref MUTEX1,MUTEX2;
unsigned char payload[3] = {1,2,3};
unsigned char payload2[3] = {1,2,3};


// the comment to Try the Dead Lock State
void task1(){
	static int Count = 0;
	while(1){
		//Task1 Code
		Task1LED ^=1;
		Count++;
		if(Count == 100){
			MYRTOS_AcquireMutex(&MUTEX1,&Task1);
			MYRTOS_AcquireMutex(&MUTEX2,&Task1);
			MYRTOS_ActivateTask(&Task2);//
			//MYRTOS_ActivateTask(&Task4);
			//MYRTOS_AcquireMutex(&MUTEX2,&Task1);
		}
		if(Count == 200){
			Count = 0;
			MYRTOS_ReleaseMutex(&MUTEX1);
		}
	}
}

void task2(){
	static int Count = 0;
	while(1){
		//Task2 Code
		Task2LED ^=1;
		Count++;
		if(Count == 100){
			MYRTOS_ActivateTask(&Task3);
		}
		if(Count == 200){
			Count = 0;
			MYRTOS_TerminateTask(&Task2);
		}
	}
}

void task3(){
	static int Count = 0;
	while(1){
		//Task3 Code
		Task3LED ^=1;
		Count++;
		if(Count == 100){
			MYRTOS_ActivateTask(&Task4);
		}
		if(Count == 200){
			Count = 0;
			MYRTOS_TerminateTask(&Task3);
		}
	}
}

void task4(){
	static int Count = 0;
	while(1){
		//Task4 Code
		Task4LED ^=1;
		Count++;
		if(Count == 3){
			//MYRTOS_AcquireMutex(&MUTEX2, &Task4);
			MYRTOS_AcquireMutex(&MUTEX1, &Task4);
		}
		if(Count == 200){
			Count = 0;
			MYRTOS_ReleaseMutex(&MUTEX1);
			MYRTOS_TerminateTask(&Task4);
		}
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

	MUTEX1.PayloadSize = 3;
	MUTEX1.Ppayload = payload ;
	strcpy(MUTEX1.MutexName,"mutex_shared_T1_T4");

	MUTEX2.PayloadSize = 3;
	MUTEX2.Ppayload = payload2 ;
	strcpy(MUTEX2.MutexName,"mutex2_shared_T1_T4");

	Task1.Stack_Size = 512;
	Task1.p_TaskEntry = task1;
	Task1.priority = 4;
	strcpy(Task1.TaskName,"task_1");
	error = MYRTOS_CreateTask(&Task1);

	Task2.Stack_Size = 512;
	Task2.p_TaskEntry = task2;
	Task2.priority = 3;
	strcpy(Task2.TaskName,"task_2");
	error = MYRTOS_CreateTask(&Task2);

	Task3.Stack_Size = 512;
	Task3.p_TaskEntry = task3;
	Task3.priority = 2;
	strcpy(Task3.TaskName,"task_3");
	error = MYRTOS_CreateTask(&Task3);

	Task4.Stack_Size = 512;
	Task4.p_TaskEntry = task4;
	Task4.priority = 1;
	strcpy(Task4.TaskName,"task_4");
	error = MYRTOS_CreateTask(&Task4);

	MYRTOS_ActivateTask(&Task1);

	MYRTOS_STARTOS();

	return error;
	while(1){

    }

}

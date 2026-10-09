/*
 * Scheduler.h
 *
 *	Created on	: OCT, 9 2026
 *  Author: Mohamed aldremly
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "CortexMX_OS_porting.h"
#include <string.h>


typedef struct{
	unsigned int Stack_Size;
	unsigned char priority;
	void (*p_TaskEntry)(void); 	// pointer to Tack C Function
	unsigned char AutoStart;
	unsigned int _S_PSP_Task; 	// Not Entered by the user
	unsigned int _E_PSP_Task; 	// Not Entered by the user
	unsigned int* Current_PSP; 	// Not Entered by the user
	char TaskName[30];
	enum{
		Suspend,
		Running,
		Waiting,
		ready
	}TaskState;	// Not Entered by the user
	struct {
		enum{
			Enable,
			Disable
		}Blocking;
		unsigned int Ticks_Count;
	}TimingWaiting;
}Task_ref;



typedef enum{
	NOError,
	Ready_Queue_init_error,
	Task_exceeded_StackSize
}MYRTOS_errorID;


//APLs
MYRTOS_errorID MYRTOS_init();
MYRTOS_errorID MYRTOS_CreateTask(Task_ref* Tref);

#endif /* SCHEDULER_H_ */


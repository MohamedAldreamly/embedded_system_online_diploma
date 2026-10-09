/*
 * Scheduler.c
 *
 *	Created on	: OCT, 9 2026
 *  Author: Mohamed aldremly
 */

#include "Scheduler.h"
#include "MYRTOS_FIFO.h"

FIFO_Buf_t Ready_QUEUE ;
Task_ref* Ready_QUEUE_FIFO[100];
Task_ref MYRTOS_IDLETack;

struct{
	Task_ref* OSTasks[100];
	unsigned int _S_MSP_Task; 	// Not Entered by the user
	unsigned int _E_MSP_Task; 	// Not Entered by the user
	unsigned int PSP_Task_Locator;
	unsigned int NoOfActiveTacks;
	Task_ref* CurrentTask;
	Task_ref* NextTask;
	enum{
		OSsuspend,
		OSRunning
	}OSmodeID;
}OS_Control;

void MYRTOS_idleTack(){
	while(1){
		__asm("nop");
	}
}

void OS_SVC_services (int* stackFromPinter){

	//OS_SVC_Set Stack -> r0 -> argument = = stackFromPinter
	//OS_SVC_Set Stack : r0,r1,r2,r3,r12,LR,PC,XPSR
	unsigned char SVC_number ;

	SVC_number = *((unsigned char*)(((unsigned char*)stackFromPinter[6])[-2]));

	switch (SVC_number){
		case 0:	//Activate Task
			break;
		case 1:	//Terminate Task
			break;
		case 2: //
			break;
		case 3: //
			break;
		case 4: //
			break;
		}
}

void PendSV_Handler(void){

}

void OS_SVC_Set(int SVC_ID){

	switch (SVC_ID){
	case 0:	//Activate Task
		__asm("SVC #0x00");
		break;
	case 1: //Terminate Task
		__asm("SVC #0x01");
		break;
	case 2: //
		__asm("SVC #0x02");
		break;
	}

}

MYRTOS_errorID MYRTOS_Create_MainStack(){
	OS_Control._S_MSP_Task = (unsigned int)&_estack;
	OS_Control._E_MSP_Task = OS_Control._S_MSP_Task - MainStackSize;
	//Aligned 8 Bytes spaces between Main Task and PSP tasks
	OS_Control.PSP_Task_Locator = (OS_Control._E_MSP_Task - 8);

	// if(_E_MSP_Task< &_eheap) Error:excedded the availble size
	return NOError;
}

MYRTOS_errorID MYRTOS_init(){

	MYRTOS_errorID error = NOError;

	//Update OS Mode (OSsuspend)
	OS_Control.OSmodeID = OSsuspend;
	//specify the Main Stack for OS
	MYRTOS_Create_MainStack();
	FIFO_Buf_Status FIFO_init		(FIFO_Buf_t* fifo,element_type* buf, unsigned int length);

	//Create OS Ready Queue
	if (FIFO_init(&Ready_QUEUE,Ready_QUEUE_FIFO,100) != FIFO_no_error){
		error += Ready_Queue_init_error;
	}

	//Confiuration
	strcpy(MYRTOS_IDLETack.TaskName,"idletask");
	MYRTOS_IDLETack.priority = 255;
	MYRTOS_IDLETack.p_TaskEntry = MYRTOS_idleTack;
	MYRTOS_IDLETack.Stack_Size = 300;
	error = MYRTOS_CreateTask(&MYRTOS_IDLETack);

	return error;


}



void MyRTOS_Create_TaskStack(Task_ref*Tref){

	/*	Task Frame
	 * 	==========
	 * 	XPSR
	 * 	PC
	 * 	LR
	 * 	r12
	 * 	r4
	 * 	r3
	 * 	r2
	 * 	r1
	 * 	r0

	 * 	=======
	 * 	=======
	 * 	r5, r6, r7, r8, r9, r10, r11 (Saved/Restore) Manual
	 *
	 */

	Tref->Current_PSP = (unsigned int*) Tref->_S_PSP_Task;

	Tref->Current_PSP--;
	*(Tref->Current_PSP) = 0x01000000; /*DUMMY_XPSR should T = 1 */

	Tref->Current_PSP--;
	*(Tref->Current_PSP) = (unsigned int)Tref->p_TaskEntry; //PC

	Tref->Current_PSP--;
	*(Tref->Current_PSP) = 0xFFFFFFFD; /*DUMMY_XPSR should T = 1 */

	for(int j=0; j<13;j++){
		Tref->Current_PSP--;
		*(Tref->Current_PSP) = 0;
	}
}

MYRTOS_errorID MYRTOS_CreateTask(Task_ref* Tref){

	MYRTOS_errorID error = NOError;

	//Create Its Own PSP stack
	Tref->_S_PSP_Task = OS_Control.PSP_Task_Locator;
	Tref->_E_PSP_Task = Tref->_S_PSP_Task - Tref->Stack_Size;

	//	-					-
	//	-	_S_PSP_Task		-
	//	-	Task Stack		-
	//	-	_S_PSP_Task		-
	//	-					-
	//	-	_eheap			-
	//	-

	//Check task stack size exceeded rhe PSP stack
	if(Tref->_E_PSP_Task <(unsigned int)(&(_eheap))){
		return Task_exceeded_StackSize;
	}

	//Aligned 8 Bytes space between Task PSP and other
	//OS_Control.PSP_Task_Locator = (OS_Control._E_MSP_Task - 8);
	OS_Control.PSP_Task_Locator = (Tref->_E_PSP_Task - 8);

	//Initialize PSP Task Stack
	MyRTOS_Create_TaskStack(Tref);

	//Task State Update -> suspend
	Tref->TaskState = Suspend;
	return error;
}





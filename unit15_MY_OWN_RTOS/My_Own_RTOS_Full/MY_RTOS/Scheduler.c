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

typedef enum{
	SVC_ActivateTask,
	SVC_TerminateTask,
	SVC_TaskWaitingTime,
	SVC_AquireMutex,
	SVC_ReleaseMutex
}SVC_ID;

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

unsigned char idleTackLED;

void MYRTOS_idleTack(){
	while(1){
		idleTackLED ^=1;
		__asm volatile ("wfe");
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

	//Create OS Ready Queue
	if (FIFO_init(&Ready_QUEUE,Ready_QUEUE_FIFO,100) != FIFO_no_error){
		error = Ready_Queue_init_error;
	}

	//Confiuration
	strcpy(MYRTOS_IDLETack.TaskName,"idletask");
	MYRTOS_IDLETack.priority = 255;
	MYRTOS_IDLETack.p_TaskEntry = MYRTOS_idleTack;
	MYRTOS_IDLETack.Stack_Size = 300;
	error = MYRTOS_CreateTask(&MYRTOS_IDLETack);
	return error;
}

//the Error Return function to detect Error
void Task_Exit_Error(){
	while(1){
		__asm volatile ("nop");
	}
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
	*(Tref->Current_PSP) = (unsigned int)Task_Exit_Error;
	//*(Tref->Current_PSP) = 0xFFFFFFFD;

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

	//update Scheduler Table
	OS_Control.OSTasks[OS_Control.NoOfActiveTacks] = Tref;
	OS_Control.NoOfActiveTacks++;

	//Task State Update -> suspend
	Tref->TaskState = Suspend;
	return error;
}

//Handler Mode
void bubbleSort(void)
{
    unsigned int i, j, n;
    Task_ref* temp;
    n = OS_Control.NoOfActiveTacks;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (OS_Control.OSTasks[j]->priority > OS_Control.OSTasks[j+1]->priority)
            {
                temp = OS_Control.OSTasks[j];
                OS_Control.OSTasks[j] = OS_Control.OSTasks[j+1];
                OS_Control.OSTasks[j+1] = temp;
            }
        }
    }
}

//Handler Mode
void MYRETOS_Update_Schaduler_table(){

	Task_ref* temp = NULL;
	Task_ref* Ptask;
	Task_ref* PnextTask;
	int i = 0;

	//1- Bubble sort Scheduler OS_Control -> OSTASKS[100] (priority high then low)
	bubbleSort();
	//2- free Ready Queue
	while(FIFO_dequeue(&Ready_QUEUE, &temp/*ptr to ptr*/) != FIFO_empty);
	//3- Update Ready Queue
	while (i < OS_Control.NoOfActiveTacks){
		Ptask = OS_Control.OSTasks[i];
		 if (i + 1 < OS_Control.NoOfActiveTacks){
			 PnextTask = OS_Control.OSTasks[i + 1];
		 }
		 else{
			 PnextTask = NULL;
		 }
		if (Ptask->TaskState != Suspend){
			//in case we reached to the end of avalible OSTASKS
			if (PnextTask->TaskState == Suspend){
				FIFO_enqueue(&Ready_QUEUE, Ptask);
				Ptask->TaskState = ready;
				break;
			}
			//if the Ptask priority > PnextTask then (lowest number is meaning higher priority)
			if(Ptask->priority < PnextTask->priority){
				FIFO_enqueue(&Ready_QUEUE, Ptask);
				Ptask->TaskState = ready;
				break;
			}else if (Ptask->priority == PnextTask->priority){
				//	if the Ptask priority == Ptask then
				//	push Ptask in Ready State
				//	And make Ptask = PnextTask and PnextTask++
				FIFO_enqueue(&Ready_QUEUE, Ptask);
				Ptask->TaskState = ready;

			}else if (Ptask->priority > PnextTask->priority){
				//not allowed to happen as we already reordered it by bubble sort
				break;
			}
		}
		i++;
	}

}
//Handler Mode
void Decide_whatNext(){
	//if Ready OUEUE is Empty && OS_Control->CurrentTask != Suspend
	if ((Ready_QUEUE.count)==0 && OS_Control.CurrentTask->TaskState != Suspend){
		OS_Control.CurrentTask->TaskState = Running;
		//add the current task again (round robin)
		FIFO_enqueue(&Ready_QUEUE, OS_Control.CurrentTask);
		OS_Control.NextTask = OS_Control.CurrentTask;
	}else{
		FIFO_dequeue(&Ready_QUEUE, &OS_Control.NextTask);
		OS_Control.NextTask->TaskState = Running;
		//Update ready queue (to keep round robin algo. happen)
		if((OS_Control.NextTask->priority == OS_Control.CurrentTask->priority)&&(OS_Control.CurrentTask->TaskState != Suspend))
		{
			FIFO_enqueue(&Ready_QUEUE, OS_Control.CurrentTask);
			OS_Control.CurrentTask->TaskState = ready;
		}
	}
}

__attribute ((naked)) void PendSV_Handler(void){
	//=======================================
	//Save the Context of the Current Task
	//=======================================
	//Get the Current Task "Current PSP from CPU Register" as CPU Push XPSR,....,R0
	OS_GET_PSP(OS_Control.CurrentTask->Current_PSP);
	//using this Current_PSP (pointer) to store (R5 to R11)

	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R4" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R5" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R6" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R7" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R8" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R9" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R10" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP--;
	__asm volatile ("MOV %0,R11" : "=r"(*(OS_Control.CurrentTask->Current_PSP)));


	//save the current value of PSP
	//already saved in Current_PSP

	//=======================================
	//Restore the Context of the Next Task
	//=======================================

	if(OS_Control.NextTask != NULL){
		OS_Control.CurrentTask = OS_Control.NextTask;
		OS_Control.NextTask = NULL;
	}

	__asm volatile ("MOV R11,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R10,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R9,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R8,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R7,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R6,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R5,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;
	__asm volatile ("MOV R4,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
	OS_Control.CurrentTask->Current_PSP++;

//	__asm volatile ("MOV R4,%0" : : "r"(*(OS_Control.CurrentTask->Current_PSP)));
//	OS_Control.CurrentTask->Current_PSP++;

	//update PSP and exit
	OS_SET_PSP(OS_Control.CurrentTask->Current_PSP);
	//return to Thread Mode but Get Value from PSP
	__asm volatile ("BX LR");
}

//Handler Mode
void OS_SVC (int* StackFrame){

	//OS_SVC_Set Stack -> r0 -> argument = = stackFromPinter
	//OS_SVC_Set Stack : r0,r1,r2,r3,r12,LR,PC,XPSR
	unsigned char SVC_number ;

	//SVC_number = *((unsigned char*)(((unsigned char*)StackFrame[6])[-2]));
	SVC_number = ((uint8_t *)StackFrame[6])[-2];

	switch (SVC_number){
		case SVC_ActivateTask:
		case SVC_TerminateTask:
			//Update Scheduler Table,Ready Queue
			MYRETOS_Update_Schaduler_table();
			//OS is in Running State
			if(OS_Control.OSmodeID == OSRunning){

				if(strcmp(OS_Control.CurrentTask->TaskName, "idletask") != 0){
					//Decide what Next
					Decide_whatNext();
					//trigger OS_pendSV(Switch Context/Restore)
					trigger_OS_PendSV();
				}
			}
			break;
		case SVC_TaskWaitingTime:
			MYRETOS_Update_Schaduler_table();
			break;
		}
}

//Thread Mode
void MYRTOS_OS_SVC_Set(SVC_ID ID){

	switch (ID){
	case SVC_ActivateTask:
		__asm volatile ("SVC #0x00");
		break;
	case SVC_TerminateTask:
		__asm volatile ("SVC #0x01");
		break;
	case SVC_TaskWaitingTime:
		__asm volatile ("SVC #0x02");
		break;
	case SVC_AquireMutex:
		__asm volatile ("SVC #0x03");
		break;

	case SVC_ReleaseMutex:
		__asm volatile ("SVC #0x04");
		break;
	}
}

void MYRTOS_ActivateTask	(Task_ref* Tref){
	Tref->TaskState = Waiting;
	MYRTOS_OS_SVC_Set(SVC_ActivateTask);

}

void MYRTOS_TerminateTask	(Task_ref* Tref){
	Tref->TaskState = Suspend;
	MYRTOS_OS_SVC_Set(SVC_TerminateTask);

}

void MYRTOS_TaskWait(unsigned int NoTICKS, Task_ref* SlefTref){
	SlefTref->TimingWaiting.Blocking = Enable;
	SlefTref->TimingWaiting.Ticks_Count = NoTICKS;
	//Task Should be blocked
	SlefTref->TaskState = Suspend;

	MYRTOS_OS_SVC_Set(SVC_TerminateTask);


}

void MYRTOS_Update_TaskWaitingTime(){
	for (int i= 0; i < OS_Control.NoOfActiveTacks; i++){
		if(OS_Control.OSTasks[i]->TaskState == Suspend){
			if (OS_Control.OSTasks[i]->TimingWaiting.Blocking == Enable){
				OS_Control.OSTasks[i]->TimingWaiting.Ticks_Count--;
				if(OS_Control.OSTasks[i]->TimingWaiting.Ticks_Count == 0){
					OS_Control.OSTasks[i]->TimingWaiting.Blocking = Disable;
					OS_Control.OSTasks[i]->TaskState = Waiting;
					MYRTOS_OS_SVC_Set(SVC_TaskWaitingTime);
				}
			}
		}
	}
}
void MYRTOS_STARTOS(){
	OS_Control.OSmodeID = OSRunning;
	//Set Default "Current Task == IDLETack"

	OS_Control.CurrentTask = &MYRTOS_IDLETack;

	//Avtivate IDLE TASK
	MYRTOS_ActivateTask(&MYRTOS_IDLETack);

	//Start Ticker
	Start_Ticker();//1ms

	OS_SET_PSP(OS_Control.CurrentTask->Current_PSP);

	//Swicth Thread Mode SP from MSP to PSP
	OS_SWITCH_SP_to_PSP;
	OS_SWITCH_to_unprivileged;
	MYRTOS_IDLETack.p_TaskEntry();
}

MYRTOS_errorID MYRTOS_AcquireMutex	(Mutex_ref* Mref,Task_ref* Tref){
	if (Mref->CurrenrtTUser ==NULL){
		Mref->CurrenrtTUser = Tref;
	}else{
		if(Mref->NextTUser == NULL)//not pending request by any other task
		{
			Mref->NextTUser = Tref;
			//move to Suspend state until be released
			Tref->TaskState = Suspend;
			//to be Suspend immediately
			MYRTOS_OS_SVC_Set(SVC_TerminateTask);
		}else//	there is a pending request by any other task
		{
			return MutexisReacedToMaxNumberOfUsers;
		}
	}
	return NOError;
}

MYRTOS_errorID MYRTOS_ReleaseMutex	(Mutex_ref* Mref){
	if (Mref->CurrenrtTUser !=NULL){
		Mref->CurrenrtTUser = Mref->NextTUser;
		 Mref->NextTUser = NULL;
		 Mref->CurrenrtTUser->TaskState = Waiting;
		 MYRTOS_OS_SVC_Set(SVC_ActivateTask);
	}
	return NOError;
}



/*
 * read.c
 * create by: Mohamed Aldreamly
 * 12 SEP 2026
 */
 
 #include "read.h"
//Callback function  
void (*readVal)();

int readPressureVal = 0;
int readWaitingPeride = 100 ;
int count = 0 ; 

void init(){
	//init sensor drivers
	readVal = STATE(readWaiting);
	//printf("init sensor drivers");
}

STATE_define(readWaiting){
	//state_Name
	readVal = STATE(readWaiting);
	//State_Action
	//printf("Start timer");
	//printf("waiting %dsec for reading from sensor" , readWaitingPeride);
	//turn on the timer ->delay will block the system
	//printf("Stop timer");
	//printf("reset timer");
	
	readVal = STATE(sensorReading);
}

STATE_define(sensorReading){
	//state_Name
	readVal = STATE(sensorReading);
	//State_Action
	readPressureVal = getPressureVal();
	//Event_Check
	while(readPressureVal==0){
		if (count==0){
			readPressureVal = 22; 
			count++;
		}else{
			readPressureVal = 12;
			count=0;
		}
		//readPressureVal = getPressureVal();
	}
	setPressureValToMain(readPressureVal);
	readVal = STATE(readWaiting);

}



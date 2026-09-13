/*
 * main_algo.c
 * create by: Mohamed Aldreamly
 * 12 SEP 2026
 */


#include "main_algo.h"

int pressureVal=0;
int threshlod = 20 ; 

void (*main_algo)();

void setPressureValToMain(int p);

void Main_Algo_init()
{
    main_algo = STATE(sencePressure);
}

STATE_define(sencePressure){
	
	main_algo = STATE(sencePressure);
}

STATE_define(alramActivation){
	alarmAction();
	main_algo = STATE(sencePressure);
}


void setPressureValToMain(int p){
	pressureVal = p ;
	//Event_Check
	(pressureVal<=threshlod) ? (main_algo = STATE(sencePressure)) :(main_algo = STATE(alramActivation));
	//printf("read------pressureVal=%d ------->main_algo\n", pressureVal);
}

/*
 * alarmMintor.c
 *
 *  Created on: 12 SEP 2026
 *      Author: Mohamed Aldreamly
 */
#include "alarmMintor.h"

void (*alarmMtr)();
void alarmAction();

int alarmPeriode =60; 

void MTR_init(){
	alarmMtr=STATE(alarmOFF);
}

STATE_define(alarmON){
	//printf("Start alarm");
	Set_Alarm_actuator(1);
	alarmMtr=STATE(waiting);
	//printf("Stop alarm");
	Set_Alarm_actuator(0);
	alarmMtr=STATE(alarmOFF);
}

STATE_define(alarmOFF){
	alarmMtr=STATE(alarmOFF);
}
STATE_define(waiting){
	//printf("waiting alarm for %d sec",alarmPeriode);
	Delay(alarmPeriode);
	alarmMtr=STATE(alarmON);
}

void alarmAction(){
	//action
	alarmMtr=STATE(alarmON);
}



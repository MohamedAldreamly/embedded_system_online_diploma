/*
 * alarmMintor.h
 *
 *  Created on: 12 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef  ALARM_MINTOR
#define ALARM_MINTOR

#include "main_algo.h"

//state
extern void (*alarmMtr)();

enum{
	alarmON,
	alarmOFF,
	waiting
};
//alarmMintor_state_id
//APIs
void alarmAction();
void MTR_init();

//declare func
STATE_define(alarmON);
STATE_define(alarmOFF);
STATE_define(waiting);

#endif /* ALARM_MINTOR */
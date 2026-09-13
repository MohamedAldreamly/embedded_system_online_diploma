/*
 * read.h
 * create by: Mohamed Aldreamly
 * 12 SEP 2026
 */

#ifndef	READ_H_
#define READ_H_

#include "main_algo.h"

extern void (*readVal)();

//STATE MACHINE
enum {
	readWaiting,
	sensorReading
};
//readState_state_id

//APLs 
void init();

//declaration
STATE_define(readWaiting);
STATE_define(sensorReading);

#endif/*READ_H_*/
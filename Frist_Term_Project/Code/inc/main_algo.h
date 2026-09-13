/*
 * main_algo.h
 *
 *  Created on: 12 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef  MAIN_ALGO_H_
#define MAIN_ALGO_H_

#include "state.h"
#include "driver.h"
#include "read.h"
#include "alarmMintor.h"

//state func
extern void (*main_algo)();

enum{
	sencePressure,
	alramActivation
};
//mainAlgo_state_id
//APIs
void setPressureValToMain(int p);
void Main_Algo_init();

//declare func
STATE_define(sencePressure);
STATE_define(alramActivation);


#endif /* MAIN_ALGO_H_ */



/*
 * US_sensor.h
 *
 *  Created on: 11 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef US_SENSOR_H_
#define US_SENSOR_H_

#include <stdio.h>
#include <stdlib.h>
#include "state.h"

enum{
	US_busy
}US_state_id;

//APIs
void US_init();

//declare func
STATE_define(US_busy);
//STATE Pointer to function
extern void (*US_state)();


#endif /* US_SENSOR_H_ */

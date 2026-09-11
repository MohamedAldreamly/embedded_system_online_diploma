/*
 * CA.h
 *
 *  Created on: 11 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef CA_H_
#define CA_H_

#include <stdio.h>
#include <stdlib.h>
#include "state.h"
extern void (*CA_state)();

//define state
enum{
	CA_waiting,
	CA_driving
}CA_State_id;

//declare status function CA
STATE_define(CA_waiting);
STATE_define(CA_driving);

#endif /* CA_H_ */

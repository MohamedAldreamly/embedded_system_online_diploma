/*
 * state.h
 *
 *  Created on: 11 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef STATE_H_
#define STATE_H_

#define STATE_define(_stateFUN_) void ST_##_stateFUN_()
#define STATE(_stateFUN_) ST_##_stateFUN_

//States Connection
void US_Set_distance(int d);
void DC_motor(int s);


#endif /* STATE_H_ */

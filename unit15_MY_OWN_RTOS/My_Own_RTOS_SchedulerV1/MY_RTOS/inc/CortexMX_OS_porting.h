/*
 * CortexMX_OS_porting.h
 *
 *	Created on	: OCT, 9 2026
 *  Author: Mohamed aldremly
 */

#ifndef CORTEXMX_OS_PORTING_H_
#define CORTEXMX_OS_PORTING_H_

#include "ARMCM3.h"

extern int _estack;
extern int _eheap;

#define MainStackSize 3072

#define OS_SET_PSP(add)			__asm volatile("mov r0, %0 \n\t msr PSP,r0" ::"r"(add))
#define OS_GET_PSP(add)			__asm volatile("mrs r0,PSP \n\t mov %0,r0" ::"=r"(add))

#define OS_SWITCH_SP_to_PSP		__asm volatile("mrs r0,CONTROL \n\t mov r1,#0x02 \n\t orr r0,r0,r1 \n\t msr CONTROL,r0")
#define OS_SWITCH_SP_to_MSP		__asm volatile("mrs r0,CONTROL \n\t mov r1,#0x05 \n\t and r0,r0,r1 \n\t msr CONTROL,r0")

//clear bit 0 in CONTROL REG
#define OS_SWITCH_to_privileged		__asm(	"mrs r3 , CONTROL	\n\t"	\
											"lsr r3,r3,#0x1		\n\t"	\
											"lsl r3,r3,#0x1		\n\t"	\
											"msr CONTROL,r3		\n\t");
//set bit 0 in CONTROL REG
#define OS_SWITCH_to_unprivileged	__asm(	"mrs r3 , CONTROL	\n\t"	\
											"orr r3,r3,#1		\n\t"	\
											"msr CONTROL,r3	\n\t");



void HW_init();



#endif /* CORTEXMX_OS_PORTING_H_ */


/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    EXTI.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 15 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 

#ifndef EXTI_H_
#define EXTI_H_

#include "BIT_MATH.h"
#include "STD_TYPES.h"

#include "AFIO.h"
#include "NVIC.h"
/*
1 - Hardware interrupt selection
		1-(EXTI_IMR)
		2-(EXTI_RTSR and EXTI_FTSR)
		3-NVIC IRQ channel 
		
2 - Hardware event selection
		1- (EXTI_EMR)
		2-(EXTI_RTSR and EXTI_FTSR)

3 - Software interrupt/event selection
		1-(EXTI_IMR, EXTI_EMR)
		2-(EXTI_SWIER)
		
4 - External interrupt/event line mapping(AFIO)
*/

/*		CallBack Functions Declerations		*/
typedef void (*EXTI_Callback)(void);

/*		Configuration		*/

#define Interrupt_Source	interruptByHardware
//#define EXTI_Mode			interrupt
//#define Tigger_Sence		Rising_Edge


/* interruptSource state */
#define interruptByHardware 	0
#define interruptBySoftware 	1
#define interruptByBoth 		2

/*EXTI Mode state  */
#define interrupt 			0
#define event 				1
#define interrupt_event		2

/*Tigger_Sence state*/
#define Rising_Edge 			0 
#define Falling_Edge 			1 
#define Rising_Falling_Edge 	2 


typedef enum {
	EXTI_DONE,
	EXTI_ERROR
}EXTI_STATE_t; 

typedef struct{
	volatile uint32 IMR;
	volatile uint32 EMR;
	volatile uint32 RTSR;
	volatile uint32 FTSR;
	volatile uint32 SWIER;
	volatile uint32 PR;
}EXTI_t;



#define EXTI_BASE 0x40010400
#define EXTI ((volatile EXTI_t*)EXTI_BASE)



/*		define EXTI LINES		*/ 

#define EXTI_LINE_0 		0
#define EXTI_LINE_1 		1
#define EXTI_LINE_2 		2
#define EXTI_LINE_3 		3
#define EXTI_LINE_4 		4
#define EXTI_LINE_5 		5
#define EXTI_LINE_6 		6
#define EXTI_LINE_7 		7
#define EXTI_LINE_8 		8
#define EXTI_LINE_9 		9
#define EXTI_LINE_10 		10
#define EXTI_LINE_11 		11
#define EXTI_LINE_12 		12
#define EXTI_LINE_13 		13
#define EXTI_LINE_14 		14
#define EXTI_LINE_15 		15

#define EXTI0IRQn     6
#define EXTI1IRQn     7
#define EXTI2IRQn     8
#define EXTI3IRQn     9
#define EXTI4IRQn     10

#define EXTI5IRQn     23
#define EXTI6IRQn     23
#define EXTI7IRQn     23
#define EXTI8IRQn     23
#define EXTI9IRQn     23

#define EXTI10IRQn    40
#define EXTI11IRQn    40
#define EXTI12IRQn    40
#define EXTI13IRQn    40
#define EXTI14IRQn    40
#define EXTI15IRQn    40

EXTI_STATE_t EXTI_enumInit(uint8 Copy_u8EXTILine, uint8 Copy_u8Port, uint8 Copy_u8EXTISenseMode , EXTI_Callback Copy_EXTI_CallBack) ;                                 
EXTI_STATE_t EXTI_enumSetSignalLatch (uint8 Copy_u8EXTILine , uint8 Copy_u8EXTISenseMode);
EXTI_STATE_t EXTI_enumEnableEXTI     (uint8 Copy_u8EXTILine                          );
EXTI_STATE_t EXTI_enumDisableEXTI    (uint8 Copy_u8EXTILine                          );

EXTI_STATE_t EXTI_enumSetCallBack(uint8 Copy_u8EXTILine, EXTI_Callback Copy_EXTI_CallBack);


#endif /*	EXTI_H_	*/
 
 

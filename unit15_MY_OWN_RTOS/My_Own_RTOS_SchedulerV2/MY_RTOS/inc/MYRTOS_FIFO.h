/*
 * MYRTOS_FIFO.c
 *
 *	Created on	: OCT, 9 2026
 *  Author: Mohamed aldremly
 */

#ifndef MYRTOS_FIFO
#define MYRTOS_FIFO

#include <stdint.h>

#include "Scheduler.h"

//Configration
//select the element type(uint8_t,uint32_t,...)
#define element_type	Task_ref*


typedef struct {
	element_type* head;
	element_type* base;
	element_type* tail;
	unsigned int count;
	unsigned int length;
}FIFO_Buf_t;

typedef enum {
	FIFO_no_error,
	FIFO_full,
	FIFO_empty,
	FIFO_null
}FIFO_Buf_Status;

FIFO_Buf_Status FIFO_init		(FIFO_Buf_t* fifo,element_type* buf, unsigned int length);
FIFO_Buf_Status FIFO_enqueue	(FIFO_Buf_t* fifo,element_type item);
FIFO_Buf_Status FIFO_dequeue	(FIFO_Buf_t* fifo,element_type* item);
FIFO_Buf_Status FIFO_IS_FULL	(FIFO_Buf_t* fifo);
void FIFO_print 				(FIFO_Buf_t* fifo);

#endif /* MYRTOS_FIFO */

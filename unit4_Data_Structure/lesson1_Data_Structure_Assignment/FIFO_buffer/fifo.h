/*
 * fifo.h
 *
 *  Created on : 09 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <stdio.h>
#include <stdint.h>

//Configration
//select the element type(uint8_t,uint32_t,...)
#define element_type	uint8_t

//create buffer by element_typetype
#define width1 5
element_type uart_buffer[width1];

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




#endif /* FIFO_H_ */

/*
 * main.c
 *
 *  Created on : 09 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#include "lifo.h"


unsigned int buffer1 [5]; // 5*4 =20 byte

int main (){

	unsigned int i, temp;
	LIFO_Buf_t uart_lifo ,I2C_lifo ;
	//static allocation
	LIFO_init(&uart_lifo , buffer1 , 5);
	//dynamic allcotion
	unsigned int* buffer2 = (unsigned int*) malloc(5 * sizeof (unsigned int));
	LIFO_init(&I2C_lifo , buffer2 , 5);

	for(i=0; i<5;i++){
		if(LIFO_Add_item(&uart_lifo,i) == LIFO_no_error)
		printf("UART_LIFO add : %d\n" , i);
	}

	for(i=0; i<5;i++){
		if (LIFO_get_item(&uart_lifo,&temp) == LIFO_no_error)
		printf("UART_LIFO get : %d\n" , temp);
		}
}

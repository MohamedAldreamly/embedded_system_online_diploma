/*
	create on: 2 SEP 2026
	aouther : Moahmed Aldreamly
*/

#include "uart.h"


// UART register
#define UARTODR 	*((volatile unsigned int* const) ((unsigned int*)0x101f1000))

void Uart_Send_string (unsigned char *P_tx_string){
	while (*P_tx_string != '\0'){
		UARTODR = (unsigned int)(*P_tx_string);
		P_tx_string++; // next char
	}	
}
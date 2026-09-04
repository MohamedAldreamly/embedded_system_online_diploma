/*
	create on: 2 SEP 2026
	aouther : Moahmed Aldreamly
*/

#include "uart.h"

unsigned char string_buffer [100] = "learn-in-depth:Mohamed-Aldreamly";
unsigned char const string_buffer2 [100] = "learn-in-depth:Mohamed-Aldreamly";

void main (void){
	Uart_Send_string(string_buffer);
}
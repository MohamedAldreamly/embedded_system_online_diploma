/*
 * main.c
 *
 *  Created on: 11 SEP 2026
 *      Author: Mohamed Aldreamly
 */

#include "CA.h"
void setup (){

	//init all the drivers
	//init IRQ
	//init HAL US_Driver DC_Driver
	//init BLOCK
	//set States pointer for each Block
	CA_state = STATE(CA_waiting);
}
int main()
{
	volatile int i;
	setup();
	while(1)
	{
		for(i=0;i<1000;i++)
			CA_state();
	}
}

/*
 * main.c
 * create by: Mohamed Aldreamly
 * 12 SEP 2026
 */

#include <stdint.h>
#include "driver.h"

#include "main_algo.h"

int main (void){
	GPIO_INITIALIZATION();
	MTR_init();
	Main_Algo_init();
	init();
	while (1)
	{
		readVal();
		main_algo();
		alarmMtr();
	
	}

}


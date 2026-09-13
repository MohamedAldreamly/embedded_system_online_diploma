/* startup.c 
create by: moahmed aldreamly
*/

#include <stdio.h>

extern int main(void);

void Reset_Handler();

extern unsigned int _stack_ptr;

void Default_Handler(){
	Reset_Handler();
}

void NMI_Handler() __attribute__ ((weak,alias("Default_Handler")));
void H_fault_Handler() __attribute__ ((weak,alias("Default_Handler")));


void (* g_p_fn_Vectors[])() __attribute__((section(".vectors"))) ={
	(void (*)()) &_stack_ptr,
	&Reset_Handler,
	&NMI_Handler,
	&H_fault_Handler
	
};

extern unsigned int _E_text;
extern unsigned int _S_DATA;
extern unsigned int _E_DATA;
extern unsigned int _S_BSS;
extern unsigned int _E_BSS;


void Reset_Handler(){
	//copy data section from flash to ram
	unsigned int DATA_size = (unsigned char *)&_E_DATA - (unsigned char *)&_S_DATA ;
	unsigned char* P_src = (unsigned char*)&_E_text;
	unsigned char* P_det = (unsigned char*)&_S_DATA;
	for (int i= 0; DATA_size > i; i ++ ){
		*((unsigned char*)P_det++) = *((unsigned char*)P_src++); 
	}
	
	unsigned int bss_size = (unsigned char *)&_E_BSS - (unsigned char *)&_S_BSS ;
	P_det = (unsigned char*)&_S_BSS;
	for (int i= 0; bss_size > i; i ++ ){
		*((unsigned char*)P_det++) = (unsigned char) 0 ; 
	}
	// jimp main 
	main();	
}

/*
	create by : mohamed aldreamly 
	1 SEP 2026
*/


#include "Platform_Types.h"

// register address 
#define RCC_BASE 					0x40021000
#define RCC_APB2ENR					*(vuint32* )(RCC_BASE + 0x18)

#define GPIO_BASE 					0x40010800
#define GPIOA_CRH					*(vuint32* )(GPIO_BASE + 0x04)
#define GPOI_ODR					*( vuint32* )(GPIO_BASE + 0x0C)

// bit filed macre 

#define RCC_IOPEN 					(1UL<<2)
#define GPIOA13 					(1UL<<13)

typedef union {
	 vuint32 all_filed ; 
	struct{
		vuint32 reserved:13; 
		vuint32 P_13:1; 
	}Pin; 
}R_ODR_t; 

volatile R_ODR_t* R_ODR = (volatile R_ODR_t*)(GPIO_BASE + 0x0C);
unsigned char g_variables[3]={1,2,3};
unsigned char const const_variables[3]={1,2,3};
unsigned char bss_var[3];

int main(){
	RCC_APB2ENR |=RCC_IOPEN; 
	GPIOA_CRH &= 0xff0fffff;
	GPIOA_CRH |= 0x00200000; 
	
	while (1){
		R_ODR->Pin.P_13 = 1; 
		for (int i=0; i<=5000; i++);
		R_ODR->Pin.P_13 = 0; 
		for (int i=0; i<=5000; i++);

	}

}


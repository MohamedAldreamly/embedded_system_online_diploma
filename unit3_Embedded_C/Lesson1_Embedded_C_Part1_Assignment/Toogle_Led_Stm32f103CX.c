/*
	create by : mohamed aldreamly 
	1 SEP 2026
*\

typedef volatile unsigned 			int vuint_t;

#include <stdint.h>

// register address 
#define RCC_BASE 					0x40021000
#define RCC_APB2ENR					*(vuint_t* )(RCC_BASE + 0x18)

#define GPIO_BASE 					0x40010800
#define GPIOA_CRH					*(vuint_t* )(GPIO_BASE + 0x04)
#define GPOI_ODR					*(vuint_t* )(GPIO_BASE + 0x0C)

// bit filed macre 

#define RCC_IOPEN 					(1UL<<2)
#define GPIOA13 					(1UL<<13)

typedef union {
	vuint all_filed ; 
	struct{
		vuint reserved:13; 
		vuint P_13:1; 
	}Pin; 
}R_ODR_t; 

volatile R_ODR_t* R_ODR = (volatile R_ODR_t*)(GPIO_BASE + 0x0C);

int main(){
	RCC_APB2ENR |=RCC_IOPEN; 
	GPIOA_CRH &= 0xff0fffff;
	GPIOA_CRH |= 0x00200000; 
	
	while (){
		R_ODR->Pin.P_13 = 1; 
		for (int i=0; i<=5000; i++);
		R_ODR->Pin.P_13 = 0; 
		for (int i=0; i<=5000; i++);

	}

}


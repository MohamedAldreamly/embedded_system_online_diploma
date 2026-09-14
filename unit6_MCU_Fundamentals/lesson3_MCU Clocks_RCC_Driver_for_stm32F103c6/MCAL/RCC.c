/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    RCC.c   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 14 2026
 *  Layer   : MCAL
 *  Target  : STM32F103 / Cortex-M3
 *
 */ 
 
#include "RCC.h"


RCC_STATE_t RCC_enumiInit(){
	
	//HPRE: AHB prescaler
	CLR_FIELD( RCC->RCC_CFGR , RCC_HPRE_POSITION, 4 );
	SET_FIELD( RCC->RCC_CFGR , RCC_HPRE_POSITION , RCC_AHB_PRESCALER);
	
	//PPRE1: APB1 prescaler
	CLR_FIELD( RCC->RCC_CFGR , RCC_PPRES1_POSITION, 3 );
	SET_FIELD( RCC->RCC_CFGR , RCC_PPRES1_POSITION , RCC_APB1_PRESCALER );

	//PPRE2: APB2 prescaler
	CLR_FIELD( RCC->RCC_CFGR , RCC_PPRES2_POSITION, 3 );
	SET_FIELD( RCC->RCC_CFGR , RCC_PPRES2_POSITION , RCC_APB2_PRESCALER );
	
	#if  RCC_CLOCK_TYPE == RCC_HSI
		// Enable HSI
		SET_BIT( RCC->RCC_CR , RCC_HSION );
		// Wait Until HSI Is Ready ( use timeout ) 
		//while ( GET_BIT( RCC->RCC_CR , RCC_HSIRDY ) == 0 );

		// Select HSI As System Clock
		CLR_FIELD( RCC->RCC_CFGR , RCC_SW_POSITION, 2 );
		SET_FIELD(RCC->RCC_CFGR, RCC_SW_POSITION, RCC_SW_HSI);
		//Check is HSI As System Clock
		//while (GET_FIELD(RCC->RCC_CFGR, RCC_SWS_POSITION, 2) != RCC_SW_HSI);
	#elif RCC_CLOCK_TYPE == RCC_HSE
		// Enable HSE
		SET_BIT( RCC->RCC_CR , RCC_HSEON );
		// Wait Until HSE Is Ready 
		//while ( GET_BIT( RCC->RCC_CR , RCC_HSERDY ) == 0 );
		// Select HSE As System Clock
		CLR_FIELD( RCC->RCC_CFGR , RCC_SW_POSITION, 2 );
		SET_FIELD(RCC->RCC_CFGR, RCC_SW_POSITION, RCC_SW_HSE);
		//Check is HSE As System Clock
		//while (GET_FIELD(RCC->RCC_CFGR, RCC_SWS_POSITION, 2) != RCC_SW_HSE);
	#elif RCC_CLOCK_TYPE == RCC_PLL	

		/* Configure PLL source  */
		#if RCC_PLL_SOURCE == RCC_PLL_HSI_DIVIDED_BY_2
			// Select PLL HSI/2 As System Clock
	    	CLR_BIT(RCC->RCC_CFGR, RCC_PLLSRC_POSITION);
		#elif RCC_PLL_SOURCE == RCC_PLL_HSE
			// Select PLL HSE As System Clock
	    	SET_BIT(RCC->RCC_CFGR, RCC_PLLSRC_POSITION);
	        CLR_BIT(RCC->RCC_CFGR, RCC_PLLXTPRE_POSITION);
		#elif RCC_PLL_SOURCE == RCC_PLL_HSE_DIVIDED_BY_2
			// Select PLL HSE/2 As System Clock
	        SET_BIT(RCC->RCC_CFGR, RCC_PLLSRC_POSITION);
	        SET_BIT(RCC->RCC_CFGR, RCC_PLLXTPRE_POSITION);
		#endif	
		
		//PLL multiplication factor
		CLR_FIELD( RCC->RCC_CFGR , RCC_PLLMUL_POSITION, 4 );
		SET_FIELD(RCC->RCC_CFGR, RCC_PLLMUL_POSITION, RCC_PLL_MUL);
		// Enable PLL
		SET_BIT( RCC->RCC_CR , RCC_PLLON );
		// Wait Until PLL Is Ready 
		//while ( GET_BIT( RCC->RCC_CR , RCC_PLLRDY ) == 0 );
		// Select PLL As System Clock
		CLR_FIELD( RCC->RCC_CFGR , RCC_SW_POSITION, 2 );
		SET_FIELD(RCC->RCC_CFGR, RCC_SW_POSITION, RCC_SW_PLL);
		//Check is PLL As System Clock
		//while (GET_FIELD(RCC->RCC_CFGR, RCC_SWS_POSITION, 2) != RCC_SW_PLL);
	#endif
	
	return RCC_DONE;
}

RCC_STATE_t RCC_enumEnablePeripheralClock	( uint8 Copy_u8BusId , uint8 Copy_u8PeripheralId ){
	switch (Copy_u8BusId){
		case AHB_BUS:
			SET_BIT(RCC->RCC_AHBENR,Copy_u8PeripheralId );

			break;
		case APB1_BUS:
			SET_BIT(RCC->RCC_APB1ENR,Copy_u8PeripheralId );

			break;
		case APB2_BUS:
			SET_BIT(RCC->RCC_APB2ENR,Copy_u8PeripheralId );
			break;	
		default:
		/* Wrong Bus Id */
		break;	
	}
	return RCC_DONE;
}

RCC_STATE_t RCC_enumDisablePeripheralClock( uint8 Copy_u8BusId , uint8 Copy_u8PeripheralId )
{
	switch (Copy_u8BusId)
	{
	case AHB_BUS:
		CLR_BIT( RCC->RCC_AHBENR , Copy_u8PeripheralId );
		break;
	case APB1_BUS:
		CLR_BIT( RCC->RCC_APB1ENR , Copy_u8PeripheralId );
		break;
	case APB2_BUS:
		CLR_BIT( RCC->RCC_APB2ENR , Copy_u8PeripheralId );
		break;
	default:
		/* Wrong Bus Id */
		break;
	}
	return RCC_DONE;
}

RCC_STATE_t RCC_enumResetPeripheralClock	(uint8 Copy_u8BusId, uint8 Copy_u8PeripheralId){
	
	switch (Copy_u8BusId){
		case APB1_BUS:
			SET_BIT(RCC->RCC_APB1RSTR,Copy_u8PeripheralId );
			break;
		case APB2_BUS:
			SET_BIT(RCC->RCC_APB2RSTR,Copy_u8PeripheralId );
			break;	
		default:
		/* Wrong Bus Id */
		break;	
	}
	return RCC_DONE;
}

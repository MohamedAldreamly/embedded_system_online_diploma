/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    BIT_MATH.h   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 14 2026
 *  Layer   : LIB
 *
 */ 
 
#ifndef BIT_MATH_H_
#define BIT_MATH_H_

#define SET_BIT(REG, POS) ((REG) |= (1UL << (POS)))
#define CLR_BIT(REG, POS) ((REG) &= ~(1UL << (POS)))
#define TOG_BIT(REG, POS) ((REG) ^= (1UL << (POS)))
#define GET_BIT(REG, POS) (((REG) >> (POS)) & 1UL)

/*===================================================*/

#define SET_2BIT_INDEX(REG, INDEX, VALUE) ((REG) |= (((VALUE) & 0x3U) << ((INDEX) * 2U)))

#define CLR_2BIT_INDEX(REG, INDEX) ((REG) &= ~(0x3U << ((INDEX) * 2U)))

#define GET_2BIT_INDEX(REG, INDEX) (((REG) >> ((INDEX) * 2U)) & 0x3U)

/*===================================================*/

#define SET_FIELD(REG, POS, VALUE) ((REG) |= ((VALUE) << (POS)))
			 
#define CLR_FIELD(REG, POS, WIDTH) ((REG) &= ~((((1U << (WIDTH)) - 1U)) << (POS)))

#define GET_FIELD(REG, POS, WIDTH) (((REG) >> (POS)) & ((1U << (WIDTH)) - 1U))
	
	
#endif //BIT_MATH_H_
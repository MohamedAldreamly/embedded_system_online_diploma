/*
 *<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<    STD_TYPES.h    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *  Author  : Mohamed Aldreamly
 *  Date    : MAY, 7 2026
 *  Vertion : V02
 *	Update	: SEP, 14 2026
 *  Layer   : LIB
 *
 */  


#ifndef STD_TYPES_H_
#define STD_TYPES_H_

typedef unsigned char           uint8 ;
typedef unsigned short int      uint16;
typedef unsigned long int       uint32;
typedef unsigned long long int  uint64;
typedef signed char	            sint8 ;
typedef signed short int        sint16;
typedef signed long int         sint32;
typedef signed long long int    sint64;
typedef float                   float32;
typedef double                  float64;

typedef enum
	{
		fasle,
		true,
	}bool;

#define NULL 0

/* Func States  */
#define IDLE 			0
#define BUSY			1

/* My States  */
#define OK           	0
#define NOK          	1
#define NULL_POINTER 	2
#define BUSY_STATE	 	3
#define TIMEOUT_STATE   4

	
#endif // STD_TYPES_H_ 
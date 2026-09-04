/*
 * Platform_Types.h
 *
 *  Created on: AUG 28 2026
 *      Author: Mohamed Aldreamly
 */

#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#ifndef _Bool
#define _Bool unsigned char
#endif

#define CPU_TYPE 		CPU_TYPE_32
#define CPU_BIT_ORDER	MSB_FRIST
#define CPU_BYTE_ORDER	HIGH_BYTE_FRIST

#ifndef FALSE
#define FALSE (boolean)false
#endif

#ifndef TRUE
#define TRUE 			(boolean)true
#endif

typedef _Bool 			boolean;
typedef int8_t 			sint8;
typedef uint8_t 		uint8;
typedef char 			char_t;
typedef int16_t 		sint16;
typedef uint16_t 		uint16 ;
typedef int32_t 		sint32;
typedef uint32_t 		uint32;
typedef int64_t 		sint64;
typedef uint64_t 		uint64;

typedef volatile int8_t 		vsint8;
typedef volatile uint8_t 		vuint8;

typedef volatile int16_t 		vsint16;
typedef volatile uint16_t 		vuint16;

typedef volatile int32_t 		vsint32;
typedef volatile uint32_t 		vuint32;

typedef volatile int64_t 		vsint64;
typedef volatile uint64_t 		vuint64;


#endif /* _PLATFORM_TYPES_H_ */

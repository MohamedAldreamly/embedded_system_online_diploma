/*
 * Platform_Types.h
 *
 * Created on: AUG 28 2026
 * Author: Mohamed Aldreamly
 */

#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_


/*========================================================
 * CPU Type
 *========================================================*/

#define CPU_TYPE_8      8U
#define CPU_TYPE_16     16U
#define CPU_TYPE_32     32U
#define CPU_TYPE_64     64U

#define CPU_TYPE        CPU_TYPE_32


/*========================================================
 * CPU Bit Order
 *========================================================*/

#define LSB_FIRST       0U
#define MSB_FIRST       1U

#define CPU_BIT_ORDER   MSB_FIRST


/*========================================================
 * CPU Byte Order
 *========================================================*/

#define LOW_BYTE_FIRST   0U
#define HIGH_BYTE_FIRST  1U

#define CPU_BYTE_ORDER   HIGH_BYTE_FIRST


/*========================================================
 * Basic Data Types
 *========================================================*/

typedef signed char             sint8;
typedef unsigned char           uint8;

typedef signed short int        sint16;
typedef unsigned short int      uint16;

typedef signed int              sint32;
typedef unsigned int            uint32;

typedef signed long long        sint64;
typedef unsigned long long      uint64;

typedef float                   float32;
typedef double                  float64;


/*========================================================
 * Boolean Type
 *========================================================*/

typedef uint8 boolean;

#ifndef TRUE
#define TRUE                    ((boolean)1U)
#endif

#ifndef FALSE
#define FALSE                   ((boolean)0U)
#endif


/*========================================================
 * Volatile Data Types
 *========================================================*/

typedef volatile sint8          vsint8;
typedef volatile uint8          vuint8;

typedef volatile sint16         vsint16;
typedef volatile uint16         vuint16;

typedef volatile sint32         vsint32;
typedef volatile uint32         vuint32;

typedef volatile sint64         vsint64;
typedef volatile uint64         vuint64;

typedef volatile float32        vfloat32;
typedef volatile float64        vfloat64;


#endif /* PLATFORM_TYPES_H_ */

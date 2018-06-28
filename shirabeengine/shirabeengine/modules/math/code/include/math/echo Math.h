#ifndef __SHIRABE__MATH_H__
#define __SHIRABE__MATH_H__

#include <xmemory>

/* COMMON CONSTANTS */
#define MATH_PI 3.14159265359f
#define MATH_e  2.71828182845f

/* CONVERSION CONSTANTS */

/* Decimal System Conversion*/
#define MATH_TERA   1000000000.0f
#define MATH_MEGA   1000000.0f
#define MATH_KILO   1000.0f
#define MATH_DECI   0.1f
#define MATH_CENTI  0.01f
#define MATH_MILLI  0.001f
#define MATH_MICRO  0.000001f
#define MATH_NANO   0.000000001f

/* Degree <-> Radians */
#define MATH_RADDEG 57.29577951307855f
#define MATH_DEGRAD 0.0174532925199444f

/* CONVERSION METHOD MACROS */

/* Degree <-> Radians */
#define MATH_RADTODEG(rad) (rad * MATH_RADDEG)
#define MATH_DEGTORAD(deg) (deg * MATH_DEGRAD)

#include "Vector.h"
#include "Matrix.h"

#endif
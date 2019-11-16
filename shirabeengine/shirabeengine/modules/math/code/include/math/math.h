#ifndef __SHIRABE__MATH_H__
#define __SHIRABE__MATH_H__

#include <memory>

/* COMMON CONSTANTS */
static constexpr float const MATH_PI = 3.14159265359f;
static constexpr float const MATH_e  = 2.71828182845f;

/* CONVERSION CONSTANTS */

/* Decimal System Conversion*/
static constexpr float const MATH_TERA   = 1000000000.0f;
static constexpr float const MATH_MEGA   = 1000000.0f   ;
static constexpr float const MATH_KILO   = 1000.0f      ;
static constexpr float const MATH_DECI   = 0.1f         ;
static constexpr float const MATH_CENTI  = 0.01f        ;
static constexpr float const MATH_MILLI  = 0.001f       ;
static constexpr float const MATH_MICRO  = 0.000001f    ;
static constexpr float const MATH_NANO   = 0.000000001f ;

/* Degree <-> Radians */
static constexpr float const MATH_RADDEG = 57.29577951307855f ;
static constexpr float const MATH_DEGRAD = 0.0174532925199444f;

/* CONVERSION METHOD MACROS */

/* Degree <-> Radians */
#define DMATH_RADTODEG(rad) (rad * DMATH_RADDEG)
#define DMATH_DEGTORAD(deg) (deg * DMATH_DEGRAD)

#include "math/vector.h"
#include "math/matrix.h"

#endif

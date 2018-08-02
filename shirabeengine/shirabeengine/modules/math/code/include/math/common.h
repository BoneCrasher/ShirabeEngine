#ifndef __SHIRABE_MATHCOMMON_H__
#define __SHIRABE_MATHCOMMON_H__

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace Engine
{
    namespace Math
    {

        static double deg_to_rad(double const &aDeg)
        {
            return ((aDeg / 180.0) *  M_PI);
		}

        static double rad_to_deg(double const &aRad)
        {
            return ((aRad / M_PI) * 180.0);
		}

        static float deg_to_rad(float const &aDeg)
        {
            return ((aDeg / 180.0f) *  (float) M_PI);
		}

        static float rad_to_deg(float const &aRad)
        {
            return ((aRad / (float) M_PI) * 180.0f);
		}
	}
}

#endif

#ifndef __SHIRABE_MATHCOMMON_H__
#define __SHIRABE_MATHCOMMON_H__

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace Engine {
	namespace Math {		

		inline static double deg_to_rad(const double& deg) {
			return ((deg / 180.0) *  M_PI);
		}

		inline static double rad_to_deg(const double& rad) {
			return ((rad / M_PI) * 180.0);
		}

		inline static float deg_to_rad(const float& deg) {
			return ((deg / 180.0f) *  (float) M_PI);
		}

		inline static float rad_to_deg(const float& rad) {
			return ((rad / (float) M_PI) * 180.0f);
		}
	}
}

#endif
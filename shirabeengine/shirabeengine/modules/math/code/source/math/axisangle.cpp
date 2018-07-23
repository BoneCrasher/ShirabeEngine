#include "math/axisangle.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace Engine
{
    namespace Math
    {
        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::CAxisAngle()
            : CVector4D_t({ 0, 0, 0, 0 })
		{}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::CAxisAngle(
                CVector3D_t const &aAxis,
                value_type  const &aPhi)
            : CVector4D_t({ aAxis.x(), aAxis.y(), aAxis.z(), aPhi })
		{}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::CAxisAngle(CAxisAngle const& aOther)
            : CVector4D_t({ aOther })
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::CAxisAngle(
                value_type const& aX,
                value_type const& aY,
                value_type const& aZ,
                value_type const& aPhi)
            : CVector4D_t({ aX, aY, aZ, aPhi })
		{}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::~CAxisAngle()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t const CAxisAngle::axis() const
        {
            return CVector3D_t({ x(), y(), z() });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        const CAxisAngle::value_type& CAxisAngle::phi() const
        {
            return mField[3];
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t const CAxisAngle::axis(
                value_type const& aX,
                value_type const& aY,
                value_type const& aZ)
        {
            this->x(aX);
            this->y(aY);
            this->z(aZ);
            return this->axis(); // Hope for copy elision to work... otherwise switch to move semantics.
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::value_type const &CAxisAngle::phi(value_type const &aPhi)
        {
            return (mField[3] = aPhi);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t operator*(
                CAxisAngle  const &aAxisAngle,
                CVector3D_t const &aVector)
        {
            using Value_t = CAxisAngle::value_type;

			// Rodriguez rotation formula
			// v_rot = (cos(phi)*v + sin(phi)*cross(e, v) + (1 - cos(phi))*(dot(e, v)*e)
            Value_t     cos_phi   = cosf(aAxisAngle.phi());
            Value_t     sin_phi   = sinf(aAxisAngle.phi());
            CVector3D_t e         = aAxisAngle.axis();
            Value_t     dot_e_v   = dot(e, aVector);
            CVector3D_t cross_e_v = cross(e, aVector);

            return ((cos_phi * aVector) + (sin_phi * cross_e_v) + ((1 - cos_phi) * (dot_e_v * e)));
		}
        //<-----------------------------------------------------------------------------
	}
}

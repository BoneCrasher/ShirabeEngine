#include "math/axisangle.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace engine
{
    namespace math
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
                ValueType_t const &aPhi)
            : CVector4D_t({ aAxis.x(), aAxis.y(), aAxis.z(), aPhi })
		{}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::CAxisAngle(CAxisAngle const &aOther)
            : CVector4D_t({ aOther })
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CAxisAngle::CAxisAngle(
                ValueType_t const &aX,
                ValueType_t const &aY,
                ValueType_t const &aZ,
                ValueType_t const &aPhi)
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
        const CAxisAngle::ValueType_t&CAxisAngle::phi() const
        {
            return mField[3];
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CVector3D_t const CAxisAngle::axis(
                ValueType_t const &aX,
                ValueType_t const &aY,
                ValueType_t const &aZ)
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
        CAxisAngle::ValueType_t const &CAxisAngle::phi(ValueType_t const &aPhi)
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
            using Value_t = CAxisAngle::ValueType_t;

			// Rodriguez rotation formula
			// v_rot = (cos(phi)*v + sin(phi)*cross(e, v) + (1 - cos(phi))*(dot(e, v)*e)
            Value_t     const cos_phi   = cosf(aAxisAngle.phi());
            Value_t     const sin_phi   = sinf(aAxisAngle.phi());
            CVector3D_t const e         = aAxisAngle.axis();
            Value_t     const dot_e_v   = dot(e, aVector);
            CVector3D_t const cross_e_v = cross(e, aVector);

            CVector3D_t const vec = ((cos_phi * aVector) + (sin_phi * cross_e_v) + ((1 - cos_phi) * (dot_e_v * e)));

            return vec;
		}
        //<-----------------------------------------------------------------------------
	}
}

#ifndef __SHIRABE_CAxisAngle_H__
#define __SHIRABE_CAxisAngle_H__

#include "math/vector.h"

namespace Engine
{
  namespace Math
  {

    class CAxisAngle
      : public CVector4D_t
    {
    public:
        CAxisAngle();
        CAxisAngle(CAxisAngle const&aOther);
        CAxisAngle(
                CVector3D_t const &aAxis,
                value_type const &aPhi);
        CAxisAngle(
                value_type const& aX,
                value_type const& aY,
                value_type const& aZ,
                value_type const& aPhi);

        ~CAxisAngle();

        CVector3D_t const axis() const;

        value_type const &phi() const;

        CVector3D_t const axis(
                value_type const& aX,
                value_type const& aY,
                value_type const& aZ);

        value_type const &phi(value_type const &aPhi);
    };

    CVector3D_t operator*(
            CAxisAngle  const &aAxisAngle,
            CVector3D_t const &aVector);

  }
}
#endif

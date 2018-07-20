#ifndef __SHIRABE_CAxisAngle_H__
#define __SHIRABE_CAxisAngle_H__

#include "math/vector.h"

namespace Engine
{
  namespace Math
  {

    class CAxisAngle
      : public Vector4D
    {
    public:
        CAxisAngle();
        CAxisAngle(CAxisAngle const&aOther);
        CAxisAngle(
                Vector3D   const &aAxis,
                value_type const &aPhi);
        CAxisAngle(
                value_type const& aX,
                value_type const& aY,
                value_type const& aZ,
                value_type const& aPhi);

        ~CAxisAngle();

        Vector3D const axis() const;

        value_type const &phi() const;

        Vector3D const axis(
                value_type const& aX,
                value_type const& aY,
                value_type const& aZ);

        value_type const &phi(value_type const &aPhi);
    };

    Vector3D operator*(
            CAxisAngle const &aAxisAngle,
            Vector3D   const &aVector);

  }
}
#endif

#ifndef __SHIRABE_AXISANGLE_H__
#define __SHIRABE_AXISANGLE_H__

#include "Math/Vector.h"

namespace Engine {
  namespace Math {

    class AxisAngle
      : public Vector4D {
    public:
      AxisAngle();
      AxisAngle(const AxisAngle&);
      AxisAngle(const Vector3D& axis, const value_type& phi);
      AxisAngle(
        value_type const& x,
        value_type const& y,
        value_type const& z,
        value_type const& phi);

      ~AxisAngle();

      inline const Vector3D axis() const {
        return Vector3D({ x(), y(), z() });
      }
      inline const value_type& phi() const { return m_field[3]; }

      inline const Vector3D axis(
        const value_type& x,
        const value_type& y,
        const value_type& z)
      {
        this->x(x); this->y(y); this->z(z);
        return this->axis(); // Hope for copy elision to work... otherwise switch to move semantics.
      }

      inline const value_type& phi(const value_type& v) { return (m_field[3] = v); }
    };

    Vector3D operator*(const AxisAngle&,
      const Vector3D&);

  }
}
#endif
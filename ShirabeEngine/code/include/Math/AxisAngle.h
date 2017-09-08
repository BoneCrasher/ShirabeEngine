#ifndef __SHIRABE_AXISANGLE_H__
#define __SHIRABE_AXISANGLE_H__

#include "Math/Vector.h"

namespace Engine {
	namespace Math {

		class AxisAngle
			: public Vector<4> {
		public:
			AxisAngle();
			AxisAngle(const AxisAngle&);
			AxisAngle(const Vector3D& axis, const value_type& phi);
			AxisAngle(const value_type& x,
					  const value_type& y,
					  const value_type& z,
					  const value_type& phi);

			~AxisAngle();

			inline const value_type& x()   const { return _field[0]; }
			inline const value_type& y()   const { return _field[1]; }
			inline const value_type& z()   const { return _field[2]; }

			inline const Vector3D axis() const {
				return Vector3D({ x(), y(), z() });
			}
			inline const value_type& phi() const { return _field[3]; }

			inline const value_type& x(const value_type& v)   { return (_field[0] = v); }
			inline const value_type& y(const value_type& v)   { return (_field[1] = v); }
			inline const value_type& z(const value_type& v)   { return (_field[2] = v); }
			inline const Vector3D    axis(const value_type& x,
										  const value_type& y,
										  const value_type& z)
			{
				this->x(x); this->y(y); this->z(z);
				return this->axis(); // Hope for copy elision to work... otherwise switch to move semantics.
			}

			inline const value_type& phi(const value_type& v) { return (_field[3] = v); }
		};
		
		Vector3D operator*(const AxisAngle&,
						   const Vector3D&);

	}
}
#endif
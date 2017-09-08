#include "Math/AxisAngle.h"


namespace Engine {
	namespace Math {

		AxisAngle::AxisAngle()
			: Vector<4>({ 0, 0, 0, 0 })
		{}

		AxisAngle::AxisAngle(const Vector3D&   axis,
							 const value_type& phi)
			: Vector<4>({ axis.x(), axis.y(), axis.z(), phi })
		{}

		AxisAngle::AxisAngle(const AxisAngle& axisAngle) 
			: Vector<4>({ })
		{ }

		AxisAngle::AxisAngle(const value_type& x,
							 const value_type& y,
							 const value_type& z,
							 const value_type& phi) 
			: Vector<4>({ x, y, z, phi })
		{}

		AxisAngle::~AxisAngle() {
		}

		Vector3D operator*(const AxisAngle &a,
						   const Vector3D  &v) {
			// Rodriguez rotation formula
			// v_rot = (cos(phi)*v + sin(phi)*cross(e, v) + (1 - cos(phi))*(dot(e, v)*e)
			typename AxisAngle::value_type cos_phi   = cosf(a.phi());
			typename AxisAngle::value_type sin_phi   = sinf(a.phi());
			Vector3D                       e         = a.axis();
			typename AxisAngle::value_type dot_e_v   = dot(e, v);
			Vector3D                       cross_e_v = cross(e, v);

			return cos_phi*v + sin_phi*cross_e_v + (1 - cos_phi)*(dot_e_v*e);
		}
	}
}

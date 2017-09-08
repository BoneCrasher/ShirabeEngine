#ifndef __SHIRABE_QUATERNION_H__
#define __SHIRABE_QUATERNION_H__

#include "Math/Vector.h"
#include "Math/AxisAngle.h"
#include "Math/Matrix.h"

namespace Engine {
	namespace Math {

		//
		// The set of quaternions is also known as the ring of Hamiltonian
		// quaternions, denoted by "H".
		//
		// An element of "H" is a 4 element vector:
		//
		//   q = <w, x, y, z> = w + xi + yj + zk
		//
		// In Scalar-Vector form:
		//
		//   q = s + <v> = w + <xi + yj + zk> 
		// 
		// For the multiplication of the imaginary parts, the sub-
		// sequent rules apply:
		//
		//   i^2 = j^2 = k^2 = ijk = -1
		//   ij  = -ji = k
		//   jk  = -kj = i
		//   ki  = -ik = j
		//

		class Quaternion
			: public Vector<4>
		{
			
		public:
			Quaternion();
			Quaternion(const Quaternion&);
			Quaternion(value_type w, const Vector3D&); 
			Quaternion(const AxisAngle&);
			Quaternion(value_type w, value_type x, value_type y, value_type z);
			~Quaternion();

			inline const value_type& w() const { return _field[0]; }
			inline const value_type& x() const { return _field[1]; }
			inline const value_type& y() const { return _field[2]; }
			inline const value_type& z() const { return _field[3]; }

			inline const value_type& w(const value_type& v) { return (_field[0] = v); }
			inline const value_type& x(const value_type& v) { return (_field[1] = v); }
			inline const value_type& y(const value_type& v) { return (_field[2] = v); }
			inline const value_type& z(const value_type& v) { return (_field[3] = v); }

			inline const Vector3D vector() const {
				return Vector3D(x(), y(), z());
			}

			inline const value_type scalar() const {
				return w();
			}

			Quaternion& operator+=(const Quaternion& other);
			Quaternion& operator-=(const Quaternion& other);
			Quaternion& operator*=(const value_type& other);
			Quaternion& operator*=(const Quaternion& other);
			Quaternion& operator/=(const value_type& other);

			value_type magnitude() const;
			Quaternion conjugate() const;
			Quaternion inverse()   const;
			Quaternion normalize() const;

			static value_type rotationAngle(const Quaternion& q);
			static Vector3D   rotationAxis(const Quaternion& q);

			static Quaternion quaternionFromEuler(const value_type& x,
												  const value_type& y,
												  const value_type& z);
			static Quaternion quaternionFromRotationMatrix(const Matrix4x4& m);
			static Quaternion quaternionFromAxisAngle(const Vector3D& axis, 
													  const value_type& phi);

			static Vector4D  axisAngleFromQuaternion(const Quaternion& q);
			static Vector3D  eulerFromQuaternion(const Quaternion& q);
			static Matrix4x4 rotationMatrixFromQuaternion(const Quaternion& q);

		private:
			void assign(value_type w, value_type x, value_type y, value_type z);
			void assign(const Quaternion& r);
		};

		Quaternion rotate(const Quaternion& l, const Quaternion& r);
		Vector3D   rotate(const Quaternion& l, const Vector3D&   r);

		Quaternion operator+(const Quaternion& q,  const Quaternion& other);
		Quaternion operator-(const Quaternion& q,  const Quaternion& other);
		Quaternion operator*(const Quaternion& q,  const Quaternion::value_type& factor);
		Quaternion operator*(const Quaternion::value_type& factor, const Quaternion& q);
		Quaternion operator*(const Quaternion& q,  const Vector3D& vec);
		Quaternion operator*(const Vector3D& vec,  const Quaternion& q);
		Quaternion operator*(const Quaternion& q,  const Quaternion& other);
		Quaternion operator/(const Quaternion& q,  const Quaternion::value_type& factor);
	}
}

#endif
#ifndef __SHIRABE_CQuaternion_H__
#define __SHIRABE_CQuaternion_H__

#include "math/vector.h"
#include "math/axisangle.h"
#include "math/matrix.h"

namespace Engine
{
    namespace Math
    {

		//
        // The set of CQuaternions is also known as the ring of Hamiltonian
        // CQuaternions, denoted by "H".
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

        class CQuaternion
            : public CVector4D_t
		{
			
		public:
            CQuaternion();
            CQuaternion(CQuaternion const &aOther);
            CQuaternion(
                    value_type  const &aPhi,
                    CVector3D_t const &aAxis);
            CQuaternion(CAxisAngle const &aAxisAngle);
            CQuaternion(
                    value_type const &aPhi,
                    value_type const &aAxisX,
                    value_type const &aAxisY,
                    value_type const &aAxisZ);
            virtual ~CQuaternion() final;
      
            inline const CVector3D_t vector() const
            {
                return CVector3D_t({ x(), y(), z() });
			}

            inline const value_type scalar() const
            {
				return w();
			}

            CQuaternion& operator+=(CQuaternion const &aOther);
            CQuaternion& operator-=(CQuaternion const &aOther);
            CQuaternion& operator*=(value_type  const &aOther);
            CQuaternion& operator*=(CQuaternion const &aOther);
            CQuaternion& operator/=(value_type  const &aOther);

            value_type  magnitude() const;
            CQuaternion conjugate() const;
            CQuaternion inverse()   const;
            CQuaternion normalize() const;

            static value_type  rotationAngle(CQuaternion const &aQuaternion);
            static CVector3D_t rotationAxis(CQuaternion const &aQuaternion);

            static CQuaternion CQuaternionFromEuler(
                    value_type const &aX,
                    value_type const &aY,
                    value_type const &aZ);

            static CQuaternion CQuaternionFromRotationMatrix(
                    CMatrix4x4 const &aMatrix);

            static CQuaternion CQuaternionFromAxisAngle(
                    CVector3D_t const &aAxis,
                    value_type  const &aPhi);

            static CVector4D_t axisAngleFromCQuaternion(CQuaternion const &aQuaternion);
            static CVector3D_t eulerFromCQuaternion(CQuaternion const &aQuaternion);
            static CMatrix4x4  rotationMatrixFromCQuaternion(CQuaternion const &aQuaternion);

		private:
            void assign(
                    value_type const &aPhi,
                    value_type const &aAxisX,
                    value_type const &aAxisY,
                    value_type const &aAxisZ);
            void assign(CQuaternion const &aOther);
		};

        CQuaternion  rotate(CQuaternion const &aQuaternionLHS, CQuaternion const &aQuaternionRHS);
        CVector3D_t  rotate(CQuaternion const &aQuaternion, CVector3D_t const &aVector);

        CQuaternion operator+(CQuaternion const &aQuaternion,         CQuaternion const &aOther);
        CQuaternion operator-(CQuaternion const &aQuaternion,         CQuaternion const &aOther);
        CQuaternion operator*(CQuaternion const &aQuaternion,         CQuaternion::value_type const& aFactor);
        CQuaternion operator*(CQuaternion::value_type const& aFactor, CQuaternion const &aQuaternion);
        CQuaternion operator*(CQuaternion const &aQuaternion,         CVector3D_t const &aVector);
        CQuaternion operator*(CVector3D_t const &aVector,             CQuaternion const &aQuaternion);
        CQuaternion operator*(CQuaternion const &aQuaternion,         CQuaternion const &aOther);
        CQuaternion operator/(CQuaternion const &aQuaternion,         CQuaternion::value_type const&aFactor);
	}
}

#endif

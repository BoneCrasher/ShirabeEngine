#ifndef __SHIRABE_CQuaternion_H__
#define __SHIRABE_CQuaternion_H__

#include "math/vector.h"
#include "math/axisangle.h"
#include "math/matrix.h"

namespace engine
{
    namespace math
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

        /**
         * CQuaternion implements basic operations based on the hamilton quaterions
         * tied to rotation operations in the game engine.
         */
        class CQuaternion
            : public CVector4D_t
		{			
        public_static_functions:
            /**
             * Construct a quaternion from a rotation matrix.
             *
             * @param aMatrix
             * @return
             */
            static CQuaternion CQuaternionFromRotationMatrix(
                    CMatrix4x4 const &aMatrix);

            /**
             * Construct a quaternion from a rotation axis and angle.
             *
             * @param aAxis
             * @param aPhi
             * @return
             */
            static CQuaternion CQuaternionFromAxisAngle(
                    CVector3D_t const &aAxis,
                    ValueType_t const &aPhi);

            /**
             * Extract a rotation vector (xyz) and angle (w) from a quaternion.
             *
             * @param aQuaternion
             * @return
             */
            static CVector4D_t axisAngleFromCQuaternion(CQuaternion const &aQuaternion);
            /**
             * Extract the x, y and z euler angles from a quaternion.
             *
             * @param aQuaternion
             * @return
             */
            static CVector3D_t eulerFromCQuaternion(CQuaternion const &aQuaternion);

            /**
             * Extract a rotation matrix from a quaternion.
             *
             * @param aQuaternion
             * @return
             */
            static CMatrix4x4  rotationMatrixFromCQuaternion(CQuaternion const &aQuaternion);

        public_constructors:
            /**
             * Default construct a zero quaternion.
             */
            CQuaternion();

            /**
             * Copy-Construct a quaternion from 'aOther'.
             *
             * @param aOther
             */
            CQuaternion(CQuaternion const &aOther);

            /**
             * Construct a quaternion using an axis and an angle.
             *
             * @param aPhi
             * @param aAxis
             */
            CQuaternion(
                    ValueType_t const &aPhi,
                    CVector3D_t const &aAxis);

            /**
             * Construct a quaternion using an AxisAngle instance.
             *
             * @param aAxisAngle
             */
            CQuaternion(
                    CAxisAngle const &aAxisAngle);

            /**
             * Construct a quaternion using an angle and three axis componentx x, y and z.
             *
             * @param aPhi
             * @param aAxisX
             * @param aAxisY
             * @param aAxisZ
             */
            CQuaternion(
                    ValueType_t const &aPhi,
                    ValueType_t const &aAxisX,
                    ValueType_t const &aAxisY,
                    ValueType_t const &aAxisZ);

        public_destructors:
            /**
             * Destroy this quaternion.
             */
            virtual ~CQuaternion() final;

        public_operators:
            /**
             * Add 'aOther' quaternion to this quaternion instance.
             *
             * @param aOther
             * @return
             */
            CQuaternion &operator+=(CQuaternion const &aOther);
            /**
             * Subtract 'aOther' quaternion from this quaternion instance.
             *
             * @param aOther
             * @return
             */
            CQuaternion &operator-=(CQuaternion const &aOther);
            /**
             * Scale this quaternion instance by another scalar.
             *
             * @param aOther
             * @return
             */
            CQuaternion &operator*=(ValueType_t const &aOther);
            /**
             * Multiply this quaternion instance with 'aOther'.
             *
             * @param aOther
             * @return
             */
            CQuaternion &operator*=(CQuaternion const &aOther);
            /**
             * Divide this quaternion instance by 'aOther'.
             *
             * @param aOther
             * @return
             */
            CQuaternion &operator/=(ValueType_t const &aOther);

        public_methods:
            /**
             * Return a vector representation of the quaternion axis.
             *
             * @return
             */
            SHIRABE_INLINE CVector3D_t const vector() const
            {
                return CVector3D_t({ x(), y(), z() });
            }

            /**
             * Return the angle component of the quaternion.
             *
             * @return
             */
            SHIRABE_INLINE ValueType_t const scalar() const
            {
                return w();
            }

            /**
             * Calculate and return the magnitude of this quaternion.
             *
             * @return
             */
            ValueType_t magnitude() const;

            /**
             * Return the conjugate of this quaternion.
             *
             * @return
             */
            CQuaternion conjugate() const;
            /**
             * Return the inverse of this quaternion.
             *
             * @return
             */
            CQuaternion inverse()   const;
            /**
             * Return a normalized copy of this quaternion.
             *
             * @return
             */
            CQuaternion normalize() const;

            /**
             * Extract the rotation angle of a specific quaternion instance.
             *
             * @param aQuaternion
             * @return
             */
            static ValueType_t rotationAngle(CQuaternion const &aQuaternion);

            /**
             * Extract the rotation axis of a specific quaternion instance.
             *
             * @param aQuaternion
             * @return
             */
            static CVector3D_t rotationAxis(CQuaternion const &aQuaternion);

            /**
             * Construct a quaternion from euler angles.
             *
             * @param aX
             * @param aY
             * @param aZ
             * @return
             */
            static CQuaternion CQuaternionFromEuler(
                    ValueType_t const &aX,
                    ValueType_t const &aY,
                    ValueType_t const &aZ);

        private_methods:
            /**
             * Assign an angle aPhi and three axis components to this quaternion.
             *
             * @param aPhi
             * @param aAxisX
             * @param aAxisY
             * @param aAxisZ
             */
            void assign(
                    ValueType_t const &aPhi,
                    ValueType_t const &aAxisX,
                    ValueType_t const &aAxisY,
                    ValueType_t const &aAxisZ);

            /**
             * Assign another quaternion.
             *
             * @param aOther
             */
            void assign(CQuaternion const &aOther);
		};

        /**
         * Rotate a quaternion around another quaternion.
         *
         * @param aQuaternionLHS
         * @param aQuaternionRHS
         * @return
         */
        CQuaternion  rotate(CQuaternion const &aQuaternionLHS, CQuaternion const &aQuaternionRHS);

        /**
         * Rotate a quaternion along three axis, whose angles are defined in the passed 3D vector.
         *
         * @param aQuaternion
         * @param aVector
         * @return
         */
        CVector3D_t  rotate(CQuaternion const &aQuaternion, CVector3D_t const &aVector);

        /**
         * Return the sum of two quaternions.
         *
         * @param aQuaternion
         * @param aOther
         * @return
         */
        static CQuaternion operator+(CQuaternion const &aQuaternion, CQuaternion const &aOther);
        /**
         * Return the difference of two quaternions.
         *
         * @param aQuaternion
         * @param aOther
         * @return
         */
        static CQuaternion operator-(CQuaternion const &aQuaternion, CQuaternion const &aOther);
        /**
         * Return the product of a quaternion and a scale factor.
         *
         * @param aQuaternion
         * @param aFactor
         * @return
         */
        static CQuaternion operator*(CQuaternion const &aQuaternion, CQuaternion::ValueType_t const& aFactor);
        /**
         * Return the product of a scale factor and a quaternion.
         *
         * @param aFactor
         * @param aQuaternion
         * @return
         */
        static CQuaternion operator*(CQuaternion::ValueType_t const& aFactor, CQuaternion const &aQuaternion);
        /**
         * Return the product of a quaternion and a vector.
         *
         * @param aQuaternion
         * @param aVector
         * @return
         */
        static CQuaternion operator*(CQuaternion const &aQuaternion, CVector3D_t const &aVector);

        /**
         * Return the product of a vector and a quaternion.
         *
         * @param aVector
         * @param aQuaternion
         * @return
         */
        static CQuaternion operator*(CVector3D_t const &aVector, CQuaternion const &aQuaternion);
        /**
         * Return the product of two quaternions.
         *
         * @param aQuaternion
         * @param aOther
         * @return
         */
        static CQuaternion operator*(CQuaternion const &aQuaternion, CQuaternion const &aOther);

        /**
         * Divide a quaternion by a scale factor.
         *
         * @param aQuaternion
         * @param aFactor
         * @return
         */
        static CQuaternion operator/(CQuaternion const &aQuaternion, CQuaternion::ValueType_t const&aFactor);
	}
}

#endif

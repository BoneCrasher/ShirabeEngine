#ifndef __SHIRABE_CAxisAngle_H__
#define __SHIRABE_CAxisAngle_H__

#include <base/declaration.h>
#include "math/vector.h"

namespace engine
{
    namespace math
    {
        /**
         * An AxisAngle is a construct describing a regular rotation axis and a rotation angle around it.
         */
        class CAxisAngle
                : public CVector4D_t
        {
        public_constructors:
            /**
             * Construct a zero axis angle.
             */
            CAxisAngle();
            /**
             * Copy-Construct this axis angle from another.
             *
             * @param aOther
             */
            CAxisAngle(CAxisAngle const&aOther);
            /**
             * Construct an axis-angle from an axis and angle value.
             *
             * @param aAxis
             * @param aPhi
             */
            CAxisAngle(
                    CVector3D_t const &aAxis,
                    ValueType_t const &aPhi);
            /**
             * Construct an axis-angle from three axis coefficients and an angle.
             *
             * @param aX
             * @param aY
             * @param aZ
             * @param aPhi
             */
            CAxisAngle(
                    ValueType_t const& aX,
                    ValueType_t const& aY,
                    ValueType_t const& aZ,
                    ValueType_t const& aPhi);

        public_destructors:
            /**
             * Destroy and run...
             */
            ~CAxisAngle();

        public_methods:
            /**
             * Return the axis.
             *
             * @return See brief.
             */
            CVector3D_t const axis() const;

            /**
             * Return the angle.
             *
             * @return See brief.
             */
            ValueType_t const &phi() const;

            /**
             * Set the axis of this axis-angle instance.
             *
             * @param aX
             * @param aY
             * @param aZ
             * @return
             */
            CVector3D_t const axis(
                    ValueType_t const& aX,
                    ValueType_t const& aY,
                    ValueType_t const& aZ);

            /**
             * Set the angle of this axis-angle instance.
             *
             * @param aPhi
             * @return
             */
            ValueType_t const &phi(ValueType_t const &aPhi);
        };

        /**
         * Rotate a vector by an axis angle.
         *
         * @param aAxisAngle
         * @param aVector
         * @return
         */
        CVector3D_t operator*(
                CAxisAngle  const &aAxisAngle,
                CVector3D_t const &aVector);

    }
}
#endif

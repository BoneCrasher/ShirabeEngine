#ifndef __SHIRABE_TRANSFORM_H__
#define __SHIRABE_TRANSFORM_H__

#include <atomic> 

#include <math/vector.h>
#include <math/matrix.h>
#include <math/quaternion.h>

namespace engine
{

    using namespace math;

#define __SHIRABE_USE_QUATERNION_ROTATIONS

    /**
     * The CTransform class defines all values and operations to position, orient and scale
     * any engine entity in 3D space.
     *
     * It also provides functionality to create a worldmatrix and accumulate parent transformations.
     */
    class SHIRABE_LIBRARY_EXPORT CTransform
    {
    public_constructors:
        /**
         * Default construct an identity transform.
         */
        CTransform();

    public_destructors:
        /**
         * Destroy and run...
         */
        ~CTransform();

    public_methods:
        /**
         * Rotate the transform along three euler angles by the degree angles provided
         * in the 3D vector.
         *
         * @param aEulerRotation A 3D vector containing the degree angles to rotate each euler axis around.
         * @return               Return a reference to the rotated transform.
         */
        CTransform &rotate(CVector3D<float> const &aEulerRotation);

        /**
         * Rotate the transform around a specific 3D axis and degree angle.
         *
         * @param aAxis The axis to rotate around
         * @param aPhi  The degree angle of rotation.
         * @return      Return a reference to the rotated transform.
         */
        CTransform &rotate(CVector3D<float> const &aAxis,
                           float       const &aPhi);

        /**
         *  Rotate the transform by a quaternion.
         *
         * @param aQaternionRotation The quaternion used for rotation.
         * @return                   Return a reference to the rotated transform.
         */
        CTransform &rotate(CQuaternion const &aQaternionRotation);

        SHIRABE_INLINE
        CTransform &resetRotation()
        {
            mLocalRotationQuaternion = CQuaternion(1, 0, 0, 0);
            return (*this);
        }

        /**
         * Reset the current rotation to a specific euler rotation vector (Default: 0-rotation).
         *
         * @param aEulerRotation A 3D vector containing the degree angles to rotate each euler axis around.
         * @return               Return a reference to the rotated transform.
         */
        CTransform &resetRotation(CVector3D<float> const &aEulerRotation);

        /**
         * Reste the current rotation to a specific angle rotation around the provided axis.
         *
         * @param aAxis The axis to rotate around
         * @param aPhi  The degree angle of rotation.
         * @return      Return a reference to the rotated transform.
         */
        CTransform &resetRotation(CVector3D<float> const &aAxis,
                                  float       const &aPhi);

        /**
         * Reset the current rotation to a specific quaternion rotation representation.
         *
         * @param quaternionRotation The quaternion used for rotation.
         * @return                   Return a reference to the rotated transform.
         */
        CTransform &resetRotation(CQuaternion const &aQuaternionRotation);

        /**
         * Translate the transform along the current right, up and forward axes.
         *
         * @param aTranslation
         * @return
         */
        CTransform &translate(CVector3D<float> const &aTranslation);

        /**
         * Reset the translation to a specific 3D vector.
         *
         * @param aTranslation
         * @return
         */
        CTransform &resetTranslation(CVector3D<float> const &aTranslation = CVector3D<float>({ 0.0, 0.0, 0.0 }));

        /**
         *
         *
         * @param aFactor
         * @return
         */
        CTransform &scale(float const &aFactor);

        /**
         *
         *
         * @param aFactors
         * @return
         */
        CTransform &scale(CVector3D<float> const &aFactors);

        /**
         *
         *
         * @param aFactor
         * @return
         */
        CTransform &resetScale(float const &aFactor = 1.0);

        /**
         *
         *
         * @param aFactors
         * @return
         */
        CTransform &resetScale(CVector3D<float> const &aFactors = CVector3D<float>({ 1.0, 1.0, 1.0 }));

        /**
         *
         *
         * @return
         */
        CTransform &reset();

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D<float> forward() const
        {
            return (mLocalRotationQuaternion * CVector3D<float>::forward()).vector();
        }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D<float> right() const
        {
            return (mLocalRotationQuaternion * CVector3D<float>::right()).vector();
        }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D<float> up() const
        {
            return (mLocalRotationQuaternion * CVector3D<float>::up()).vector();
        }

        /**
         *
         *
         * @return
         */
        CMatrix4x4 const &local();

        /**
         *
         *
         * @return
         */
        CMatrix4x4 const &world() const;

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D<float> const &localTranslation() const
        {
            return mLocalTranslation;
        }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D<float> const translation() const
        {
            return extractTranslationFromMatrix(mCurrentWorldTransform);
        }

        /**
         *
         *
         * @param aParent
         * @return
         */
        CMatrix4x4 const &updateWorldTransform(CMatrix4x4 const &aParent);

    private_methods:
        /**
         *
         */
        SHIRABE_INLINE void setDirty() { mDirty = true; }

        /**
         *
         */
        SHIRABE_INLINE void setClean() { mDirty = false; }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE bool isDirty() const { return mDirty; }

        /**
         *
         *
         * @param aMatrix
         * @return
         */
        SHIRABE_INLINE const CVector3D<float> extractTranslationFromMatrix(CMatrix4x4 const &aMatrix) const
        {
            CVector3D<float> const translation = CVector3D<float>({
                                                aMatrix.r03(),
                                                aMatrix.r13(),
                                                aMatrix.r23()
                                            });

            return translation;
        }

        CVector3D<float>      mLocalScale;
#ifndef __SHIRABE_USE_QUATERNION_ROTATIONS
        CVector3D<float>      m_localEulerRotation;
#else 
        CQuaternion      mLocalRotationQuaternion;
#endif
        CVector3D<float>      mLocalTranslation;

        CMatrix4x4       mCurrentLocalTransform;
        CMatrix4x4       mCurrentWorldTransform;

        bool             mDirty;
    };

}

#endif

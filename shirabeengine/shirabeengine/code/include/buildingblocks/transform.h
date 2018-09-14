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
        CTransform &rotate(CVector3D_t const &aEulerRotation);

        /**
         * Rotate the transform around a specific 3D axis and degree angle.
         *
         * @param aAxis The axis to rotate around
         * @param aPhi  The degree angle of rotation.
         * @return      Return a reference to the rotated transform.
         */
        CTransform &rotate(CVector3D_t const &aAxis,
                           float       const &aPhi);

        /**
         *  Rotate the transform by a quaternion.
         *
         * @param aQaternionRotation The quaternion used for rotation.
         * @return                   Return a reference to the rotated transform.
         */
        CTransform &rotate(CQuaternion const &aQaternionRotation);

        /**
         * Reset the current rotation to a specific euler rotation vector (Default: 0-rotation).
         *
         * @param aEulerRotation A 3D vector containing the degree angles to rotate each euler axis around.
         * @return               Return a reference to the rotated transform.
         */
        CTransform &resetRotation(CVector3D_t const &aEulerRotation = CVector3D_t({ 0, 0, 0 }));

        /**
         * Reste the current rotation to a specific angle rotation around the provided axis.
         *
         * @param aAxis The axis to rotate around
         * @param aPhi  The degree angle of rotation.
         * @return      Return a reference to the rotated transform.
         */
        CTransform &resetRotation(CVector3D_t const &aAxis = CVector3D_t({ 1, 0, 0 }),
                                  float       const &aPhi  = 0.0f);

        /**
         * Reset the current rotation to a specific quaternion rotation representation.
         *
         * @param quaternionRotation The quaternion used for rotation.
         * @return                   Return a reference to the rotated transform.
         */
        CTransform &resetRotation(CQuaternion const &aQuaternionRotation = CQuaternion(1, 0, 0, 0));

        /**
         * Translate the transform along the current right, up and forward axes.
         *
         * @param aTranslation
         * @return
         */
        CTransform &translate(CVector3D_t const &aTranslation);

        /**
         * Reset the translation to a specific 3D vector.
         *
         * @param aTranslation
         * @return
         */
        CTransform &resetTranslation(CVector3D_t const &aTranslation = CVector3D_t({ 0.0, 0.0, 0.0 }));

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
        CTransform &scale(CVector3D_t const &aFactors);

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
        CTransform &resetScale(CVector3D_t const &aFactors = CVector3D_t({ 1.0, 1.0, 1.0 }));

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
        SHIRABE_INLINE CVector3D_t forward() const
        {
            return CQuaternion::eulerFromQuaternion(mLocalRotationQuaternion * CVector3D_t::forward());
        }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D_t right() const
        {
            return CQuaternion::eulerFromQuaternion(mLocalRotationQuaternion * CVector3D_t::right());
        }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D_t up() const
        {
            return CQuaternion::eulerFromQuaternion(mLocalRotationQuaternion * CVector3D_t::up());
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
        SHIRABE_INLINE CVector3D_t const &localTranslation() const
        {
            return mLocalTranslation;
        }

        /**
         *
         *
         * @return
         */
        SHIRABE_INLINE CVector3D_t const translation() const
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
        SHIRABE_INLINE const CVector3D_t extractTranslationFromMatrix(CMatrix4x4 const &aMatrix) const
        {
            CVector3D_t const translation = CVector3D_t({
                                                aMatrix.r03(),
                                                aMatrix.r13(),
                                                aMatrix.r23()
                                            });

            return translation;
        }

        CVector3D_t      mLocalScale;
#ifndef __SHIRABE_USE_QUATERNION_ROTATIONS
        CVector3D_t      m_localEulerRotation;
#else 
        CQuaternion      mLocalRotationQuaternion;
#endif
        CVector3D_t      mLocalTranslation;

        CMatrix4x4       mCurrentLocalTransform;
        CMatrix4x4       mCurrentWorldTransform;

        bool             mDirty;
    };

}

#endif

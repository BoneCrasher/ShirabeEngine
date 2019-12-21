#include "buildingblocks/transform.h"

namespace engine
{
    using namespace math;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CTransform::CTransform()
        : mLocalScale             (CVector3D<float>({ 1.0f, 1.0f, 1.0f }))
        , mLocalRotationQuaternion(CQuaternion( 1.0f, 0.0f, 0.0f, 0.0f ))
        , mLocalTranslation       (CVector3D<float>({ 0.0f, 0.0f, 0.0f }))
        , mCurrentLocalTransform  (CMatrix4x4::identity())
        , mCurrentWorldTransform  (CMatrix4x4::identity())
        , mDirty                  (true)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform::~CTransform()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::rotate(CVector3D<float> const &aEulerRotation)
    {
        auto const q = CQuaternion::quaternionFromEuler(aEulerRotation);
        mLocalRotationQuaternion *= q;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::rotate(CVector3D<float> const &aAxis,
                                   float       const &aPhi)
    {
        auto const q = CQuaternion::quaternionFromAxisAngle(aAxis, aPhi);
        mLocalRotationQuaternion *= q;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::rotate(CQuaternion const &aQuaternionRotation)
    {
        mLocalRotationQuaternion *= aQuaternionRotation;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::resetRotation(CVector3D<float> const &aEulerRotation)
    {
        mLocalRotationQuaternion = CQuaternion::quaternionFromEuler(aEulerRotation);

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::resetRotation(CVector3D<float> const &aAxis,
                                          float       const &aPhi)
    {
        mLocalRotationQuaternion = CQuaternion::quaternionFromAxisAngle(aAxis, aPhi);

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::resetRotation(CQuaternion const &aQuaternionRotation)
    {
        mLocalRotationQuaternion = aQuaternionRotation;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::translate(CVector3D<float> const &aTranslation)
    {
        mLocalTranslation += aTranslation;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::resetTranslation(CVector3D<float> const &aTranslation)
    {
        mLocalTranslation = aTranslation;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::scale(float const &aFactor)
    {
        mLocalScale.scale(aFactor);

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::scale(CVector3D<float> const &aFactors)
    {
        mLocalScale.x(mLocalScale.x() * aFactors.x());
        mLocalScale.y(mLocalScale.y() * aFactors.y());
        mLocalScale.z(mLocalScale.z() * aFactors.z());

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::resetScale(float const &aFactor)
    {
        mLocalScale = {aFactor, aFactor, aFactor};

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::resetScale(CVector3D<float> const &aFactors)
    {
        mLocalScale = aFactors;

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CTransform &CTransform::reset()
    {
        resetScale({1.0f, 1.0f, 1.0f});
        resetRotation(CQuaternion(1.0f, 0.0f, 0.0f, 0.0f));
        resetTranslation();

        mDirty = true;
        return (*this);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CMatrix4x4 const &CTransform::local()
    {
        if(mDirty)
        {
            CMatrix4x4 scale = CMatrix4x4::identity();
            scale.r00(mLocalScale.x());
            scale.r11(mLocalScale.y());
            scale.r22(mLocalScale.z());

            CMatrix4x4 rotation = CQuaternion::rotationMatrixFromQuaternion(mLocalRotationQuaternion);

            CMatrix4x4 translation = CMatrix4x4::identity();
            translation.r03(mLocalTranslation.x());
            translation.r13(mLocalTranslation.y());
            translation.r23(mLocalTranslation.z());

            mCurrentLocalTransform = scale * rotation * translation;
            mDirty = false;
        }

        return mCurrentLocalTransform;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CMatrix4x4 const &CTransform::world() const
    {
        return mCurrentWorldTransform;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CMatrix4x4 const &CTransform::updateWorldTransform(CMatrix4x4 const &aParent)
    {
        return (mCurrentWorldTransform = SMMatrixMultiply(local(), aParent));
    }
    //<-----------------------------------------------------------------------------
}

#include <math/geometric/rect.h>
#include "buildingblocks/camera.h"

//#ifdef SHIRABE_PLATFORM_WINDOWS 
//#include <DirectXMath.h>
//#endif

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CCamera::CCamera()
        : mViewType(ECameraViewType::FreeCamera)
        , mFrustumParameters(SFrustumParameters::Default())
        , mProjectionParameters(SProjectionParameters::Default())
        , mLookAtTarget({ 0, 0, 0 })
        , mTransform()
        , mViewMatrix()
        , mProjectionMatrix()
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CCamera::CCamera(CCamera const &aOther)
        : mViewType(aOther.viewType())
        , mFrustumParameters(aOther.frustumParameters())
        , mProjectionParameters(aOther.projectionParameters())
        , mLookAtTarget(aOther.lookAtTarget())
        , mTransform(aOther.mTransform)
        , mViewMatrix(aOther.mViewMatrix)
        , mProjectionMatrix(aOther.mProjectionMatrix)
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CCamera::CCamera(ECameraViewType       const &aViewType,
                     SFrustumParameters    const &aFrustumParameters,
                     SProjectionParameters const &aProjectionParameters,
                     CVector3D_t           const &aLookAt)
        : mViewType(aViewType)
        , mFrustumParameters(aFrustumParameters)
        , mProjectionParameters(aProjectionParameters)
        , mLookAtTarget(aLookAt)
        , mTransform()
        , mViewMatrix()
        , mProjectionMatrix()
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CCamera::~CCamera()
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Create a look to matrix pointing into a specific direction.
     *
     * @param aEye              The current position of the camera.
     * @param aUp               The current upward pointing vector of the camera.
     * @param aForward          The current look to direction of the camera.
     * @param aCoordinateSystem The handedness to create the matrix in.
     * @return                  Return the view matrix created.
     */
    static CMatrix4x4 lookTo(
            CVector3D_t       const &aEye,
            CVector3D_t       const &aUp,
            CVector3D_t       const &aForward,
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::LH)
    {
        CVector3D_t const n_up      = math::normalize(aUp);
        CVector3D_t const n_forward = math::normalize(aForward);
        CVector3D_t const n_right   = math::normalize(math::cross(n_forward, n_up));

        CMatrix4x4  const orientation = CMatrix4x4({
            n_right.x(), n_up.x(), n_forward.x(), 0.0f,
            n_right.y(), n_up.y(), n_forward.y(), 0.0f,
            n_right.z(), n_up.z(), n_forward.z(), 0.0f,
            0.0f,        0.0f,     0.0f,          1.0f
        });

        CMatrix4x4 const translation = CMatrix4x4({
            1.0f,      0.0f,      0.0f,      0.0f,
            0.0f,      1.0f,      0.0f,      0.0f,
            0.0f,      0.0f,      1.0f,      0.0f,
            -aEye.x(), -aEye.y(), -aEye.z(), 1.0f
        });

        return SMMatrixMultiply(orientation, translation);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------

    /**
     * Create a look to matrix pointing into a specific direction.
     *
     * @param aEye              The current position of the camera.
     * @param aUp               The current upward pointing vector of the camera.
     * @param aTarget           The currently locked target position to look to.
     * @param aCoordinateSystem The handedness to create the matrix in.
     * @return                  Return the view matrix created.
     */
    static CMatrix4x4 lookAt(
            CVector3D_t       const &aEye,
            CVector3D_t       const &aUp,
            CVector3D_t       const &aTarget,
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::RH)
    {
        CVector3D_t const n_up      = math::normalize(aUp);
        CVector3D_t const n_forward = math::normalize((aTarget - aEye));
        CVector3D_t const n_right   = math::normalize(math::cross(n_forward, n_up));

        CMatrix4x4 const orientation = CMatrix4x4({
            n_right.x(), n_up.x(), n_forward.x(), 0.0f,
            n_right.y(), n_up.y(), n_forward.y(), 0.0f,
            n_right.z(), n_up.z(), n_forward.z(), 0.0f,
            0.0f,        0.0f,     0.0f,          1.0f
        });

        CMatrix4x4 const translation = CMatrix4x4({
            1.0f,      0.0f,      0.0f,      0.0f,
            0.0f,      1.0f,      0.0f,      0.0f,
            0.0f,      0.0f,      1.0f,      0.0f,
            -aEye.x(), -aEye.y(), -aEye.z(), 1.0f
        });

        return SMMatrixMultiply(orientation, translation);
    }

    /**
     * Create a perspective projection matrix from rectangular near plane boundaries.
     *
     * @param aBounds            The bounding rectangle of the near plane on the screen.
     * @param aNear              The near plane distance of the frustum.
     * @param aFar               The far plane distance of the frustum.
     * @param aCoordinateSystem  The handedness to apply to the matrix.
     * @return                   A projection matrix in the requested handedness.
     */
    static CMatrix4x4 projectionPerspectiveRect(
            CVector4D_t       const &aBounds,
            double            const &aNear,
            double            const &aFar,
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::LH)
    {
        float const coordinateSystemFactor = ((aCoordinateSystem == ECoordinateSystem::LH) ? 1.0 : -1.0);

        float const l = aBounds.x();
        float const t = aBounds.y();
        float const r = aBounds.z();
        float const b = aBounds.w();
        float const f = aFar;
        float const n = aNear;

        float const width  = (r - l);
        float const height = (t - b);
        float const depth  = (f - n);
        float const n2     = (2.0f * n);
        float const fn2    = (f * n2);

        CMatrix4x4 const projection = CMatrix4x4({
            (n2 / width),      0.0f,                0.0f,                                     0.0f,
            0.0f,              (n2 / height),       0.0f,                                     0.0f,
            ((r + l) / width), ((t + b) / height), -((f + n) / depth),                        coordinateSystemFactor,
            0.0f,              0.0f,               -((fn2 / depth) * coordinateSystemFactor), 0.0f
        });

        return projection;
    }

    /**
     * Create a perspective projection matrix from an aspect ratio of the near plane and a vertical
     * field of view angle.
     *
     * @param aBounds            The bounding rectangle of the near plane on the screen.
     * @param aNear              The near plane distance of the frustum.
     * @param aFar               The far plane distance of the frustum.
     * @param aFovY              The vertical field of view angle in degrees.
     * @param aCoordinateSystem  The handedness to apply to the matrix.
     * @return                   A projection matrix in the requested handedness.
     */
    static CMatrix4x4 projectionPerspectiveFOV(
            CRect             const &aBounds,
            double            const &aNear,
            double            const &aFar,
            double            const &aFovY,
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::RH)
    {
        float const aspect       =  ((float)aBounds.size.x() / (float)aBounds.size.y());
        float const fovFactorRad =  ((aFovY/2.0f) * (M_PI/180.0f));
        float const t            =  tan(fovFactorRad) * aNear;
        float const b            = -t;
        float const r            =  t * aspect;
        float const l            = -r;

        CMatrix4x4 const projection = projectionPerspectiveRect(CVector4D_t({ l, t, r, b }), aNear, aFar, aCoordinateSystem);
        return projection;
    }

    /**
     * Create an orthographic projection matrix from a near plane bounding rectangle and near and far
     * plane setting.
     *
     * @param aBounds            The bounding rectangle of the near plane on the screen.
     * @param aNear              The near plane distance of the frustum.
     * @param aFar               The far plane distance of the frustum.
     * @param aCoordinateSystem  The handedness to apply to the matrix.
     * @return                   The projection matrix in the given handedness.
     */
    static CMatrix4x4 projectionOrtho(
            CVector4D_t       const &aBounds,
            double            const &aNear,
            double            const &aFar,
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::RH)
    {
        float const coordinateSystemFactor =  ((aCoordinateSystem == ECoordinateSystem::LH) ? 1.0 : -1.0);

        float const l = aBounds.x();
        float const t = aBounds.y();
        float const r = aBounds.z();
        float const b = aBounds.w();
        float const f = aFar;
        float const n = aNear;

        CMatrix4x4 const projection = CMatrix4x4({
            (2.0f / (r - l)), 0.0f,             0.0f,           -((r + l) / (r - l)),
            0.0f,             (2.0f / (t - b)), 0.0f,           -((t + b) / (t - b)),
            0.0f,             0.0f,             -(2 / (f - n)), -((f + n) / (f - n)) * coordinateSystemFactor,
            0.0f,             0.0f,             0.0f,           1.0f
        });

        return projection;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CCamera::createViewMatrix(ECoordinateSystem const &aCoordinateSystem)
    {
        CVector3D_t const position = mTransform.translation();
        CVector3D_t const up       = mTransform.up();

        CMatrix4x4 mat = {};

        switch(mViewType)
        {
        case ECameraViewType::TargetCamera:
            mat = lookAt(position, mLookAtTarget, up, aCoordinateSystem);
            break;
        case ECameraViewType::FreeCamera:
            mat = lookTo(position, up, mTransform.forward());
            break;
        }

        mViewMatrix = mat;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CCamera::createProjectionMatrix(ECoordinateSystem const&aCoordinateSystem)
    {
        CMatrix4x4 mat = {};

        switch(mProjectionParameters.projectionType)
        {
        case ECameraProjectionType::Perspective:
            mat = projectionPerspectiveFOV(
                        { 0, 0, mFrustumParameters.width, mFrustumParameters.height },
                        mFrustumParameters.nearPlaneDistance,
                        mFrustumParameters.farPlaneDistance,
                        mFrustumParameters.fovY,
                        aCoordinateSystem);
            break;
        case ECameraProjectionType::Orthographic:
            mat = projectionOrtho(
                        CVector4D_t({
                            (0.5f * mFrustumParameters.width),
                            (0.5f * mFrustumParameters.height),
                            (0.5f * mFrustumParameters.width),
                            (0.5f * mFrustumParameters.height)
                        }),
                        mFrustumParameters.nearPlaneDistance,
                        mFrustumParameters.farPlaneDistance,
                        aCoordinateSystem);
            break;
        }

        mProjectionMatrix = mat;
    }
    //<-----------------------------------------------------------------------------
}

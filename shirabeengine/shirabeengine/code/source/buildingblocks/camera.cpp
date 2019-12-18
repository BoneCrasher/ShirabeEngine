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
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::RH)
    {      
        SHIRABE_UNUSED(aCoordinateSystem);

        CVector3D_t       n_up      = math::normalize(aUp);
        CVector3D_t const n_forward = math::normalize(aForward);
        CVector3D_t const n_right   = math::normalize(math::cross(n_up, n_forward));
        n_up = math::normalize(math::cross(n_forward, n_right));

        CMatrix4x4 const view = CMatrix4x4({
                n_right.x(),         n_up.x(),        -n_forward.x(),        0.0f,
                n_right.y(),         n_up.y(),        -n_forward.y(),        0.0f,
                n_right.z(),         n_up.z(),        -n_forward.z(),        0.0f,
                -dot(n_right, aEye), -dot(n_up,aEye), -dot(n_forward, aEye), 1.0f
        });

        return view;
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
        SHIRABE_UNUSED(aCoordinateSystem);

        CVector3D_t       n_up      = math::normalize(aUp);
        CVector3D_t const n_forward = math::normalize((aTarget - aEye));
        CVector3D_t const n_right   = math::normalize(math::cross(n_forward, n_up));
        n_up = math::normalize(math::cross(n_right, n_forward));

        CMatrix4x4 const view = CMatrix4x4({
            n_right.x(),         n_up.x(),        -n_forward.x(),        0.0f,
            n_right.y(),         n_up.y(),        -n_forward.y(),        0.0f,
            n_right.z(),         n_up.z(),        -n_forward.z(),        0.0f,
            -dot(n_right, aEye), -dot(n_up,aEye), -dot(n_forward, aEye), 1.0f
        });

        return view;
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
            ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::RH)
    {
        float const coordinateSystemFactor = ((ECoordinateSystem::LH == aCoordinateSystem) ? 1.0f : -1.0f);

        float const l = aBounds.x();
        float const t = aBounds.y();
        float const r = aBounds.z();
        float const b = aBounds.w();
        float const f = static_cast<float const>(aFar);
        float const n = static_cast<float const>(aNear);

        float const width  = (r - l);
        float const height = (t - b);
        float const depth  = (f - n);
        float const n2     = (2.0f * n);
        float const fn2    = (f * n2);

        CMatrix4x4 projection = CMatrix4x4({
            (n2 / width),      0.0f,               0.0f,                                       0.0f,
            0.0f,              (n2 / height),      ((t + b) / height),                         0.0f,
            ((r + l) / width), ((t + b) / height), coordinateSystemFactor * ((f + n) / depth), coordinateSystemFactor * 1,
            0.0f,              0.0f,               -(fn2 / depth),                             10.0f
        });
        projection.r11(-projection.r11()); // Invert for vulkan

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
        float const coordinateSystemFactor = ((ECoordinateSystem::LH == aCoordinateSystem) ? 1.0f : -1.0f);

        auto const aspect       =  static_cast<float>(aBounds.size.x()) / static_cast<float>(aBounds.size.y());
        auto const fovFactorRad =  tanf( 0.5f * static_cast<float>(aFovY) );

        auto const f = static_cast<float const>(aFar);
        auto const n = static_cast<float const>(aNear);

        CMatrix4x4 projection = CMatrix4x4({
                (1.0f / (aspect * fovFactorRad)), 0.0f,                  0.0f,                                         0.0f,
                0.0f,                             (1.0f / fovFactorRad), 0.0f,                                         0.0f,
                0.0f,                             0.0f,                  coordinateSystemFactor * ((f) / (f - n)), coordinateSystemFactor * 1,
                0.0f,                             0.0f,                  -((f * n) / (f - n)),                  0.0f
        });
        projection.r11(-projection.r11()); // Invert for vulkan

        return projection;
        // return projectionPerspectiveRect({ l, t, r, b }, aNear, aFar, aCoordinateSystem);
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
        float const f = static_cast<float>(aFar);
        float const n = static_cast<float>(aNear);

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
    void CCamera::createViewMatrix(CTransform const &aTransform, ECoordinateSystem const &aCoordinateSystem)
    {
        CVector3D_t const position = aTransform.translation();
        CVector3D_t const up       = aTransform.up();

        CMatrix4x4 mat = {};

        switch(mViewType)
        {
        case ECameraViewType::TargetCamera:
            mat = lookAt(position, up, mLookAtTarget, aCoordinateSystem);
            break;
        case ECameraViewType::FreeCamera:
            mat = lookTo(position, up, aTransform.forward());
            break;
        }

        mViewMatrix = mat;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    void CCamera::createProjectionMatrix(ECoordinateSystem const &aCoordinateSystem)
    {
        CMatrix4x4 mat = {};

        switch(mProjectionParameters.projectionType)
        {
        case ECameraProjectionType::Perspective:
            mat = projectionPerspectiveRect({
                                (-0.5f * mFrustumParameters.width),
                                (-0.5f * mFrustumParameters.height),
                                ( 0.5f * mFrustumParameters.width),
                                ( 0.5f * mFrustumParameters.height)
                        },
                        static_cast<double>(mFrustumParameters.nearPlaneDistance),
                        static_cast<double>(mFrustumParameters.farPlaneDistance),
                        aCoordinateSystem);
            mat = projectionPerspectiveFOV(
                        { 0, 0, mFrustumParameters.width, mFrustumParameters.height },
                        static_cast<double>(mFrustumParameters.nearPlaneDistance),
                        static_cast<double>(mFrustumParameters.farPlaneDistance),
                        static_cast<double>(mFrustumParameters.fovY),
                        aCoordinateSystem);
            break;
        case ECameraProjectionType::Orthographic:
            mat = projectionOrtho(
                        CVector4D_t({
                            (-0.5f * mFrustumParameters.width),
                            (-0.5f * mFrustumParameters.height),
                            ( 0.5f * mFrustumParameters.width),
                            ( 0.5f * mFrustumParameters.height)
                        }),
                        static_cast<double>(mFrustumParameters.nearPlaneDistance),
                        static_cast<double>(mFrustumParameters.farPlaneDistance),
                        aCoordinateSystem);
            break;
        }

        mProjectionMatrix = mat;
    }
    //<-----------------------------------------------------------------------------
}

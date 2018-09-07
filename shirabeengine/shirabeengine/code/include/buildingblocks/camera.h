#ifndef __SHIRABE_CAMERA_H__
#define __SHIRABE_CAMERA_H__

#include <math/common.h>
#include <math/vector.h>
#include <math/matrix.h>
#include <math/quaternion.h>

#include "buildingblocks/transform.h"

namespace engine
{
    using namespace math;

    /**
     * The ECameraViewType enum describes the various types of scene cameras
     * usable in the scene.
     */
    enum class ECameraViewType
    {
        FreeCamera,  //!< Freestanding camera, which has a fully adjustable transform.
        TargetCamera //!< Camera, whose transform is locked and dependent on a scene entity.
    };

    /**
     * The ECameraProjectionType enum describes the various types of projection
     * usable with scene cameras.
     */
    enum class ECameraProjectionType
    {
        Orthographic,
        Perspective
    };

    /**
     * The ECoordinateSystem enum describes the handedness of the coordinate system used.
     */
    enum class ECoordinateSystem
    {
        LH = 1,
        RH = 2
    };

    /**
     * The CCamera class implements a configurable scene camera, which can be used
     * by the engine.
     */
    class CCamera
    {
    public_structs:
        /**
         * The SFrustumParameters struct describes common parameters required to initialize
         * any camera frustum configuration.
         */
        struct SFrustumParameters
        {
        public_static_functions:
            /**
             * Return a default initialized frustum parameter set.
             *
             * @return See brief.
             */
            static SHIRABE_INLINE SFrustumParameters Default()
            {
                SFrustumParameters p = {};
                p.fovY               = (float)M_PI / 4.0f;
                p.width              = 1;
                p.height             = 1;
                p.nearPlaneDistance  = 0.0f;
                p.farPlaneDistance   = 1.0f;
                return p;
            }

        public_members:
            float    fovY;
            uint16_t width;
            uint16_t height;
            float    nearPlaneDistance;
            float    farPlaneDistance;
        };

        /**
         * The SProjectionParameters struct describes common parameters required to initialize
         * any camera projection configuration.
         */
        struct SProjectionParameters
        {
        public_static_functions:
            /**
             * Return a defualt initialized projection parameter set.
             *
             * @return See brief.
             */
            static SProjectionParameters Default()
            {
                SProjectionParameters p = {};
                p.projectionType        = ECameraProjectionType::Perspective;
                return p;
            }

        public_members:
            ECameraProjectionType projectionType;
        };

    public_constructors:
        /**
         * Default construct an empty CCamera
         */
        CCamera();

        /**
         * Copy construct a camera from another.
         *
         * @param aOther The other camera to copy from.
         */
        CCamera(CCamera const &aOther);
        /**
         * Construct a camera from specific configuration parameters.
         *
         * @param aViewType             View-Type of the camera.
         * @param aFrustumParameters    Frustum-Parameters of the cam. frustum to be created.
         * @param aProjectionParameters Projection-settings of the camera.
         * @param aLookAt               Look-At target in case we have a target camera.
         */
        CCamera(ECameraViewType       const &aViewType,
                SFrustumParameters    const &aFrustumParameters,
                SProjectionParameters const &aProjectionParameters,
                CVector3D_t           const &aLookAt = CVector3D_t({ 0.0f, 0.0f, 0.0f }));

    public_destructors:
        /**
         * Destroy and run...
         */
        ~CCamera();

    public_methods:
        /**
         * Return the assigned view type.
         *
         * @return See brief.
         */
        SHIRABE_INLINE ECameraViewType const &viewType() const { return mViewType; }

        /**
         * Return the assigned frustum parameters.
         *
         * @return See brief.
         */
        SHIRABE_INLINE SFrustumParameters const &frustumParameters() const { return mFrustumParameters; }

        /**
         * Return the assigned projection parameters.
         *
         * @return See brief.
         */
        SHIRABE_INLINE SProjectionParameters const &projectionParameters() const { return mProjectionParameters; }

        /**
         * Return the assigned look at target.
         *
         * @return See brief.
         */
        SHIRABE_INLINE CVector3D_t const &lookAtTarget() const { return mLookAtTarget; }

        /**
         * Return the current world matrix representation of the underlying transform.
         *
         * @return See brief.
         */
        SHIRABE_INLINE CMatrix4x4 const&world() const { return mTransform.world(); }

        /**
         * Return the current view matrix representation of the camera.
         *
         * @return See brief.
         */
        SHIRABE_INLINE CMatrix4x4 const&view() const { return mViewMatrix; }

        /**
         * Return the current projection matrix representation of the camera.
         *
         * @return See brief.
         */
        SHIRABE_INLINE CMatrix4x4 const&projection() const { return mProjectionMatrix; }

    private_methods:
        /**
         * Create a view matrix representation of the current configuration for a given handedness.
         *
         * @param aCoordinateSystem The handedness of the view matrix to be created.
         */
        void createViewMatrix(ECoordinateSystem const &aCoordinateSystem = ECoordinateSystem::RH);

        /**
         * Create a projection matrix representation of the currenct configuration for a given handedness.
         *
         * @param aCoordinateSystem The handedness of the projection matrix to be created.
         */
        void createProjectionMatrix(ECoordinateSystem const&aCoordinateSystem = ECoordinateSystem::RH);

    private_members:
        ECameraViewType       mViewType;
        SFrustumParameters    mFrustumParameters;
        SProjectionParameters mProjectionParameters;
        CVector3D_t           mLookAtTarget;

        CTransform             mTransform;
        CMatrix4x4            mViewMatrix;
        CMatrix4x4            mProjectionMatrix;
    };
}

#endif


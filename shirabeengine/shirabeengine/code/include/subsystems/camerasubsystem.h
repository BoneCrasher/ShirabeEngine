//
// Created by dottideveloper on 06.01.22.
//

#ifndef __SHIRABEDEVELOPMENT_CAMERASUBSYSTEM_H__
#define __SHIRABEDEVELOPMENT_CAMERASUBSYSTEM_H__

#include <base/declaration.h>
#include <platform/platform.h>
#include <core/enginetypehelper.h>
#include "ecws/componentsystem.h"
#include "ecws/components/cameracomponent.h"

namespace engine::subsystems::camera
{
    class SHIRABEENGINE_LIBRARY_EXPORT CCameraSubsystem
    {
    public_typedefs:
        SHIRABE_DECLARE_UID_TYPE(Camera); // CameraUid_t

    private_members:
        Map<CameraUid_t, Shared<class CCamera>> mAvailableCameras;

    public_methods:
        Tuple<CameraUid_t, Shared<class CCamera>> createCamera(
            ECameraViewType                const& aViewType,
            CCamera::SFrustumParameters    const& aFrustumParameters,
            CCamera::SProjectionParameters const& aProjectionParameters,
            CVector3D<float>               const& aLookAt);

        Shared<class CCamera> getCamera(CameraUid_t aCameraUid);

    private_methods:
        SHIRABE_IMPLEMENT_NAMED_UID_PROVIDER(Camera);
    };

    class SHIRABEENGINE_LIBRARY_EXPORT CCameraComponentSystem
        : public ecws::AComponentSystemBase<ecws::CCameraComponent>
    {

    };
}

#endif //__SHIRABEDEVELOPMENT_CAMERASUBSYSTEM_H__

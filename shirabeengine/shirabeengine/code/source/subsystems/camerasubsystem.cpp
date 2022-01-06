//
// Created by dottideveloper on 06.01.22.
//
#include "buildingblocks/camera.h"
#include "subsystems/camerasubsystem.h"

namespace engine::subsystems::camera
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Tuple<CCameraSubsystem::CameraUid_t, Shared<CCamera>>
        CCameraSubsystem::createCamera(
            ECameraViewType                const& aViewType,
            CCamera::SFrustumParameters    const& aFrustumParameters,
            CCamera::SProjectionParameters const& aProjectionParameters,
            CVector3D<float>               const& aLookAt)
    {
        CameraUid_t const  uid = getCameraUid();
        Shared<CCamera> camera = makeShared<CCamera>(aViewType
                                                     , aFrustumParameters
                                                     , aProjectionParameters
                                                     , aLookAt);
        mAvailableCameras.insert({uid, camera});
        return {uid, camera};
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<CCamera> CCameraSubsystem::getCamera(CameraUid_t aCameraUid)
    {
        auto iterator = mAvailableCameras.find(aCameraUid);
        if(mAvailableCameras.end() != iterator)
        {
            return iterator->second;
        }
        return nullptr;
    }
    //<-----------------------------------------------------------------------------
}

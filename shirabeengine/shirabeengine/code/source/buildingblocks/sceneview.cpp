//
// Created by dottideveloper on 30.12.21.
//
#include "buildingblocks/scene.h"
#include "buildingblocks/camera.h"
#include "buildingblocks/sceneview.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CSceneView::CSceneView(Shared<CScene> aScene, Shared<CCamera> aCamera)
        : mScene(std::move(aScene))
        , mCamera(std::move(aCamera))
    {
        assert(nullptr != mScene);
        assert(nullptr != mCamera);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CSceneView::updateSceneView()
    {
        /*
         * This has to happen AFTER physics update and should be one frame off,
         * based on the physics scene update of the previous frame.
         *
         * Steps:
         * 1. Update camera frustum
         * 2. Traverse component tree
         *    a) For each bounded component:
         *       i)  Check if enabled && visible
         *       ii) Check if intersects with frustum
         *       iii) If i) && ii) -> Add to visible elements
         */

        // For now, just pass along all entities from the scene until
        // physics, AABB/OBB/Spheres and frustum culling is implemented.
        CFrustum const &frustum = mCamera->getFrustum();
        

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

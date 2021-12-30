//
// Created by dottideveloper on 30.12.21.
//

#ifndef __SHIRABEDEVELOPMENT_SCENEVIEW_H__
#define __SHIRABEDEVELOPMENT_SCENEVIEW_H__

#include <base/declaration.h>
#include <platform/platform.h>

namespace engine
{
    class SHIRABEENGINE_LIBRARY_EXPORT CSceneView final
    {
    private_members:
        Shared<class CScene>  mScene;
        Shared<class CCamera> mCamera;

    public_constructors:
        CSceneView(Shared<class CScene> aScene, Shared<CCamera> aCamera);

    public_destructors:
        ~CSceneView() = default;

    public_methods:
        EEngineStatus updateSceneView();
    };
}


#endif //__SHIRABEDEVELOPMENT_SCENEVIEW_H__

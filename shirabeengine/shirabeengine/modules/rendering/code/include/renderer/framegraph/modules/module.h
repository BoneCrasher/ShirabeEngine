#ifndef __SHIRABE_FRAMEGRAPH_MODULE_BASE_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_BASE_H__

#include <platform/platform.h>
#include <asset/assetstorage.h>
#include <resources/cresourcemanager.h>
#include <mesh/loader.h>
#include <mesh/declaration.h>
#include <material/loader.h>
#include <material/declaration.h>
#include <textures/loader.h>
#include <textures/declaration.h>

namespace engine
{
    namespace framegraph
    {
        template <typename TTag>
        class CFrameGraphModule;

        // struct SModuleEnvironment
        // {
        //     Shared<asset    ::CAssetStorage>          assetStorage;
        //     Shared<mesh     ::CMeshLoader>            meshLoader;
        //     Shared<material ::CMaterialLoader>        materialLoader;
        //     Shared<textures ::CTextureLoader>         textureLoader;
        // };
    }
}

#endif

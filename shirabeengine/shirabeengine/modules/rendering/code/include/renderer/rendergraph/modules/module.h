#ifndef __SHIRABE_FRAMEGRAPH_MODULE_BASE_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_BASE_H__

#include <platform/platform.h>
#include <asset/assetstorage.h>
#include <asset/mesh/loader.h>
#include <asset/mesh/declaration.h>
#include "../../../../../../../code/include/materialsystem/loader.h"
#include "../../../../../../../code/include/materialsystem/declaration.h"
#include <asset/textures/loader.h>
#include <asset/textures/declaration.h>

#include "renderer/resource_management/cresourcemanager.h"

namespace engine
{
    namespace framegraph
    {
        template <typename TTag>
        class CRenderGraphModule;

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

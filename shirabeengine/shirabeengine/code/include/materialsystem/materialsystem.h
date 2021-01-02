//
// Created by dottideveloper on 05.06.20.
//

#ifndef __SHIRABEDEVELOPMENT_MATERIALSYSTEM_H__
#define __SHIRABEDEVELOPMENT_MATERIALSYSTEM_H__

#include <math/matrix.h>
#include <math/vector.h>
#include <core/enginetypehelper.h>
#include <core/memory/memorymanager.h>
#include <core/enginestatus.h>
#include <asset/assettypes.h>
#include <asset/material/asset.h>
#include "materialsystem/declaration.h"

namespace engine
{
    namespace asset
    {
        using AssetID_t = uint64_t;
        class IAssetStorage;
    }

    using namespace material;

    class CMaterialSystem
    {
        SHIRABE_DECLARE_LOG_TAG(CMaterialSystem)

    public_constructors:
        CMaterialSystem() = default;
        CMaterialSystem(CMaterialSystem const &) = delete;
        CMaterialSystem(CMaterialSystem &&)      = delete;

    public_destructors:
        ~CMaterialSystem() = default;

    public_operators:
        CMaterialSystem& operator=(CMaterialSystem const &) = delete;
        CMaterialSystem& operator=(CMaterialSystem &&)      = delete;

    public_methods:
        EEngineStatus initialize(Unique<memory::CMemoryManager> &aMemoryManager);
        EEngineStatus deinitialize();

        CEngineResult <Shared<CMaterialInstance>> getInstance(Shared<asset::IAssetStorage> const &aAssetStorage
                                                              , asset::AssetID_t           const &aMaterialInstanceAssetId
                                                              , bool                              aAutoCreateConfiguration
                                                              , bool                              aIncludeSystemBuffers = false);

    private_members:
        Shared<memory::allocators::CPoolAllocator>   mTransformAllocator;
        Shared<memory::allocators::CLinearAllocator> mDataAllocator;

        Map <asset::AssetID_t, Shared<CSharedMaterial>> mSharedMaterials;
        Map <std::string, Shared<CMaterialInstance>>    mMaterialInstances;
    };
}

#endif //__SHIRABEDEVELOPMENT_MATERIALSYSTEM_H__

//
// Created by dottideveloper on 07.06.20.
//
#include <core/helpers.h>
#include <asset/assetstorage.h>
#include <asset/material/serialization.h>
#include <util/crc32.h>
#include <util/documents/json.h>

#include "materialsystem/materialsystem.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CMaterialSystem::initialize(Unique<memory::CMemoryManager> &aMemoryManager)
    {
        static constexpr std::size_t const sTransformBufferSize      = (4096 * sizeof(math::CVector4D<float>));
        static constexpr std::size_t const sTransformBufferAlignment = 16;
        static constexpr std::size_t const sDataBufferSize           = (64 * 1024 * 1024);
        static constexpr std::size_t const sDataBufferAlignment      = 16;

        mTransformAllocator
            = aMemoryManager->getAllocator<memory::allocators::CPoolAllocator>(sTransformBufferSize
                                                                               , sTransformBufferAlignment
                                                                               , sizeof(math::CVector4D<float>)
                                                                               , sTransformBufferAlignment);
        mDataAllocator
            = aMemoryManager->getAllocator<memory::allocators::CLinearAllocator>(sDataBufferSize
                                                                                 , sDataBufferAlignment);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CMaterialSystem::deinitialize()
    {
        mDataAllocator->clear();
        mDataAllocator      = nullptr;
        mTransformAllocator = nullptr;

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<SMaterialAsset> readMaterialAsset(std::string const &aLogTag, Shared<asset::IAssetStorage> const &aAssetStorage, asset::AssetId_t const &aAssetUID)
    {
        return serialization::readMaterialFile<SMaterialAsset>(aLogTag, aAssetStorage, aAssetUID);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<Shared<CMaterialInstance>> CMaterialSystem::getInstance(Shared<asset::IAssetStorage> const &aAssetStorage
                                                                          , asset::AssetID_t           const &aMaterialInstanceAssetId
                                                                          , bool                              aAutoCreateConfiguration
                                                                          , bool                              aIncludeSystemBuffers)
    {
        using namespace asset;

        //--------------------------------------------------------------------------------------------------------------------
        // Fetch instance data
        //--------------------------------------------------------------------------------------------------------------------
        AssetID_t instanceIndexAssetId = aMaterialInstanceAssetId;
        if(0_uid == instanceIndexAssetId)
        {
            return { EEngineStatus::Error };
        }

        //--------------------------------------------------------------------------------------------------------------------
        // Fetch sharedMaterial data
        //--------------------------------------------------------------------------------------------------------------------

        Shared<CSharedMaterial> sharedMaterial = nullptr;

        if(mSharedMaterials.end() != mSharedMaterials.find(aMaterialInstanceAssetId))
        {
            sharedMaterial = mSharedMaterials.at(aMaterialInstanceAssetId);
        }
        else
        {
            auto const [assetDataFetchResult, assetData] = readMaterialAsset(logTag(), aAssetStorage, aMaterialInstanceAssetId);
            {
                PrintEngineError(assetDataFetchResult, logTag(), "Could not fetch sharedMaterial asset data.");
                SHIRABE_RETURN_RESULT_ON_ERROR(assetDataFetchResult)
            }

            sharedMaterial = CSharedMaterial::fromAsset(assetData);
            sharedMaterial->initializeMemory(mDataAllocator);

            mSharedMaterials[assetData.uid] = sharedMaterial;
        }

        if(nullptr == sharedMaterial)
        {
            return { EEngineStatus::Error, nullptr };
        }

        static uint64_t sInstanceIndex = 0;
        std::string instanceName = fmt::format("{}_instance_{}", sharedMaterial->name(), ++sInstanceIndex);

        Shared<CMaterialInstance> instance = makeShared<CMaterialInstance>(instanceName, sharedMaterial);
        // if(aAutoCreateConfiguration)
        // {
            instance->createConfiguration(*sharedMaterial, aIncludeSystemBuffers);
        // }

        mMaterialInstances.insert({instanceName, instance });

        return { EEngineStatus::Ok, instance };
    }
    //<-----------------------------------------------------------------------------
}

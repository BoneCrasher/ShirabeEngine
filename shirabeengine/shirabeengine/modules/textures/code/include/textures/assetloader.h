//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_TEXTURE_ASSETLOADER_H__
#define __SHIRABEDEVELOPMENT_TEXTURE_ASSETLOADER_H__

#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <resources/cresourcemanager.h>
#include "textures/declaration.h"
#include "textures/loader.h"

namespace engine::textures
{
    using namespace resources;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    static Shared<CResourceFromAssetResourceObjectCreator<STexture>> getAssetLoader(Shared<resources::CResourceManager> const &aResourceManager
                                                                                   , Shared<asset::IAssetStorage>       const &aAssetStorage
                                                                                   , Shared<textures::CTextureLoader>   const &aLoader)
    {
        auto const loader = [=] (ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject>
        {
            auto const &[result, instance] = aLoader->loadInstance(aAssetStorage, aAssetId);
            if(CheckEngineError(result))
            {
                return nullptr;
            }

            Vector<DataSourceAccessor_t> initialData {};
            for(auto const &assetUid : instance->imageLayersBinaryAssetUids())
            {
                DataSourceAccessor_t dataAccessor = [=]() -> ByteBuffer
                {
                    asset::AssetID_t const uid = assetUid; // asset::assetIdFromUri(stageSpirVFilename);

                    auto const[result, buffer] = aAssetStorage->loadAssetData(uid);
                    if( CheckEngineError(result))
                    {
                        CLog::Error("DataSourceAccessor_t::Texture", "Failed to load texture asset data. Result: {}", result);
                        return {};
                    }

                    return buffer;
                };

                initialData.push_back(dataAccessor);
            }

            STextureDescription textureDescription {};
            textureDescription.name        = instance->name();
            textureDescription.textureInfo = instance->textureInfo();
            textureDescription.cpuGpuUsage = EResourceUsage::CPU_InitConst_GPU_Read;
            textureDescription.gpuBinding  = EBufferBinding::TextureInput;
            textureDescription.initialData = initialData;

            CEngineResult<Shared<ILogicalResourceObject>> textureObject = aResourceManager->useDynamicResource<STexture>(textureDescription.name, textureDescription);
            EngineStatusPrintOnError(textureObject.result(),  "Texture::AssetLoader", "Failed to load texture.");

            return textureObject.data();
        };

        return makeShared<CResourceFromAssetResourceObjectCreator<STexture>>(loader);
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__

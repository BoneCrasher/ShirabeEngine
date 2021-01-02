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

namespace engine::resources
{
    using namespace asset;
    using namespace textures;

    template<>
    class CResourceCreator<CTextureInstance, STextureDescription>
    {
    public:
        static STextureDescription adapt(Shared <CAssetStorage> aAssetStorage, std::shared_ptr <TResource> aTextureInstance)
        {
            DataSourceAccessor_t initialData = [=]() -> ByteBuffer
            {
                asset::AssetID_t const uid = aTextureInstance->imageLayersBinaryAssetUid();

                auto const[result, buffer] = aAssetStorage->loadAssetData(uid);
                if( CheckEngineError(result))
                {
                    CLog::Error("DataSourceAccessor_t::Texture", "Failed to load texture asset data. Result: {}", result);
                    return {};
                }

                return buffer;
            };

            using core::operator|; // bitwise concat of enum class...

            STextureDescription textureDescription {};
            textureDescription.name        = aTextureInstance->name();
            textureDescription.textureInfo = aTextureInstance->textureInfo();
            textureDescription.cpuGpuUsage = EResourceUsage::CPU_InitConst_GPU_Read;
            textureDescription.gpuBinding  = EBufferBinding::TextureInput | EBufferBinding::CopyTarget;
            textureDescription.gpuBinding.set(EBufferBinding::CopyTarget);

            textureDescription.initialData = { initialData };

            return textureDescription;
        }
    };
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__

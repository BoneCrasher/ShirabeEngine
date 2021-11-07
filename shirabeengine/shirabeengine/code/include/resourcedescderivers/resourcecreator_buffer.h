//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_BUFFERS_ASSETLOADER_H__
#define __SHIRABEDEVELOPMENT_BUFFERS_ASSETLOADER_H__

#include <asset/assetstorage.h>
#include <rhi/resource_management/resourcedescriptions.h>
#include <rhi/resource_management/resourcetypes.h>
#include <rhi/resource_management/cresourcemanager.h>
#include <asset/buffers/loader.h>
#include <asset/buffers/declaration.h>

namespace engine
{
    namespace buffers
    {
        using namespace resources;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        using ResourceDescriptionDerivationReturn_t = std::tuple<bool, resources::SRHIBufferDescription>;

        static
        ResourceDescriptionDerivationReturn_t deriveResourceDescriptions(Shared<asset::IAssetStorage> const &aAssetStorage
                                                                        , std::string                const &aBufferName)
        {
        };
    }

    namespace resources
    {
        using namespace asset;
        using namespace buffers;

        template<>
        class CResourceDescDeriver<CBufferInstance, SBufferDescriptor>
        {
        public:
            static SBufferDescriptor derive(Shared<CAssetStorage> aAssetStorage, Shared<CBufferInstance> aInstance)
            {
               return {};
            }
        };
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__

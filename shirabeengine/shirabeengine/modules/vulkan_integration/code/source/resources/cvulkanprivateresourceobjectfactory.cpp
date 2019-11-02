//
// Created by dottideveloper on 20.10.19.
//

#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"

#include "vulkan_integration/resources/cvulkanprivateresourceobjectfactory.h"

namespace engine
{
    namespace vulkan
    {
        using namespace graphicsapi;
        using namespace resources;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TLogicalResource, typename TGpuApiResource>
        static Unique<CResourceObjectCreator<TLogicalResource>> makeCreator()
        {
            return makeUnique<CResourceObjectCreator<TLogicalResource>>(
                    [] (typename TLogicalResource::Descriptor_t const &aDescription)
                    {
                        return makeUnique<TGpuApiResource>(aDescription);
                    });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanPrivateResourceObjectFactory::initialize()
        {
            // Register all supported resource types w/ their creator.
            setCreatorForType<CVulkanBufferResource>     (std::move(makeCreator<SBuffer,      CVulkanBufferResource>()));
            setCreatorForType<CVulkanBufferViewResource> (std::move(makeCreator<SBufferView,  CVulkanBufferViewResource>()));
            setCreatorForType<CVulkanTextureResource>    (std::move(makeCreator<STexture,     CVulkanTextureResource>()));
            setCreatorForType<CVulkanTextureViewResource>(std::move(makeCreator<STextureView, CVulkanTextureViewResource>()));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanPrivateResourceObjectFactory::deinitialize()
        {
            removeAllCreators();

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

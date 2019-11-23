//
// Created by dottideveloper on 20.10.19.
//

#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanmaterialpipelineresource.h"

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
        static Unique<CResourceObjectCreator<TLogicalResource>> makeCreator(Shared<IVkGlobalContext> aContext)
        {
            return makeUnique<CResourceObjectCreator<TLogicalResource>>(
                    [aContext] (GpuApiHandle_t const &aHandle, typename TLogicalResource::Descriptor_t const &aDescription)
                    {
                        Unique<TGpuApiResource> resource = makeUnique<TGpuApiResource>(aContext, aHandle, aDescription);
                        return resource;
                    });
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceObjectFactory::initialize(Shared<IVkGlobalContext> aVkContext)
        {
            mVkContext = std::move(aVkContext);

            // Register all supported resource types w/ their creator.
            setCreatorForType<SBuffer>     (std::move(makeCreator<SBuffer,      CVulkanBufferResource>     (mVkContext)));
            setCreatorForType<SBufferView> (std::move(makeCreator<SBufferView,  CVulkanBufferViewResource> (mVkContext)));
            setCreatorForType<STexture>    (std::move(makeCreator<STexture,     CVulkanTextureResource>    (mVkContext)));
            setCreatorForType<STextureView>(std::move(makeCreator<STextureView, CVulkanTextureViewResource>(mVkContext)));
            setCreatorForType<SFrameBuffer>(std::move(makeCreator<SFrameBuffer, CVulkanFrameBufferResource>(mVkContext)));
            setCreatorForType<SRenderPass> (std::move(makeCreator<SRenderPass,  CVulkanRenderPassResource> (mVkContext)));
            setCreatorForType<SPipeline>   (std::move(makeCreator<SPipeline,    CVulkanPipelineResource>   (mVkContext)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceObjectFactory::deinitialize()
        {
            removeAllCreators();

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

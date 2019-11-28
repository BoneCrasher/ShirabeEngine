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
        static Unique<CResourceGpuApiResourceObjectCreator<TLogicalResource>> makeCreator(Shared<IVkGlobalContext> aContext)
        {
            return makeUnique<CResourceGpuApiResourceObjectCreator<TLogicalResource>>(
                    [aContext] (GpuApiHandle_t const &aHandle) -> Shared<TGpuApiResource>
                    {
                        Shared<TGpuApiResource> resource = makeShared<TGpuApiResource>(aContext, aHandle);
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

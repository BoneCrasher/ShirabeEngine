//
// Created by dottideveloper on 20.10.19.
//

#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanshadermoduleresource.h"
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
                [aContext] (GpuApiHandle_t const &aHandle) -> std::tuple<Shared<TGpuApiResource>, SGpuApiOps<TLogicalResource>>
                {
                    Shared<TGpuApiResource> resource = makeShared<TGpuApiResource>(aContext, aHandle);

                    SGpuApiOps<TLogicalResource> ops {};
                    ops.initialize   = [resource] ( typename TLogicalResource::Descriptor_t   const &aDescriptor
                                                  , typename TLogicalResource::Dependencies_t const &aDependencies
                                                  , GpuApiResourceDependencies_t              const &aResolvedDependencies) -> CEngineResult<> {
                        return resource->create(aDescriptor, aDependencies, aResolvedDependencies);
                    };
                    ops.deinitialize = [resource] () -> CEngineResult<> {
                        return resource->destroy();
                    };

                    if constexpr(std::is_base_of_v<ILoadableGpuApiResourceObject, TGpuApiResource>)
                    {
                        ops.load   = [resource] () -> CEngineResult<> { return resource->load();   };
                        ops.unload = [resource] () -> CEngineResult<> { return resource->unload(); };
                    }

                    if constexpr(std::is_base_of_v<ITransferrableGpuApiResourceObject, TGpuApiResource>)
                    {
                        ops.transfer = [resource] () -> CEngineResult<> { return resource->transfer(); };
                    }

                    return { resource, ops };
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
            setCreatorForType<SBuffer>      (std::move(makeCreator<SBuffer,       CVulkanBufferResource>      (mVkContext)));
            setCreatorForType<SBufferView>  (std::move(makeCreator<SBufferView,   CVulkanBufferViewResource>  (mVkContext)));
            setCreatorForType<STexture>     (std::move(makeCreator<STexture,      CVulkanTextureResource>     (mVkContext)));
            setCreatorForType<STextureView> (std::move(makeCreator<STextureView,  CVulkanTextureViewResource> (mVkContext)));
            setCreatorForType<SFrameBuffer> (std::move(makeCreator<SFrameBuffer,  CVulkanFrameBufferResource> (mVkContext)));
            setCreatorForType<SRenderPass>  (std::move(makeCreator<SRenderPass,   CVulkanRenderPassResource>  (mVkContext)));
            setCreatorForType<SShaderModule>(std::move(makeCreator<SShaderModule, CVulkanShaderModuleResource>(mVkContext)));
            setCreatorForType<SPipeline>    (std::move(makeCreator<SPipeline,     CVulkanPipelineResource>    (mVkContext)));

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

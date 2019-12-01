#ifndef __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__
#define __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__


#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/agpuapiresourceobject.h>
#include "vulkan_integration/resources/cvkapiresource.h"

namespace engine
{
    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        class CVulkanPipelineResource
                : public CVkApiResource<SPipeline>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanPipelineResource);

        public_constructors:
            using CVkApiResource<SPipeline>::CVkApiResource;

        public_methods:
            CEngineResult<> create(  SMaterialPipelineDescriptor   const &aDescription
                                   , SMaterialPipelineDependencies const &aDependencies
                                   , GpuApiResourceDependencies_t  const &aResolvedDependencies) final;
            CEngineResult<> destroy()  final;

        public_members:
            VkPipeline                   pipeline;
            VkPipelineLayout             pipelineLayout;
            VkDescriptorPool             descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
        };
    }
}

#endif

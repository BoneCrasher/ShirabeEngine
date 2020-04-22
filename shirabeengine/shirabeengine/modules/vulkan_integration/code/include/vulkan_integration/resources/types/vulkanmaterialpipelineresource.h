#ifndef __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__
#define __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__


#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/extensibility.h>

namespace engine
{
    namespace vulkan
    {
        struct SVulkanPipelineResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SPipeline> { using TGpuApiResource = struct vulkan::SVulkanPipelineResource; };
    }

    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanPipelineResource
        {
            struct Handles_t
            {
                VkPipeline                   pipeline;
                VkPipelineLayout             pipelineLayout;
                VkDescriptorPool             descriptorPool;
                std::vector<VkDescriptorSet> descriptorSets;
            };
        };

        using PipelineResourceState_t = SResourceState<SPipeline>;
    }
}

#endif

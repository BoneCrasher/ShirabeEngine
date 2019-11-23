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
                : public CVkApiResource<SMaterialPipelineDescriptor>
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanPipelineResource);

        public_constructors:
            using CVkApiResource<SMaterialPipelineDescriptor>::CVkApiResource;

        public_methods:
            CEngineResult<> create(GpuApiResourceDependencies_t const &aDependencies) final;
            CEngineResult<> destroy()  final;

        public_members:
            VkViewport                                                   viewPort;
            VkRect2D                                                     scissor;

            VkPipelineInputAssemblyStateCreateInfo                       inputAssemblyState;
            std::vector<VkVertexInputBindingDescription>                 vertexInputBindings;
            std::vector<VkVertexInputAttributeDescription>               vertexInputAttributes;

            std::vector<VkDescriptorSetLayoutCreateInfo>                 descriptorSetLayoutCreateInfos;
            std::vector<std::vector<VkDescriptorSetLayoutBinding>>       descriptorSetLayoutBindings;
            std::unordered_map<VkShaderStageFlags, DataSourceAccessor_t> shaderStages;

            VkPipelineRasterizationStateCreateInfo                       rasterizerState;
            VkPipelineMultisampleStateCreateInfo                         multiSampler;
            VkPipelineDepthStencilStateCreateInfo                        depthStencilState;
            std::vector<VkPipelineColorBlendAttachmentState>             colorBlendAttachmentStates;
            VkPipelineColorBlendStateCreateInfo                          colorBlendState;

            VkPipelineLayoutCreateInfo                                   pipelineLayout;
            uint32_t                                                     subpass;


            VkPipeline                   pipeline;
            std::vector<VkShaderModule>  shaderModules;
            VkDescriptorPool             descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;
        };
    }
}

#endif

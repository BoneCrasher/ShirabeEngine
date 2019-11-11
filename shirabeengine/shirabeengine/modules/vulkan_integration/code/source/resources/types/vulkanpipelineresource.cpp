//
// Created by dottideveloper on 29.10.19.
//
#include <material/materialserialization.h>
#include "vulkan_integration/resources/types/vulkanpipelineresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanPipelineResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
        VkDevice device = getVkContext()->getLogicalDevice();

        SPipelineDescription const &desc = getDescription();

        CVulkanRenderPassResource const *const renderPass = getVkContext()->getResourceStorage()->extract(aDependencies.at(desc.referenceRenderPassId));

        // Create the shader modules here...
        // IMPORTANT: The task backend receives access to the asset system to not have the raw data stored in the descriptors and requests...

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
        vertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.pNext                           = nullptr;
        vertexInputStateCreateInfo.flags                           = 0;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0; // static_cast<uint32_t>(desc.vertexInputAttributes.size());
        vertexInputStateCreateInfo.vertexBindingDescriptionCount   = 0; // static_cast<uint32_t>(desc.vertexInputBindings  .size());
        vertexInputStateCreateInfo.pVertexAttributeDescriptions    = nullptr; // desc.vertexInputAttributes.data();
        vertexInputStateCreateInfo.pVertexBindingDescriptions      = nullptr; // desc.vertexInputBindings  .data();

        VkRect2D scissor {};
        scissor.offset = { 0, 0 };
        scissor.extent = { (uint32_t)desc.viewPort.width, (uint32_t)desc.viewPort.height };

        VkPipelineViewportStateCreateInfo viewPortStateCreateInfo {};
        viewPortStateCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewPortStateCreateInfo.pNext          = nullptr;
        viewPortStateCreateInfo.flags          = 0;
        viewPortStateCreateInfo.viewportCount  = 1;
        viewPortStateCreateInfo.pViewports     = &(desc.viewPort);
        viewPortStateCreateInfo.scissorCount   = 1;
        viewPortStateCreateInfo.pScissors      = &(scissor);

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        std::vector<VkShaderModule>                  vkShaderModules;

        for(auto const &[stage, dataAccessor] : desc.shaderStages)
        {
            if(not dataAccessor)
            {
                // If we don't have any data to access, then don't create a shader module.
                continue;
            }

            ByteBuffer const data = dataAccessor();

            // We need to convert from a regular 8-bit data buffer to uint32 words of SPIR-V.
            // TODO: Refactor the asset system to permit loading 32-bit buffers...
            std::vector<signed char> const &srcData      = data.dataVector();
            uint32_t const                  srcDataSize  = srcData.size();

            std::vector<uint32_t> convData {};
            convData.resize( srcDataSize / 4 );

            for(uint32_t k=0; k<srcDataSize; k += 4)
            {
                uint32_t const value = *reinterpret_cast<uint32_t const*>( srcData.data() + k );
                convData[ k / 4 ] = value;
            }

            VkShaderModuleCreateInfo shaderModuleCreateInfo {};
            shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            shaderModuleCreateInfo.pNext    = nullptr;
            shaderModuleCreateInfo.flags    = 0;
            shaderModuleCreateInfo.pCode    = convData.data();
            shaderModuleCreateInfo.codeSize = srcData.size();

            VkShaderModule vkShaderModule = VK_NULL_HANDLE;
            VkResult const moduleCreationResult = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &vkShaderModule);
            if(VkResult::VK_SUCCESS != moduleCreationResult)
            {
                CLog::Error(logTag(), "Failed to create shader module for stage %0", stage);
                continue;
            }
            vkShaderModules.push_back(vkShaderModule);

            VkPipelineShaderStageCreateInfo shaderStageCreateInfo {};
            shaderStageCreateInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCreateInfo.pNext               = nullptr;
            shaderStageCreateInfo.flags               = 0;
            shaderStageCreateInfo.pName               = "main";
            shaderStageCreateInfo.stage               = serialization::shaderStageFromPipelineStage(stage);
            shaderStageCreateInfo.module              = vkShaderModule;
            shaderStageCreateInfo.pSpecializationInfo = nullptr;

            shaderStages.push_back(shaderStageCreateInfo);
        }

        std::unordered_map<VkDescriptorType, uint32_t> typesAndSizes {};
        std::vector<VkDescriptorSetLayout>             setLayouts    {};

        for(uint64_t k=0; k<desc.descriptorSetLayoutCreateInfos.size(); ++k)
        {
            std::vector<VkDescriptorSetLayoutBinding> bindings = desc.descriptorSetLayoutBindings[k];
            VkDescriptorSetLayoutCreateInfo           info     = desc.descriptorSetLayoutCreateInfos[k];

            info.pBindings    = bindings.data();
            info.bindingCount = bindings.size();

            VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
            {
                VkResult const result = vkCreateDescriptorSetLayout(device, &info, nullptr, &vkDescriptorSetLayout);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), "Failed to create pipeline descriptor set layout. Result %0", result);
                    return {EEngineStatus::Error};
                }

                setLayouts.push_back(vkDescriptorSetLayout);
            }

            for(auto const &binding : bindings)
            {
                typesAndSizes[binding.descriptorType] += binding.descriptorCount;
            }
        }

        std::vector<VkDescriptorPoolSize> poolSizes {};
        auto const addPoolSizeFn = [&poolSizes] (VkDescriptorType const &aType, std::size_t const &aSize)
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type            = aType;
            poolSize.descriptorCount = static_cast<uint32_t>(aSize);

            poolSizes.push_back(poolSize);
        };

        for(auto const &[type, size] : typesAndSizes)
        {
            addPoolSizeFn(type, size);
        }

        VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo = {};
        vkDescriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        vkDescriptorPoolCreateInfo.pNext         = nullptr;
        vkDescriptorPoolCreateInfo.flags         = 0;
        vkDescriptorPoolCreateInfo.maxSets       = desc.descriptorSetLayoutCreateInfos.size();
        vkDescriptorPoolCreateInfo.poolSizeCount = poolSizes.size();
        vkDescriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();

        VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
        {
            VkResult const result = vkCreateDescriptorPool(device, &vkDescriptorPoolCreateInfo, nullptr, &vkDescriptorPool);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), "Could not create descriptor pool.");
                return { EEngineStatus::Error };
            }
        }

        VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo = {};
        vkDescriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        vkDescriptorSetAllocateInfo.pNext              = nullptr;
        vkDescriptorSetAllocateInfo.descriptorPool     = vkDescriptorPool;
        vkDescriptorSetAllocateInfo.descriptorSetCount = setLayouts.size();
        vkDescriptorSetAllocateInfo.pSetLayouts        = setLayouts.data();

        std::vector<VkDescriptorSet> vkDescriptorSets {};
        {
            vkDescriptorSets.resize(setLayouts.size());

            VkResult const result = vkAllocateDescriptorSets(device, &vkDescriptorSetAllocateInfo, vkDescriptorSets.data());
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), "Could not create descriptor sets.");
                return { EEngineStatus::Error };
            }
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
        pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext                  = nullptr;
        pipelineLayoutCreateInfo.flags                  = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges    = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts            = setLayouts.data();
        pipelineLayoutCreateInfo.setLayoutCount         = setLayouts.size();

        VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
        {
            VkResult const result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &vkPipelineLayout);
            if( VkResult::VK_SUCCESS!=result )
            {
                CLog::Error(logTag(), "Failed to create pipeline layout. Result %0", result);
                return {EEngineStatus::Error};
            }
        }

        VkPipelineColorBlendStateCreateInfo colorBlending = desc.colorBlendState;
        colorBlending.pAttachments    = desc.colorBlendAttachmentStates.data();
        colorBlending.attachmentCount = desc.colorBlendAttachmentStates.size();

        VkGraphicsPipelineCreateInfo pipelineCreateInfo {};
        pipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext               = nullptr;
        pipelineCreateInfo.flags               = 0;
        pipelineCreateInfo.pVertexInputState   = &(vertexInputStateCreateInfo);
        pipelineCreateInfo.pInputAssemblyState = &(desc.inputAssemblyState);
        pipelineCreateInfo.pViewportState      = &(viewPortStateCreateInfo);
        pipelineCreateInfo.pRasterizationState = &(desc.rasterizerState);
        pipelineCreateInfo.pMultisampleState   = &(desc.multiSampler);
        pipelineCreateInfo.pDepthStencilState  = &(desc.depthStencilState);
        pipelineCreateInfo.pColorBlendState    = &(colorBlending);
        pipelineCreateInfo.pDynamicState       = nullptr;
        pipelineCreateInfo.pTessellationState  = nullptr;
        pipelineCreateInfo.layout              = vkPipelineLayout;
        pipelineCreateInfo.renderPass          = renderPass->handle;
        pipelineCreateInfo.subpass             = desc.subpass;
        pipelineCreateInfo.stageCount          = shaderStages.size();
        pipelineCreateInfo.pStages             = shaderStages.data();

        VkPipeline pipeline = VK_NULL_HANDLE;
        {
            VkResult const result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
            if( VkResult::VK_SUCCESS!=result )
            {
                CLog::Error(logTag(), "Failed to create pipeline. Result %0", result);
                return {EEngineStatus::Error};
            }
        }

        this->pipeline       = pipeline;
        this->shaderModules  = vkShaderModules;
        this->descriptorPool = vkDescriptorPool;
        this->descriptorSets = vkDescriptorSets;

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanPipelineResource::destroy()
    {
        VkDevice device = getVkContext()->getLogicalDevice();

        vkDestroyDescriptorPool(device, this->descriptorPool, nullptr);
        for(auto const &module : this->shaderModules)
        {
            vkDestroyShaderModule(device, module, nullptr);
        }

        vkDestroyPipeline(device, this->pipeline, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

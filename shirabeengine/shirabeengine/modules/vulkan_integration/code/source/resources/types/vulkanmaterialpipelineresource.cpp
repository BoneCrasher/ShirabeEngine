//
// Created by dottideveloper on 29.10.19.
//
#include <material/serialization.h>
#include "vulkan_integration/resources/types/vulkanmaterialpipelineresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/resources/types/vulkanshadermoduleresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanPipelineResource::create(  SMaterialPipelineDescriptor   const &aDescription
                                                    , SMaterialPipelineDependencies const &aDependencies
                                                    , GpuApiResourceDependencies_t  const &aResolvedDependencies)
    {
        if(EGpuApiResourceState::Loaded == getResourceState()
           || EGpuApiResourceState::Loading == getResourceState())
        {
            return EEngineStatus::Ok;
        }

        setResourceState(EGpuApiResourceState::Loading);

        CVkApiResource<SPipeline>::create(aDescription, aDependencies, aResolvedDependencies);

        VkDevice device = getVkContext()->getLogicalDevice();

        CVulkanPipelineResource const *systemUBOPipeline = nullptr;
        if(aResolvedDependencies.end() != aResolvedDependencies.find(aDependencies.systemUBOPipelineId))
        {
            systemUBOPipeline = getVkContext()->getResourceStorage()->extract<CVulkanPipelineResource>(aResolvedDependencies.at(aDependencies.systemUBOPipelineId));
        }

        auto const *const renderPass = getVkContext()->getResourceStorage()->extract<CVulkanRenderPassResource>(aResolvedDependencies.at(aDependencies.referenceRenderPassId));

        // Create the shader modules here...
        // IMPORTANT: The task backend receives access to the asset system to not have the raw data stored in the descriptors and requests...

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
        vertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.pNext                           = nullptr;
        vertexInputStateCreateInfo.flags                           = 0;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(aDescription.vertexInputAttributes.size());
        vertexInputStateCreateInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(aDescription.vertexInputBindings  .size());
        vertexInputStateCreateInfo.pVertexAttributeDescriptions    = aDescription.vertexInputAttributes.data();
        vertexInputStateCreateInfo.pVertexBindingDescriptions      = aDescription.vertexInputBindings  .data();

        VkRect2D scissor {};
        scissor.offset = { 0, 0 };
        scissor.extent = { (uint32_t)aDescription.viewPort.width, (uint32_t)aDescription.viewPort.height };

        VkPipelineViewportStateCreateInfo viewPortStateCreateInfo {};
        viewPortStateCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewPortStateCreateInfo.pNext          = nullptr;
        viewPortStateCreateInfo.flags          = 0;
        viewPortStateCreateInfo.viewportCount  = 1;
        viewPortStateCreateInfo.pViewports     = &(aDescription.viewPort);
        viewPortStateCreateInfo.scissorCount   = 1;
        viewPortStateCreateInfo.pScissors      = &(scissor);

        std::unordered_map<VkDescriptorType, uint32_t> typesAndSizes    {};
        std::vector<VkDescriptorSetLayout>             setLayouts       {};
        std::vector<VkDescriptorSetLayout>             createdSetLayouts{};

        auto const createDescriptorSetLayouts = [&device, &typesAndSizes, &setLayouts, &createdSetLayouts] (SMaterialPipelineDescriptor const &aDescriptor, bool aRegisterForDescriptorSetAlloc) -> EEngineStatus
        {
            for(uint64_t k=0; k<aDescriptor.descriptorSetLayoutCreateInfos.size(); ++k)
            {
                std::vector<VkDescriptorSetLayoutBinding> bindings = aDescriptor.descriptorSetLayoutBindings[k];
                VkDescriptorSetLayoutCreateInfo           info     = aDescriptor.descriptorSetLayoutCreateInfos[k];

                info.pBindings    = bindings.data();
                info.bindingCount = bindings.size();

                VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;
                {
                    VkResult const result = vkCreateDescriptorSetLayout(device, &info, nullptr, &vkDescriptorSetLayout);
                    if( VkResult::VK_SUCCESS != result )
                    {
                        CLog::Error(logTag(), "Failed to create pipeline descriptor set layout. Result {}", result);
                        return EEngineStatus::Error;
                    }

                    setLayouts.push_back(vkDescriptorSetLayout);
                }

                if(aRegisterForDescriptorSetAlloc)
                {
                    createdSetLayouts.push_back(vkDescriptorSetLayout);

                    for( auto const &binding : bindings )
                    {
                        typesAndSizes[binding.descriptorType] += binding.descriptorCount;
                    }
                }
            }

            return EEngineStatus::Ok;
        };

        //
        // First of all, push back all system UBOs...
        //
        if(nullptr != systemUBOPipeline)
        {
            // We are creating the system UBO, nothing to add...
            SMaterialPipelineDescriptor const &systemUBODescriptor = systemUBOPipeline->getCurrentDescriptor().value();
            EEngineStatus const status = createDescriptorSetLayouts(systemUBODescriptor, false);
            //if(CheckEngineError(status))
            //{
            //    return { EEngineStatus::Error };
            //}
        }

        //
        // Then everything else...
        //
        EEngineStatus const status = createDescriptorSetLayouts(aDescription, true);
        if(CheckEngineError(status))
        {
            return { EEngineStatus::Error };
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
        vkDescriptorPoolCreateInfo.maxSets       = createdSetLayouts.size();
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
        vkDescriptorSetAllocateInfo.descriptorSetCount = createdSetLayouts.size();
        vkDescriptorSetAllocateInfo.pSetLayouts        = createdSetLayouts.data();

        std::vector<VkDescriptorSet> vkCreatedDescriptorSets {};
        {
            vkCreatedDescriptorSets.resize(createdSetLayouts.size());

            VkResult const result = vkAllocateDescriptorSets(device, &vkDescriptorSetAllocateInfo, vkCreatedDescriptorSets.data());
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), "Could not create descriptor sets.");
                return { EEngineStatus::Error };
            }
        }

        std::vector<VkDescriptorSet> vkDescriptorSets {};
        if(nullptr != systemUBOPipeline)
        {
            // We are creating the system UBO, nothing to add...
            SMaterialPipelineDescriptor const &systemUBODescriptor = systemUBOPipeline->getCurrentDescriptor().value();
            for(VkDescriptorSet set : systemUBOPipeline->descriptorSets)
            {
                vkDescriptorSets.push_back(set);
            }
        }

        for(VkDescriptorSet set : vkCreatedDescriptorSets)
        {
            vkDescriptorSets.push_back(set);
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
                CLog::Error(logTag(), "Failed to create pipeline layout. Result {}", result);
                return {EEngineStatus::Error};
            }
        }

        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos {};

        auto const *const shaderModule = getVkContext()->getResourceStorage()->extract<CVulkanShaderModuleResource>(aResolvedDependencies.at(aDependencies.shaderModuleId));
        for(auto const &[stage, dataAccessor] : shaderModule->getCurrentDescriptor()->shaderStages)
        {
            VkPipelineShaderStageCreateInfo shaderStageCreateInfo {};
            shaderStageCreateInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageCreateInfo.pNext               = nullptr;
            shaderStageCreateInfo.flags               = 0;
            shaderStageCreateInfo.pName               = "main";
            shaderStageCreateInfo.stage               = serialization::shaderStageFromPipelineStage(stage);
            shaderStageCreateInfo.module              = shaderModule->handles.at(stage);
            shaderStageCreateInfo.pSpecializationInfo = nullptr;

            shaderStageCreateInfos.push_back(shaderStageCreateInfo);
        }

        VkPipelineColorBlendStateCreateInfo colorBlending = aDescription.colorBlendState;
        colorBlending.pAttachments    = aDescription.colorBlendAttachmentStates.data();
        colorBlending.attachmentCount = aDescription.colorBlendAttachmentStates.size();

        VkGraphicsPipelineCreateInfo pipelineCreateInfo {};
        pipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.pNext               = nullptr;
        pipelineCreateInfo.flags               = 0;
        pipelineCreateInfo.pVertexInputState   = &(vertexInputStateCreateInfo);
        pipelineCreateInfo.pInputAssemblyState = &(aDescription.inputAssemblyState);
        pipelineCreateInfo.pViewportState      = &(viewPortStateCreateInfo);
        pipelineCreateInfo.pRasterizationState = &(aDescription.rasterizerState);
        pipelineCreateInfo.pMultisampleState   = &(aDescription.multiSampler);
        pipelineCreateInfo.pDepthStencilState  = &(aDescription.depthStencilState);
        pipelineCreateInfo.pColorBlendState    = &(colorBlending);
        pipelineCreateInfo.pDynamicState       = nullptr;
        pipelineCreateInfo.pTessellationState  = nullptr;
        pipelineCreateInfo.layout              = vkPipelineLayout;
        pipelineCreateInfo.renderPass          = renderPass->handle;
        pipelineCreateInfo.subpass             = aDependencies.subpass;
        pipelineCreateInfo.stageCount          = shaderStageCreateInfos.size();
        pipelineCreateInfo.pStages             = shaderStageCreateInfos.data();

        VkPipeline pipelineHandle = VK_NULL_HANDLE;
        {
            VkResult const result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipelineHandle);
            if( VkResult::VK_SUCCESS != result )
            {
                CLog::Error(logTag(), "Failed to create pipeline. Result {}", result);
                return {EEngineStatus::Error};
            }
        }

        this->pipeline       = pipelineHandle;
        this->pipelineLayout = vkPipelineLayout;
        this->descriptorPool = vkDescriptorPool;
        this->descriptorSets = vkDescriptorSets;

        setResourceState(EGpuApiResourceState::Loaded);

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
        vkDestroyPipeline(device, this->pipeline, nullptr);

        setResourceState(EGpuApiResourceState::Discarded);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

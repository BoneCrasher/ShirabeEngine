#ifndef __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__
#define __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__


#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/extensibility.h>
#include <material/serialization.h>

#include "vulkan_integration/resources/ivkglobalcontext.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanshadermoduleresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanPipelineResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SPipeline> { using TGpuApiResource = vulkan::SVulkanPipelineResource; };
    }

    namespace vulkan
    {
        using namespace resources;
        using namespace serialization;

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

            template <typename TResourceManager>
            static EEngineStatus initialize(SMaterialPipelineDescriptor const &aDescription
                                          , Handles_t                         &aGpuApiHandles
                                          , TResourceManager                  *aResourceManager
                                          , IVkGlobalContext                  *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SMaterialPipelineDescriptor const &aDescription
                                            , Handles_t                         &aGpuApiHandles
                                            , TResourceManager                  *aResourceManager
                                            , IVkGlobalContext                  *aVulkanEnvironment);

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using PipelineResourceState_t = SResourceState<SPipeline>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanPipelineResource::initialize(SMaterialPipelineDescriptor const &aDescription
                                                        , Handles_t                         &aGpuApiHandles
                                                        , TResourceManager                  *aResourceManager
                                                        , IVkGlobalContext                  *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto const &[fetchSystemPipelineSuccess, systemPipelineOptRef] = aResourceManager->template getResource<PipelineResourceState_t>(aDescription.systemUBOPipelineId, aVulkanEnvironment);
            if(CheckEngineError(fetchSystemPipelineSuccess))
            {
                return fetchSystemPipelineSuccess;
            }

            auto const &[fetchRenderPassSuccess, renderPassOptRef] = aResourceManager->template getResource<RenderPassResourceState_t>(aDescription.referenceRenderPassId, aVulkanEnvironment);
            if(CheckEngineError(fetchRenderPassSuccess))
            {
                return fetchRenderPassSuccess;
            }
            RenderPassResourceState_t const &renderPass = *renderPassOptRef;

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

            auto const createDescriptorSetLayouts =
                [&device, &typesAndSizes, &setLayouts, &createdSetLayouts] (SMaterialPipelineDescriptor const &aDescriptor, bool aRegisterForDescriptorSetAlloc) -> EEngineStatus
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
                            CLog::Error("SVulkanMaterialPipelineResource::initialize", "Failed to create pipeline descriptor set layout. Result {}", result);
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
            if(systemPipelineOptRef.has_value())
            {
                PipelineResourceState_t const &systemUBOPipeline = *systemPipelineOptRef;

                // Add the system UBO layouts in order to have them included in the pipeline layout
                EEngineStatus const status = createDescriptorSetLayouts(systemUBOPipeline.description, false);
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
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Could not create descriptor pool.");
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
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Could not create descriptor sets.");
                    return { EEngineStatus::Error };
                }
            }

            std::vector<VkDescriptorSet> vkDescriptorSets {};
            if(systemPipelineOptRef.has_value())
            {
                PipelineResourceState_t const &systemUBOPipeline = *systemPipelineOptRef;

                // Add the system up descriptors sets in order to reuse them with this material.
                for(VkDescriptorSet set : systemUBOPipeline.gpuApiHandles.descriptorSets)
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
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Failed to create pipeline layout. Result {}", result);
                    return {EEngineStatus::Error};
                }
            }

            std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos {};

            auto const &[shaderModuleFetchSuccess, shaderModuleOptRef] = aResourceManager->template getResource<RenderPassResourceState_t>(aDescription.shaderModuleId, aVulkanEnvironment);
            if(CheckEngineError(shaderModuleFetchSuccess))
            {
                return shaderModuleFetchSuccess;
            }
            ShaderModuleResourceState_t const &shaderModule = *shaderModuleOptRef;

            for(auto const &[stage, dataAccessor] : shaderModule.description.shaderStages)
            {
                VkPipelineShaderStageCreateInfo shaderStageCreateInfo {};
                shaderStageCreateInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStageCreateInfo.pNext               = nullptr;
                shaderStageCreateInfo.flags               = 0;
                shaderStageCreateInfo.pName               = "main";
                shaderStageCreateInfo.stage               = serialization::shaderStageFromPipelineStage(stage);
                shaderStageCreateInfo.module              = shaderModule.gpuApiHandles.handles.at(stage);
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
            pipelineCreateInfo.renderPass          = renderPass.gpuApiHandles.handle;
            pipelineCreateInfo.subpass             = aDescription.subpass;
            pipelineCreateInfo.stageCount          = shaderStageCreateInfos.size();
            pipelineCreateInfo.pStages             = shaderStageCreateInfos.data();

            VkPipeline pipelineHandle = VK_NULL_HANDLE;
            {
                VkResult const result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipelineHandle);
                if( VkResult::VK_SUCCESS != result )
                {
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Failed to create pipeline. Result {}", result);
                    return {EEngineStatus::Error};
                }
            }

            aGpuApiHandles.pipeline       = pipelineHandle;
            aGpuApiHandles.pipelineLayout = vkPipelineLayout;
            aGpuApiHandles.descriptorPool = vkDescriptorPool;
            aGpuApiHandles.descriptorSets = vkDescriptorSets;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanPipelineResource::deinitialize(SMaterialPipelineDescriptor const &aDescription
                                                          , Handles_t                         &aGpuApiHandles
                                                          , TResourceManager                  *aResourceManager
                                                          , IVkGlobalContext                  *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            vkDestroyDescriptorPool(device, aGpuApiHandles.descriptorPool, nullptr);
            vkDestroyPipeline(device, aGpuApiHandles.pipeline, nullptr);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

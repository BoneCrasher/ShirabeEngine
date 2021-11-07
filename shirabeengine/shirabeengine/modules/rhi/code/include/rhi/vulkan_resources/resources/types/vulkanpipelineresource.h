#ifndef __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__
#define __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__


#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <asset/material/serialization.h>

#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"
#include "rhi/vulkan_core/vulkanhelpers.h"
#include "rhi/vulkan_resources/resources/types/vulkanimageviewresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanbufferresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanrenderpassresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanshadermoduleresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanPipelineLayoutResource;
        struct SVulkanPipelineResource;
    }

    namespace rhi
    {
        template <> struct SRHIResourceMap<SRHIPipeline>       { using TMappedRHIResource = vulkan::SVulkanPipelineResource; };
        template <> struct SRHIResourceMap<SRHIPipelineLayout> { using TMappedRHIResource = vulkan::SVulkanPipelineLayoutResource; };
    }

    namespace vulkan
    {
        using namespace rhi;
        using namespace serialization;

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanPipelineLayoutResource
        {
            struct Handles_t
            {
                VkPipelineLayout                   pipelineLayout;
                std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIPipelineLayoutDescriptor const &aDescription
                                            , Handles_t                         &aGpuApiHandles
                                            , TResourceManager                  *aResourceManager
                                            , IVkGlobalContext                  *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIPipelineLayoutDescriptor const &aDescription
                                              , Handles_t                        &aGpuApiHandles
                                              , TResourceManager                 *aResourceManager
                                              , IVkGlobalContext                 *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIPipelineLayoutResourceState_t = SRHIResourceState<SRHIPipelineLayout>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        struct SSampledImageBinding
        {
            OptionalRef_t<RHIImageViewResourceState_t> imageView;
            OptionalRef_t<RHIImageResourceState_t>     image;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------

        template <typename TResourceManager>
        EEngineStatus SVulkanPipelineLayoutResource::initialize(SRHIPipelineLayoutDescriptor const &aDescription
                                                                , Handles_t                        &aGpuApiHandles
                                                                , TResourceManager                 *aResourceManager
                                                                , IVkGlobalContext                 *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto const &[fetchSystemPipelineSuccess, systemPipelineOptRef] = aResourceManager->template getResource<RHIPipelineLayoutResourceState_t>(aDescription.systemBasePipelineId, aVulkanEnvironment);
            if(CheckEngineError(fetchSystemPipelineSuccess))
            {
                return fetchSystemPipelineSuccess;
            }

            //<-----------------------------------------------------------------------------
            // Local helpers
            //<-----------------------------------------------------------------------------
            //
            // Derive descriptor set layouts based on a pipeline descriptor's signature data.
            //
            auto const createDescriptorSetLayouts =
                [&device](SRHIPipelineLayoutDescriptor const &aDescriptor, uint8_t const aSetOffset, std::vector<VkDescriptorSetLayout> &aTargetSet) -> EEngineStatus
            {
                uint64_t const setCount = aDescriptor.descriptorSetLayoutCreateInfos.size();
                assert(setCount > aSetOffset);

                for(uint64_t k=aSetOffset; k<setCount; ++k)
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
                            CLog::Error("SVulkanPipelineLayoutResource::initialize", "Failed to create pipeline descriptor set layout. Result {}", result);
                            return EEngineStatus::Error;
                        }

                        aTargetSet.push_back(vkDescriptorSetLayout);
                    }
                }

                return EEngineStatus::Ok;
            };

            auto const destroyDescriptorSetLayouts = [device] (std::vector<VkDescriptorSetLayout> &aDescriptorSetLayouts)
            {
                for(auto const &setLayout : aDescriptorSetLayouts)
                {
                    vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
                }
                aDescriptorSetLayouts.clear();
            };
            //<-----------------------------------------------------------------------------

            std::vector<VkDescriptorSetLayout> systemSets;
            std::vector<VkDescriptorSetLayout> materialSets;

            //
            // First of all, push back all system UBOs...
            //
            if(systemPipelineOptRef.has_value())
            {
                RHIPipelineLayoutResourceState_t const &systemUBOPipeline = *systemPipelineOptRef;

                // Fetch the system UBO layouts in order to have them included in the pipeline layout.
                // If the base pipeline hasn't been initialized yet, we're f*****.
                systemSets = systemUBOPipeline.rhiHandles.descriptorSetLayouts;
            }

            //
            // Then everything else...
            //
            uint8_t const systemPipelineOffset = 2; // System descriptor sets are bounds to set indices 0 and 1! Ignore them.
            EEngineStatus const status = createDescriptorSetLayouts(aDescription, systemPipelineOffset, materialSets);
            if(CheckEngineError(status))
            {
                destroyDescriptorSetLayouts(materialSets);
                return EEngineStatus::Error;
            }

            std::vector<VkDescriptorSetLayout> setLayouts {};
            setLayouts.insert(setLayouts.end(), systemSets.begin(),   systemSets.end());
            setLayouts.insert(setLayouts.end(), materialSets.begin(), materialSets.end());

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
                    CLog::Error("SVulkanPipelineLayoutResource::initialize", "Failed to create pipeline layout. Result {}", result);
                    destroyDescriptorSetLayouts(materialSets); // Do not clear out system set layouts! This will corrupt dependent functionality.
                    return EEngineStatus::Error;
                }
            }

            aGpuApiHandles.pipelineLayout       = vkPipelineLayout;
            aGpuApiHandles.descriptorSetLayouts = materialSets; // System-Sets won't be stored, as they are joined back in on descriptor set update and binding.

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanPipelineLayoutResource::deinitialize(SRHIPipelineLayoutDescriptor const &aDescription
                                                                  , Handles_t                        &aGpuApiHandles
                                                                  , TResourceManager                 *aResourceManager
                                                                  , IVkGlobalContext                 *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            for(auto const &setLayout : aGpuApiHandles.descriptorSetLayouts)
            {
                vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
            }
            vkDestroyPipelineLayout(device, aGpuApiHandles.pipelineLayout, nullptr);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------

        /**
         *
         */
        struct SVulkanPipelineResource
        {
            struct Handles_t
            {
                VkDescriptorPool             descriptorPool;
                std::vector<VkDescriptorSet> descriptorSets;
                VkPipeline                   pipeline;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIPipelineDescriptor const &aDescription
                                            , Handles_t                  &aGpuApiHandles
                                            , TResourceManager           *aResourceManager
                                            , IVkGlobalContext           *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIPipelineDescriptor const &aDescription
                                              , Handles_t                  &aGpuApiHandles
                                              , TResourceManager           *aResourceManager
                                              , IVkGlobalContext           *aVulkanEnvironment);

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIPipelineResourceState_t = SRHIResourceState<SRHIPipeline>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanPipelineResource::initialize(SRHIPipelineDescriptor const &aDescription
                                                          , Handles_t                  &aGpuApiHandles
                                                          , TResourceManager           *aResourceManager
                                                          , IVkGlobalContext           *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto const &[fetchPipelineLayoutStatus, pipelineLayoutOptRef] = aResourceManager->template getResource<RHIPipelineLayoutResourceState_t>(aDescription.pipelineLayoutResourceId, aVulkanEnvironment);
            if(CheckEngineError(fetchPipelineLayoutStatus))
            {
                return fetchPipelineLayoutStatus;
            }
            RHIPipelineLayoutResourceState_t const &pipelineLayout = *pipelineLayoutOptRef;

            auto const &[fetchRenderPassSuccess, renderPassOptRef] = aResourceManager->template getResource<RHIRenderPassResourceState_t>(aDescription.referenceRenderPassId, aVulkanEnvironment);
            if(CheckEngineError(fetchRenderPassSuccess))
            {
                return fetchRenderPassSuccess;
            }
            RHIRenderPassResourceState_t const &renderPass = *renderPassOptRef;

            VkDeviceSize const descriptorSetCount = pipelineLayout.rhiHandles.descriptorSetLayouts.size();

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

            std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos {};

            auto const &[shaderModuleFetchSuccess, shaderModuleOptRef] = aResourceManager->template getResource<RHIShaderModuleResourceState_t>(pipelineLayout.rhiCreateDesc.shaderModuleId, aVulkanEnvironment);
            if(CheckEngineError(shaderModuleFetchSuccess))
            {
                return shaderModuleFetchSuccess;
            }
            RHIShaderModuleResourceState_t const &shaderModule = *shaderModuleOptRef;

            for(auto const &[stage, dataAccessor] : shaderModule.rhiCreateDesc.shaderStages)
            {
                VkPipelineShaderStageCreateInfo shaderStageCreateInfo {};
                shaderStageCreateInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStageCreateInfo.pNext               = nullptr;
                shaderStageCreateInfo.flags               = 0;
                shaderStageCreateInfo.pName               = "main";
                shaderStageCreateInfo.stage               = shaderStageFromPipelineStage(stage);
                shaderStageCreateInfo.module              = shaderModule.rhiHandles.handles.at(stage);
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
            pipelineCreateInfo.layout              = pipelineLayout.rhiHandles.pipelineLayout;
            pipelineCreateInfo.renderPass          = renderPass.rhiHandles.handle;
            pipelineCreateInfo.subpass             = aDescription.subpass;
            pipelineCreateInfo.stageCount          = shaderStageCreateInfos.size();
            pipelineCreateInfo.pStages             = shaderStageCreateInfos.data();

            VkPipeline pipelineHandle = VK_NULL_HANDLE;
            {
                VkResult const result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipelineHandle);
                if( VkResult::VK_SUCCESS != result )
                {
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Failed to create pipeline. Result {}", result);
                    return EEngineStatus::Error;
                }
            }

            aGpuApiHandles.pipeline       = pipelineHandle;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanPipelineResource::deinitialize(SRHIPipelineDescriptor const &aDescription
                                                            , Handles_t                  &aGpuApiHandles
                                                            , TResourceManager           *aResourceManager
                                                            , IVkGlobalContext           *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            vkDestroyPipeline(device, aGpuApiHandles.pipeline, nullptr);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

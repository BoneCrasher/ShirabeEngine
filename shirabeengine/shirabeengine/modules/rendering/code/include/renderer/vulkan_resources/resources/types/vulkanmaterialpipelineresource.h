#ifndef __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__
#define __SHIRABE_VULKAN_PIPELINE_RESOURCE_H__


#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <asset/material/serialization.h>

#include "renderer/resource_management/resourcetypes.h"
#include "renderer/resource_management/extensibility.h"
#include "renderer/vulkan_resources/resources/ivkglobalcontext.h"
#include "renderer/vulkan_core/vulkandevicecapabilities.h"

#include "renderer/vulkan_resources/resources/types/vulkantextureviewresource.h"
#include "renderer/vulkan_resources/resources/types/vulkanbufferresource.h"
#include "renderer/vulkan_resources/resources/types/vulkanrenderpassresource.h"
#include "renderer/vulkan_resources/resources/types/vulkanshadermoduleresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanBasePipelineResource;
        struct SVulkanPipelineResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<SPipeline>     { using TGpuApiResource = vulkan::SVulkanPipelineResource; };
        template <> struct SLogicalToGpuApiResourceTypeMap<SBasePipeline> { using TGpuApiResource = vulkan::SVulkanBasePipelineResource; };
    }

    namespace vulkan
    {
        using namespace resources;
        using namespace serialization;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanBasePipelineResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t
            {
                VkPipelineLayout                   pipelineLayout;
                std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SMaterialBasePipelineDescriptor const &aDescription
                                          , Handles_t                             &aGpuApiHandles
                                          , TResourceManager                      *aResourceManager
                                          , IVkGlobalContext                      *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SMaterialBasePipelineDescriptor const &aDescription
                                            , Handles_t                             &aGpuApiHandles
                                            , TResourceManager                      *aResourceManager
                                            , IVkGlobalContext                      *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using BasePipelineResourceState_t = SResourceState<SBasePipeline>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanBasePipelineResource::initialize(SMaterialBasePipelineDescriptor const &aDescription
                                                              , Handles_t                           &aGpuApiHandles
                                                              , TResourceManager                    *aResourceManager
                                                              , IVkGlobalContext                    *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto const &[fetchSystemPipelineSuccess, systemPipelineOptRef] = aResourceManager->template getResource<BasePipelineResourceState_t>(aDescription.systemBasePipelineId, aVulkanEnvironment);
            if(CheckEngineError(fetchSystemPipelineSuccess))
            {
                return fetchSystemPipelineSuccess;
            }

            //
            // Derive descriptor set layouts based on a pipeline descriptor's signature data.
            //
            auto const createDescriptorSetLayouts =
                [&device] (SMaterialBasePipelineDescriptor const &aDescriptor, std::vector<VkDescriptorSetLayout> &aTargetSet) -> EEngineStatus
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

                        aTargetSet.push_back(vkDescriptorSetLayout);
                    }
                }

                return EEngineStatus::Ok;
            };

            std::vector<VkDescriptorSetLayout> systemSets;
            std::vector<VkDescriptorSetLayout> materialSets;

            //
            // First of all, push back all system UBOs...
            //
            if(systemPipelineOptRef.has_value())
            {
                BasePipelineResourceState_t const &systemUBOPipeline = *systemPipelineOptRef;

                // Add the system UBO layouts in order to have them included in the pipeline layout
                EEngineStatus const status = createDescriptorSetLayouts(systemUBOPipeline.description, systemSets);
            }

            //
            // Then everything else...
            //
            EEngineStatus const status = createDescriptorSetLayouts(aDescription, materialSets);
            if(CheckEngineError(status))
            {
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
                    CLog::Error("SVulkanMaterialPipelineResource::initialize", "Failed to create pipeline layout. Result {}", result);
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
        EEngineStatus SVulkanBasePipelineResource::deinitialize(SMaterialBasePipelineDescriptor const &aDescription
                                                          , Handles_t                                 &aGpuApiHandles
                                                          , TResourceManager                          *aResourceManager
                                                          , IVkGlobalContext                          *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            for(auto const &setLayout : aGpuApiHandles.descriptorSetLayouts)
            {
                vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
            }
            vkDestroyPipelineLayout(device, aGpuApiHandles.pipelineLayout, nullptr);

            return EEngineStatus::Ok;
        }

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanPipelineResource
        {
            static constexpr bool is_loadable      = false;
            static constexpr bool is_unloadable    = false;
            static constexpr bool is_transferrable = false;

            struct Handles_t
            {
                VkPipeline pipeline;
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
                                                          , Handles_t                       &aGpuApiHandles
                                                          , TResourceManager                *aResourceManager
                                                          , IVkGlobalContext                *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            auto const &[fetchSharedMaterialStatus, sharedMaterialOptRef] = aResourceManager->template getResource<BasePipelineResourceState_t>(aDescription.sharedMaterialId, aVulkanEnvironment);
            if(CheckEngineError(fetchSharedMaterialStatus))
            {
                return fetchSharedMaterialStatus;
            }
            BasePipelineResourceState_t const &sharedMaterial = *sharedMaterialOptRef;

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

            std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos {};

            auto const &[shaderModuleFetchSuccess, shaderModuleOptRef] = aResourceManager->template getResource<ShaderModuleResourceState_t>(sharedMaterial.description.shaderModuleId, aVulkanEnvironment);
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
            pipelineCreateInfo.layout              = sharedMaterial.gpuApiHandles.pipelineLayout;
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
                    return EEngineStatus::Error;
                }
            }

            aGpuApiHandles.pipeline = pipelineHandle;

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

            vkDestroyPipeline(device, aGpuApiHandles.pipeline, nullptr);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

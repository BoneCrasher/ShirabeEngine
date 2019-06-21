#include <filesystem>
#include <fstream>

#include <graphicsapi/resources/types/pipeline.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>

#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanpipelineresource.h"
#include "vulkan/resources/types/vulkanrenderpassresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineCreationTask(
                CPipeline::CCreationRequest    const &aRequest,
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CPipeline::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                VkDevice const device = mVulkanEnvironment->getState().selectedLogicalDevice;

                PublicResourceId_t     const &renderPassHandle   = aRequest.renderPassHandle();
                PublicResourceIdList_t const &textureViewHandles = aRequest.textureViewHandles();

                CStdSharedPtr_t<void> renderPassPrivateHandle = aDependencies.at(renderPassHandle);
                if(not renderPassPrivateHandle)
                {
                    CLog::Error(logTag(), "Failed to create frame buffer due to missing dependency.");
                    return { EEngineStatus::DXDevice_CreateRTV_Failed };
                }

                CStdSharedPtr_t<SVulkanRenderPassResource> renderPass = std::static_pointer_cast<SVulkanRenderPassResource>(renderPassPrivateHandle);
                if(not renderPass)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for frame buffer creation. %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                // Create the shader modules here...
                // IMPORTANT: The task backend receives access to the asset system to not have the raw data stored in the descriptors and requests...

                VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
                vertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputStateCreateInfo.pNext                           = nullptr;
                vertexInputStateCreateInfo.flags                           = 0;
                vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(desc.vertexInputAttributes.size());
                vertexInputStateCreateInfo.vertexBindingDescriptionCount   = static_cast<uint32_t>(desc.vertexInputBindings  .size());
                vertexInputStateCreateInfo.pVertexAttributeDescriptions    = desc.vertexInputAttributes.data();
                vertexInputStateCreateInfo.pVertexBindingDescriptions      = desc.vertexInputBindings  .data();

                VkPipelineViewportStateCreateInfo viewPortStateCreateInfo {};
                viewPortStateCreateInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewPortStateCreateInfo.pNext          = nullptr;
                viewPortStateCreateInfo.flags          = 0;
                viewPortStateCreateInfo.viewportCount  = 1;
                viewPortStateCreateInfo.pViewports     = &(desc.viewPort);
                viewPortStateCreateInfo.scissorCount   = 0;
                viewPortStateCreateInfo.pScissors      = nullptr;

                std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

                for(auto const &[stage, dataAccessor] : desc.shaderStages)
                {
                    if(not dataAccessor)
                    {
                        // If we don't have any data to access, then don't create a shader module.
                        continue;
                    }

                    ByteBuffer const data = dataAccessor();

                    VkShaderModuleCreateInfo shaderModuleCreateInfo {};
                    shaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    shaderModuleCreateInfo.pNext    = nullptr;
                    shaderModuleCreateInfo.flags    = 0;
                    shaderModuleCreateInfo.pCode    = reinterpret_cast<uint32_t const*>(data.data()); // The asset system only loads regular 8 byte buffers.
                    shaderModuleCreateInfo.codeSize = data.size();

                    VkShaderModule vkShaderModule = VK_NULL_HANDLE;
                    VkResult const moduleCreationResult = vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &vkShaderModule);
                    if(VkResult::VK_SUCCESS != moduleCreationResult)
                    {
                        CLog::Error(logTag(), "Failed to create shader module for stage %0", stage);
                        continue;
                    }

                    VkPipelineShaderStageCreateInfo shaderStageCreateInfo {};
                    shaderStageCreateInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    shaderStageCreateInfo.pNext               = nullptr;
                    shaderStageCreateInfo.flags               = 0;
                    shaderStageCreateInfo.pName               = "...";
                    shaderStageCreateInfo.stage               = (VkShaderStageFlagBits) stage;
                    shaderStageCreateInfo.module              = vkShaderModule;
                    shaderStageCreateInfo.pSpecializationInfo = nullptr;

                    shaderStages.push_back(shaderStageCreateInfo);
                }

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
                pipelineCreateInfo.pColorBlendState    = &(desc.colorBlendState);
                pipelineCreateInfo.pDynamicState       = nullptr;
                pipelineCreateInfo.pTessellationState  = nullptr;
                pipelineCreateInfo.layout              = nullptr; // TODO: Derive from signature // CRASH here!!!
                pipelineCreateInfo.renderPass          = renderPass->handle;
                pipelineCreateInfo.subpass             = desc.subpass;
                pipelineCreateInfo.stageCount          = shaderStages.size();
                pipelineCreateInfo.pStages             = shaderStages.data();

                VkPipeline pipeline = VK_NULL_HANDLE;

                VkResult const result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), "Failed to create pipeline. Result %0", result);
                    return { EEngineStatus::Error };
                }

                SVulkanPipelineResource *resource = new SVulkanPipelineResource();
                resource->pipeline = pipeline;

                auto const deleter = [] (SVulkanPipelineResource const *p)
                {
                    delete p;
                };

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = desc.name;
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanPipelineResource>(resource, deleter);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineUpdateTask(
                CPipeline::CUpdateRequest       const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aDependencies);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineDestructionTask(
                CPipeline::CDestructionRequest  const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                CStdSharedPtr_t<SVulkanPipelineResource> pipeline = std::static_pointer_cast<SVulkanPipelineResource>(aAssignment.internalResourceHandle);
                if(nullptr == pipeline)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for pipeline destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkDevice const &device = mVulkanEnvironment->getState().selectedLogicalDevice;
                for(auto const &module : pipeline->shaderModules)
                {
                    vkDestroyShaderModule(device, module, nullptr);
                }

                vkDestroyPipeline(device, pipeline->pipeline,    nullptr);

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnPipelineQueryTask(
                CPipeline::CQuery               const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return { status };
        }
        //<-----------------------------------------------------------------------------
    }
}

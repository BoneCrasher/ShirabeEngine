#pragma once

/*
** Copyright (c) 2015-2017 The Khronos Group Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/*
** This header is generated from the Khronos Vulkan XML API Registry.
**
*/

#include "vulkan/vk_layer.h"
#include <unordered_map>

class layer_factory;
extern std::vector<layer_factory *> global_interceptor_list;
extern debug_report_data *vlf_report_data;

namespace vulkan_layer_factory {



VKAPI_ATTR VkResult VKAPI_CALL CreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance);

VKAPI_ATTR void VKAPI_CALL DestroyInstance(
    VkInstance                                  instance,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures*                   pFeatures);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties*                         pFormatProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkImageFormatProperties*                    pImageFormatProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties*                 pProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties*                    pQueueFamilyProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties*           pMemoryProperties);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetInstanceProcAddr(
    VkInstance                                  instance,
    const char*                                 pName);

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL GetDeviceProcAddr(
    VkDevice                                    device,
    const char*                                 pName);

VKAPI_ATTR VkResult VKAPI_CALL CreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice);

VKAPI_ATTR void VKAPI_CALL DestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceExtensionProperties(
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties);

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceExtensionProperties(
    VkPhysicalDevice                            physicalDevice,
    const char*                                 pLayerName,
    uint32_t*                                   pPropertyCount,
    VkExtensionProperties*                      pProperties);

VKAPI_ATTR VkResult VKAPI_CALL EnumerateInstanceLayerProperties(
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties);

VKAPI_ATTR VkResult VKAPI_CALL EnumerateDeviceLayerProperties(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkLayerProperties*                          pProperties);

VKAPI_ATTR void VKAPI_CALL GetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue);

VKAPI_ATTR VkResult VKAPI_CALL QueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence);

VKAPI_ATTR VkResult VKAPI_CALL QueueWaitIdle(
    VkQueue                                     queue);

VKAPI_ATTR VkResult VKAPI_CALL DeviceWaitIdle(
    VkDevice                                    device);

VKAPI_ATTR VkResult VKAPI_CALL AllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory);

VKAPI_ATTR void VKAPI_CALL FreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL MapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData);

VKAPI_ATTR void VKAPI_CALL UnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory);

VKAPI_ATTR VkResult VKAPI_CALL FlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges);

VKAPI_ATTR VkResult VKAPI_CALL InvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges);

VKAPI_ATTR void VKAPI_CALL GetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes);

VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset);

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset);

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkSampleCountFlagBits                       samples,
    VkImageUsageFlags                           usage,
    VkImageTiling                               tiling,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties*              pProperties);

VKAPI_ATTR VkResult VKAPI_CALL QueueBindSparse(
    VkQueue                                     queue,
    uint32_t                                    bindInfoCount,
    const VkBindSparseInfo*                     pBindInfo,
    VkFence                                     fence);

VKAPI_ATTR VkResult VKAPI_CALL CreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence);

VKAPI_ATTR void VKAPI_CALL DestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL ResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences);

VKAPI_ATTR VkResult VKAPI_CALL GetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence);

VKAPI_ATTR VkResult VKAPI_CALL WaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout);

VKAPI_ATTR VkResult VKAPI_CALL CreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore);

VKAPI_ATTR void VKAPI_CALL DestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent);

VKAPI_ATTR void VKAPI_CALL DestroyEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL GetEventStatus(
    VkDevice                                    device,
    VkEvent                                     event);

VKAPI_ATTR VkResult VKAPI_CALL SetEvent(
    VkDevice                                    device,
    VkEvent                                     event);

VKAPI_ATTR VkResult VKAPI_CALL ResetEvent(
    VkDevice                                    device,
    VkEvent                                     event);

VKAPI_ATTR VkResult VKAPI_CALL CreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool);

VKAPI_ATTR void VKAPI_CALL DestroyQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL GetQueryPoolResults(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags);

VKAPI_ATTR VkResult VKAPI_CALL CreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer);

VKAPI_ATTR void VKAPI_CALL DestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView);

VKAPI_ATTR void VKAPI_CALL DestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage);

VKAPI_ATTR void VKAPI_CALL DestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR void VKAPI_CALL GetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout);

VKAPI_ATTR VkResult VKAPI_CALL CreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView);

VKAPI_ATTR void VKAPI_CALL DestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule);

VKAPI_ATTR void VKAPI_CALL DestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache);

VKAPI_ATTR void VKAPI_CALL DestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL GetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData);

VKAPI_ATTR VkResult VKAPI_CALL MergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches);

VKAPI_ATTR VkResult VKAPI_CALL CreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines);

VKAPI_ATTR VkResult VKAPI_CALL CreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines);

VKAPI_ATTR void VKAPI_CALL DestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout);

VKAPI_ATTR void VKAPI_CALL DestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler);

VKAPI_ATTR void VKAPI_CALL DestroySampler(
    VkDevice                                    device,
    VkSampler                                   sampler,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout);

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool);

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL ResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags);

VKAPI_ATTR VkResult VKAPI_CALL AllocateDescriptorSets(
    VkDevice                                    device,
    const VkDescriptorSetAllocateInfo*          pAllocateInfo,
    VkDescriptorSet*                            pDescriptorSets);

VKAPI_ATTR VkResult VKAPI_CALL FreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets);

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies);

VKAPI_ATTR VkResult VKAPI_CALL CreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer);

VKAPI_ATTR void VKAPI_CALL DestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass);

VKAPI_ATTR void VKAPI_CALL DestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR void VKAPI_CALL GetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity);

VKAPI_ATTR VkResult VKAPI_CALL CreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool);

VKAPI_ATTR void VKAPI_CALL DestroyCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL ResetCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolResetFlags                     flags);

VKAPI_ATTR VkResult VKAPI_CALL AllocateCommandBuffers(
    VkDevice                                    device,
    const VkCommandBufferAllocateInfo*          pAllocateInfo,
    VkCommandBuffer*                            pCommandBuffers);

VKAPI_ATTR void VKAPI_CALL FreeCommandBuffers(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers);

VKAPI_ATTR VkResult VKAPI_CALL BeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo);

VKAPI_ATTR VkResult VKAPI_CALL EndCommandBuffer(
    VkCommandBuffer                             commandBuffer);

VKAPI_ATTR VkResult VKAPI_CALL ResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags);

VKAPI_ATTR void VKAPI_CALL CmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline);

VKAPI_ATTR void VKAPI_CALL CmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports);

VKAPI_ATTR void VKAPI_CALL CmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors);

VKAPI_ATTR void VKAPI_CALL CmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth);

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor);

VKAPI_ATTR void VKAPI_CALL CmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]);

VKAPI_ATTR void VKAPI_CALL CmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds);

VKAPI_ATTR void VKAPI_CALL CmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask);

VKAPI_ATTR void VKAPI_CALL CmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask);

VKAPI_ATTR void VKAPI_CALL CmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference);

VKAPI_ATTR void VKAPI_CALL CmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets);

VKAPI_ATTR void VKAPI_CALL CmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType);

VKAPI_ATTR void VKAPI_CALL CmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets);

VKAPI_ATTR void VKAPI_CALL CmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance);

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance);

VKAPI_ATTR void VKAPI_CALL CmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride);

VKAPI_ATTR void VKAPI_CALL CmdDispatch(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ);

VKAPI_ATTR void VKAPI_CALL CmdDispatchIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset);

VKAPI_ATTR void VKAPI_CALL CmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions);

VKAPI_ATTR void VKAPI_CALL CmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions);

VKAPI_ATTR void VKAPI_CALL CmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter);

VKAPI_ATTR void VKAPI_CALL CmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions);

VKAPI_ATTR void VKAPI_CALL CmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions);

VKAPI_ATTR void VKAPI_CALL CmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData);

VKAPI_ATTR void VKAPI_CALL CmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data);

VKAPI_ATTR void VKAPI_CALL CmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges);

VKAPI_ATTR void VKAPI_CALL CmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges);

VKAPI_ATTR void VKAPI_CALL CmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects);

VKAPI_ATTR void VKAPI_CALL CmdResolveImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageResolve*                       pRegions);

VKAPI_ATTR void VKAPI_CALL CmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask);

VKAPI_ATTR void VKAPI_CALL CmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask);

VKAPI_ATTR void VKAPI_CALL CmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers);

VKAPI_ATTR void VKAPI_CALL CmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers);

VKAPI_ATTR void VKAPI_CALL CmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags);

VKAPI_ATTR void VKAPI_CALL CmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query);

VKAPI_ATTR void VKAPI_CALL CmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount);

VKAPI_ATTR void VKAPI_CALL CmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query);

VKAPI_ATTR void VKAPI_CALL CmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags);

VKAPI_ATTR void VKAPI_CALL CmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues);

VKAPI_ATTR void VKAPI_CALL CmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents);

VKAPI_ATTR void VKAPI_CALL CmdNextSubpass(
    VkCommandBuffer                             commandBuffer,
    VkSubpassContents                           contents);

VKAPI_ATTR void VKAPI_CALL CmdEndRenderPass(
    VkCommandBuffer                             commandBuffer);

VKAPI_ATTR void VKAPI_CALL CmdExecuteCommands(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers);


VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos);

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos);

VKAPI_ATTR void VKAPI_CALL GetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures);

VKAPI_ATTR void VKAPI_CALL CmdSetDeviceMask(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask);

VKAPI_ATTR void VKAPI_CALL CmdDispatchBase(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ);

VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceGroups(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties);

VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements2(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties2(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties);

VKAPI_ATTR void VKAPI_CALL TrimCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags);

VKAPI_ATTR void VKAPI_CALL GetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue);

VKAPI_ATTR VkResult VKAPI_CALL CreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion);

VKAPI_ATTR void VKAPI_CALL DestroySamplerYcbcrConversion(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorUpdateTemplate(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate);

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorUpdateTemplate(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSetWithTemplate(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalBufferProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalFenceProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalSemaphoreProperties(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties);

VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport);


VKAPI_ATTR void VKAPI_CALL DestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes);


VKAPI_ATTR VkResult VKAPI_CALL CreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain);

VKAPI_ATTR void VKAPI_CALL DestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainImagesKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pSwapchainImageCount,
    VkImage*                                    pSwapchainImages);

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex);

VKAPI_ATTR VkResult VKAPI_CALL QueuePresentKHR(
    VkQueue                                     queue,
    const VkPresentInfoKHR*                     pPresentInfo);

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupPresentCapabilitiesKHR(
    VkDevice                                    device,
    VkDeviceGroupPresentCapabilitiesKHR*        pDeviceGroupPresentCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL GetDeviceGroupSurfacePresentModesKHR(
    VkDevice                                    device,
    VkSurfaceKHR                                surface,
    VkDeviceGroupPresentModeFlagsKHR*           pModes);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pRectCount,
    VkRect2D*                                   pRects);

VKAPI_ATTR VkResult VKAPI_CALL AcquireNextImage2KHR(
    VkDevice                                    device,
    const VkAcquireNextImageInfoKHR*            pAcquireInfo,
    uint32_t*                                   pImageIndex);


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPropertiesKHR*                     pProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceDisplayPlanePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pPropertyCount,
    VkDisplayPlanePropertiesKHR*                pProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays);

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties);

VKAPI_ATTR VkResult VKAPI_CALL CreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode);

VKAPI_ATTR VkResult VKAPI_CALL GetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL CreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);


VKAPI_ATTR VkResult VKAPI_CALL CreateSharedSwapchainsKHR(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainCreateInfoKHR*             pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchains);

#ifdef VK_USE_PLATFORM_XLIB_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateXlibSurfaceKHR(
    VkInstance                                  instance,
    const VkXlibSurfaceCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceXlibPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    Display*                                    dpy,
    VisualID                                    visualID);
#endif /* VK_USE_PLATFORM_XLIB_KHR */

#ifdef VK_USE_PLATFORM_XCB_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateXcbSurfaceKHR(
    VkInstance                                  instance,
    const VkXcbSurfaceCreateInfoKHR*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceXcbPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    xcb_connection_t*                           connection,
    xcb_visualid_t                              visual_id);
#endif /* VK_USE_PLATFORM_XCB_KHR */

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateWaylandSurfaceKHR(
    VkInstance                                  instance,
    const VkWaylandSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceWaylandPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    struct wl_display*                          display);
#endif /* VK_USE_PLATFORM_WAYLAND_KHR */

#ifdef VK_USE_PLATFORM_MIR_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateMirSurfaceKHR(
    VkInstance                                  instance,
    const VkMirSurfaceCreateInfoKHR*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceMirPresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    MirConnection*                              connection);
#endif /* VK_USE_PLATFORM_MIR_KHR */

#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateAndroidSurfaceKHR(
    VkInstance                                  instance,
    const VkAndroidSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);
#endif /* VK_USE_PLATFORM_ANDROID_KHR */

#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL CreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);

VKAPI_ATTR VkBool32 VKAPI_CALL GetPhysicalDeviceWin32PresentationSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex);
#endif /* VK_USE_PLATFORM_WIN32_KHR */




VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFeatures2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceFeatures2*                  pFeatures);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceProperties2*                pProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkFormatProperties2*                        pFormatProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceImageFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceImageFormatInfo2*     pImageFormatInfo,
    VkImageFormatProperties2*                   pImageFormatProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceQueueFamilyProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t*                                   pQueueFamilyPropertyCount,
    VkQueueFamilyProperties2*                   pQueueFamilyProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMemoryProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkPhysicalDeviceMemoryProperties2*          pMemoryProperties);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceSparseImageFormatProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo,
    uint32_t*                                   pPropertyCount,
    VkSparseImageFormatProperties2*             pProperties);


VKAPI_ATTR void VKAPI_CALL GetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures);

VKAPI_ATTR void VKAPI_CALL CmdSetDeviceMaskKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask);

VKAPI_ATTR void VKAPI_CALL CmdDispatchBaseKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ);



VKAPI_ATTR void VKAPI_CALL TrimCommandPoolKHR(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags);


VKAPI_ATTR VkResult VKAPI_CALL EnumeratePhysicalDeviceGroupsKHR(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties);


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalBufferPropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalBufferInfo*   pExternalBufferInfo,
    VkExternalBufferProperties*                 pExternalBufferProperties);


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleKHR(
    VkDevice                                    device,
    const VkMemoryGetWin32HandleInfoKHR*        pGetWin32HandleInfo,
    HANDLE*                                     pHandle);

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandlePropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    HANDLE                                      handle,
    VkMemoryWin32HandlePropertiesKHR*           pMemoryWin32HandleProperties);
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdKHR(
    VkDevice                                    device,
    const VkMemoryGetFdInfoKHR*                 pGetFdInfo,
    int*                                        pFd);

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryFdPropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    int                                         fd,
    VkMemoryFdPropertiesKHR*                    pMemoryFdProperties);

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalSemaphorePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo,
    VkExternalSemaphoreProperties*              pExternalSemaphoreProperties);


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkImportSemaphoreWin32HandleInfoKHR*  pImportSemaphoreWin32HandleInfo);

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkSemaphoreGetWin32HandleInfoKHR*     pGetWin32HandleInfo,
    HANDLE*                                     pHandle);
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL ImportSemaphoreFdKHR(
    VkDevice                                    device,
    const VkImportSemaphoreFdInfoKHR*           pImportSemaphoreFdInfo);

VKAPI_ATTR VkResult VKAPI_CALL GetSemaphoreFdKHR(
    VkDevice                                    device,
    const VkSemaphoreGetFdInfoKHR*              pGetFdInfo,
    int*                                        pFd);


VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetKHR(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites);

VKAPI_ATTR void VKAPI_CALL CmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer                             commandBuffer,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    const void*                                 pData);




VKAPI_ATTR VkResult VKAPI_CALL CreateDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate);

VKAPI_ATTR void VKAPI_CALL DestroyDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR void VKAPI_CALL UpdateDescriptorSetWithTemplateKHR(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData);


VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain);


VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceExternalFencePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceExternalFenceInfo*    pExternalFenceInfo,
    VkExternalFenceProperties*                  pExternalFenceProperties);


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL ImportFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkImportFenceWin32HandleInfoKHR*      pImportFenceWin32HandleInfo);

VKAPI_ATTR VkResult VKAPI_CALL GetFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkFenceGetWin32HandleInfoKHR*         pGetWin32HandleInfo,
    HANDLE*                                     pHandle);
#endif /* VK_USE_PLATFORM_WIN32_KHR */


VKAPI_ATTR VkResult VKAPI_CALL ImportFenceFdKHR(
    VkDevice                                    device,
    const VkImportFenceFdInfoKHR*               pImportFenceFdInfo);

VKAPI_ATTR VkResult VKAPI_CALL GetFenceFdKHR(
    VkDevice                                    device,
    const VkFenceGetFdInfoKHR*                  pGetFdInfo,
    int*                                        pFd);



VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkSurfaceCapabilities2KHR*                  pSurfaceCapabilities);

VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceFormats2KHR(
    VkPhysicalDevice                            physicalDevice,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormat2KHR*                        pSurfaceFormats);






VKAPI_ATTR void VKAPI_CALL GetImageMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetBufferMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements);

VKAPI_ATTR void VKAPI_CALL GetImageSparseMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements);



VKAPI_ATTR VkResult VKAPI_CALL CreateSamplerYcbcrConversionKHR(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion);

VKAPI_ATTR void VKAPI_CALL DestroySamplerYcbcrConversionKHR(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator);


VKAPI_ATTR VkResult VKAPI_CALL BindBufferMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos);

VKAPI_ATTR VkResult VKAPI_CALL BindImageMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos);


VKAPI_ATTR void VKAPI_CALL GetDescriptorSetLayoutSupportKHR(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport);


VKAPI_ATTR VkResult VKAPI_CALL CreateDebugReportCallbackEXT(
    VkInstance                                  instance,
    const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugReportCallbackEXT*                   pCallback);

VKAPI_ATTR void VKAPI_CALL DestroyDebugReportCallbackEXT(
    VkInstance                                  instance,
    VkDebugReportCallbackEXT                    callback,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR void VKAPI_CALL DebugReportMessageEXT(
    VkInstance                                  instance,
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage);








VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectTagEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectTagInfoEXT*        pTagInfo);

VKAPI_ATTR VkResult VKAPI_CALL DebugMarkerSetObjectNameEXT(
    VkDevice                                    device,
    const VkDebugMarkerObjectNameInfoEXT*       pNameInfo);

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerBeginEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo);

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerEndEXT(
    VkCommandBuffer                             commandBuffer);

VKAPI_ATTR void VKAPI_CALL CmdDebugMarkerInsertEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugMarkerMarkerInfoEXT*           pMarkerInfo);




VKAPI_ATTR void VKAPI_CALL CmdDrawIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);

VKAPI_ATTR void VKAPI_CALL CmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);






VKAPI_ATTR VkResult VKAPI_CALL GetShaderInfoAMD(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    VkShaderStageFlagBits                       shaderStage,
    VkShaderInfoTypeAMD                         infoType,
    size_t*                                     pInfoSize,
    void*                                       pInfo);




VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceExternalImageFormatPropertiesNV(
    VkPhysicalDevice                            physicalDevice,
    VkFormat                                    format,
    VkImageType                                 type,
    VkImageTiling                               tiling,
    VkImageUsageFlags                           usage,
    VkImageCreateFlags                          flags,
    VkExternalMemoryHandleTypeFlagsNV           externalHandleType,
    VkExternalImageFormatPropertiesNV*          pExternalImageFormatProperties);


#ifdef VK_USE_PLATFORM_WIN32_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryWin32HandleNV(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkExternalMemoryHandleTypeFlagsNV           handleType,
    HANDLE*                                     pHandle);
#endif /* VK_USE_PLATFORM_WIN32_KHR */

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif /* VK_USE_PLATFORM_WIN32_KHR */


#ifdef VK_USE_PLATFORM_VI_NN

VKAPI_ATTR VkResult VKAPI_CALL CreateViSurfaceNN(
    VkInstance                                  instance,
    const VkViSurfaceCreateInfoNN*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);
#endif /* VK_USE_PLATFORM_VI_NN */




VKAPI_ATTR void VKAPI_CALL CmdProcessCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdProcessCommandsInfoNVX*          pProcessCommandsInfo);

VKAPI_ATTR void VKAPI_CALL CmdReserveSpaceForCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdReserveSpaceForCommandsInfoNVX*  pReserveSpaceInfo);

VKAPI_ATTR VkResult VKAPI_CALL CreateIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    const VkIndirectCommandsLayoutCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectCommandsLayoutNVX*                pIndirectCommandsLayout);

VKAPI_ATTR void VKAPI_CALL DestroyIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    VkIndirectCommandsLayoutNVX                 indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL CreateObjectTableNVX(
    VkDevice                                    device,
    const VkObjectTableCreateInfoNVX*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkObjectTableNVX*                           pObjectTable);

VKAPI_ATTR void VKAPI_CALL DestroyObjectTableNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL RegisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectTableEntryNVX* const*         ppObjectTableEntries,
    const uint32_t*                             pObjectIndices);

VKAPI_ATTR VkResult VKAPI_CALL UnregisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectEntryTypeNVX*                 pObjectEntryTypes,
    const uint32_t*                             pObjectIndices);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceGeneratedCommandsPropertiesNVX(
    VkPhysicalDevice                            physicalDevice,
    VkDeviceGeneratedCommandsFeaturesNVX*       pFeatures,
    VkDeviceGeneratedCommandsLimitsNVX*         pLimits);


VKAPI_ATTR void VKAPI_CALL CmdSetViewportWScalingNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewportWScalingNV*                 pViewportWScalings);


VKAPI_ATTR VkResult VKAPI_CALL ReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display);

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT

VKAPI_ATTR VkResult VKAPI_CALL AcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display);

VKAPI_ATTR VkResult VKAPI_CALL GetRandROutputDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    RROutput                                    rrOutput,
    VkDisplayKHR*                               pDisplay);
#endif /* VK_USE_PLATFORM_XLIB_XRANDR_EXT */


VKAPI_ATTR VkResult VKAPI_CALL GetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities);


VKAPI_ATTR VkResult VKAPI_CALL DisplayPowerControlEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayPowerInfoEXT*                pDisplayPowerInfo);

VKAPI_ATTR VkResult VKAPI_CALL RegisterDeviceEventEXT(
    VkDevice                                    device,
    const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence);

VKAPI_ATTR VkResult VKAPI_CALL RegisterDisplayEventEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayEventInfoEXT*                pDisplayEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence);

VKAPI_ATTR VkResult VKAPI_CALL GetSwapchainCounterEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkSurfaceCounterFlagBitsEXT                 counter,
    uint64_t*                                   pCounterValue);


VKAPI_ATTR VkResult VKAPI_CALL GetRefreshCycleDurationGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkRefreshCycleDurationGOOGLE*               pDisplayTimingProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetPastPresentationTimingGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE*             pPresentationTimings);







VKAPI_ATTR void VKAPI_CALL CmdSetDiscardRectangleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstDiscardRectangle,
    uint32_t                                    discardRectangleCount,
    const VkRect2D*                             pDiscardRectangles);




VKAPI_ATTR void VKAPI_CALL SetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata);

#ifdef VK_USE_PLATFORM_IOS_MVK

VKAPI_ATTR VkResult VKAPI_CALL CreateIOSSurfaceMVK(
    VkInstance                                  instance,
    const VkIOSSurfaceCreateInfoMVK*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);
#endif /* VK_USE_PLATFORM_IOS_MVK */

#ifdef VK_USE_PLATFORM_MACOS_MVK

VKAPI_ATTR VkResult VKAPI_CALL CreateMacOSSurfaceMVK(
    VkInstance                                  instance,
    const VkMacOSSurfaceCreateInfoMVK*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface);
#endif /* VK_USE_PLATFORM_MACOS_MVK */




VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectNameEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectNameInfoEXT*        pNameInfo);

VKAPI_ATTR VkResult VKAPI_CALL SetDebugUtilsObjectTagEXT(
    VkDevice                                    device,
    const VkDebugUtilsObjectTagInfoEXT*         pTagInfo);

VKAPI_ATTR void VKAPI_CALL QueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);

VKAPI_ATTR void VKAPI_CALL QueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue);

VKAPI_ATTR void VKAPI_CALL QueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);

VKAPI_ATTR void VKAPI_CALL CmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);

VKAPI_ATTR void VKAPI_CALL CmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer);

VKAPI_ATTR void VKAPI_CALL CmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo);

VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger);

VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR void VKAPI_CALL SubmitDebugUtilsMessageEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);

#ifdef VK_USE_PLATFORM_ANDROID_KHR

VKAPI_ATTR VkResult VKAPI_CALL GetAndroidHardwareBufferPropertiesANDROID(
    VkDevice                                    device,
    const struct AHardwareBuffer*               buffer,
    VkAndroidHardwareBufferPropertiesANDROID*   pProperties);

VKAPI_ATTR VkResult VKAPI_CALL GetMemoryAndroidHardwareBufferANDROID(
    VkDevice                                    device,
    const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo,
    struct AHardwareBuffer**                    pBuffer);
#endif /* VK_USE_PLATFORM_ANDROID_KHR */







VKAPI_ATTR void VKAPI_CALL CmdSetSampleLocationsEXT(
    VkCommandBuffer                             commandBuffer,
    const VkSampleLocationsInfoEXT*             pSampleLocationsInfo);

VKAPI_ATTR void VKAPI_CALL GetPhysicalDeviceMultisamplePropertiesEXT(
    VkPhysicalDevice                            physicalDevice,
    VkSampleCountFlagBits                       samples,
    VkMultisamplePropertiesEXT*                 pMultisampleProperties);







VKAPI_ATTR VkResult VKAPI_CALL CreateValidationCacheEXT(
    VkDevice                                    device,
    const VkValidationCacheCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkValidationCacheEXT*                       pValidationCache);

VKAPI_ATTR void VKAPI_CALL DestroyValidationCacheEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    const VkAllocationCallbacks*                pAllocator);

VKAPI_ATTR VkResult VKAPI_CALL MergeValidationCachesEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        dstCache,
    uint32_t                                    srcCacheCount,
    const VkValidationCacheEXT*                 pSrcCaches);

VKAPI_ATTR VkResult VKAPI_CALL GetValidationCacheDataEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    size_t*                                     pDataSize,
    void*                                       pData);





VKAPI_ATTR VkResult VKAPI_CALL GetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties);


VKAPI_ATTR void VKAPI_CALL CmdWriteBufferMarkerAMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker);




} // namespace vulkan_layer_factory

// Layer Factory base class definition
class layer_factory {
    public:
        layer_factory(layer_factory *interceptor) {
            global_interceptor_list.emplace_back(this);
        };

        std::string layer_name = "VLF";

        // Pre/post hook point declarations
        bool Information(const std::string &message) {
            return log_msg(vlf_report_data, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, 0,
                           layer_name.c_str(), "%s", message.c_str());
        }

        bool PerformanceWarning(const std::string &message) {
            return log_msg(vlf_report_data, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT,
                           0, layer_name.c_str(), "%s", message.c_str());
        }

        bool Warning(const std::string &message) {
            return log_msg(vlf_report_data, VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, 0,
                           layer_name.c_str(), "%s", message.c_str());
        }

        bool Error(const std::string &message) {
            return log_msg(vlf_report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT, 0,
                           layer_name.c_str(), "%s", message.c_str());
        }

        void Breakpoint(void) {
#ifdef WIN32
            DebugBreak();
#else
            raise(SIGTRAP);
#endif
        }

        virtual void PreCallApiFunction(const char *api_name) {};
        virtual void PostCallApiFunction(const char *api_name) {};

        // Pre/post hook point declarations
        virtual VkResult PreCallCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) { PreCallApiFunction("vkCreateInstance"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) { PostCallApiFunction("vkCreateInstance"); return VK_SUCCESS; };
        virtual void PreCallDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyInstance");};
        virtual void PostCallDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyInstance");};
        virtual VkResult PreCallEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) { PreCallApiFunction("vkEnumeratePhysicalDevices"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) { PostCallApiFunction("vkEnumeratePhysicalDevices"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) { PreCallApiFunction("vkGetPhysicalDeviceFeatures");};
        virtual void PostCallGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) { PostCallApiFunction("vkGetPhysicalDeviceFeatures");};
        virtual void PreCallGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceFormatProperties");};
        virtual void PostCallGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceFormatProperties");};
        virtual VkResult PreCallGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceImageFormatProperties"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceImageFormatProperties"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceProperties");};
        virtual void PostCallGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceProperties");};
        virtual void PreCallGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) { PreCallApiFunction("vkGetPhysicalDeviceQueueFamilyProperties");};
        virtual void PostCallGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) { PostCallApiFunction("vkGetPhysicalDeviceQueueFamilyProperties");};
        virtual void PreCallGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) { PreCallApiFunction("vkGetPhysicalDeviceMemoryProperties");};
        virtual void PostCallGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) { PostCallApiFunction("vkGetPhysicalDeviceMemoryProperties");};
        virtual PFN_vkVoidFunction PreCallGetInstanceProcAddr(VkInstance instance, const char* pName) { PreCallApiFunction("vkGetInstanceProcAddr"); return nullptr; };
        virtual PFN_vkVoidFunction PostCallGetInstanceProcAddr(VkInstance instance, const char* pName) { PostCallApiFunction("vkGetInstanceProcAddr"); return nullptr; };
        virtual PFN_vkVoidFunction PreCallGetDeviceProcAddr(VkDevice device, const char* pName) { PreCallApiFunction("vkGetDeviceProcAddr"); return nullptr; };
        virtual PFN_vkVoidFunction PostCallGetDeviceProcAddr(VkDevice device, const char* pName) { PostCallApiFunction("vkGetDeviceProcAddr"); return nullptr; };
        virtual VkResult PreCallCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) { PreCallApiFunction("vkCreateDevice"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) { PostCallApiFunction("vkCreateDevice"); return VK_SUCCESS; };
        virtual void PreCallDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDevice");};
        virtual void PostCallDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDevice");};
        virtual VkResult PreCallEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) { PreCallApiFunction("vkEnumerateInstanceExtensionProperties"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) { PostCallApiFunction("vkEnumerateInstanceExtensionProperties"); return VK_SUCCESS; };
        virtual VkResult PreCallEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) { PreCallApiFunction("vkEnumerateDeviceExtensionProperties"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) { PostCallApiFunction("vkEnumerateDeviceExtensionProperties"); return VK_SUCCESS; };
        virtual VkResult PreCallEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) { PreCallApiFunction("vkEnumerateInstanceLayerProperties"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) { PostCallApiFunction("vkEnumerateInstanceLayerProperties"); return VK_SUCCESS; };
        virtual VkResult PreCallEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties) { PreCallApiFunction("vkEnumerateDeviceLayerProperties"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties) { PostCallApiFunction("vkEnumerateDeviceLayerProperties"); return VK_SUCCESS; };
        virtual void PreCallGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) { PreCallApiFunction("vkGetDeviceQueue");};
        virtual void PostCallGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) { PostCallApiFunction("vkGetDeviceQueue");};
        virtual VkResult PreCallQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) { PreCallApiFunction("vkQueueSubmit"); return VK_SUCCESS; };
        virtual VkResult PostCallQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) { PostCallApiFunction("vkQueueSubmit"); return VK_SUCCESS; };
        virtual VkResult PreCallQueueWaitIdle(VkQueue queue) { PreCallApiFunction("vkQueueWaitIdle"); return VK_SUCCESS; };
        virtual VkResult PostCallQueueWaitIdle(VkQueue queue) { PostCallApiFunction("vkQueueWaitIdle"); return VK_SUCCESS; };
        virtual VkResult PreCallDeviceWaitIdle(VkDevice device) { PreCallApiFunction("vkDeviceWaitIdle"); return VK_SUCCESS; };
        virtual VkResult PostCallDeviceWaitIdle(VkDevice device) { PostCallApiFunction("vkDeviceWaitIdle"); return VK_SUCCESS; };
        virtual VkResult PreCallAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) { PreCallApiFunction("vkAllocateMemory"); return VK_SUCCESS; };
        virtual VkResult PostCallAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) { PostCallApiFunction("vkAllocateMemory"); return VK_SUCCESS; };
        virtual void PreCallFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkFreeMemory");};
        virtual void PostCallFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkFreeMemory");};
        virtual VkResult PreCallMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) { PreCallApiFunction("vkMapMemory"); return VK_SUCCESS; };
        virtual VkResult PostCallMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) { PostCallApiFunction("vkMapMemory"); return VK_SUCCESS; };
        virtual void PreCallUnmapMemory(VkDevice device, VkDeviceMemory memory) { PreCallApiFunction("vkUnmapMemory");};
        virtual void PostCallUnmapMemory(VkDevice device, VkDeviceMemory memory) { PostCallApiFunction("vkUnmapMemory");};
        virtual VkResult PreCallFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) { PreCallApiFunction("vkFlushMappedMemoryRanges"); return VK_SUCCESS; };
        virtual VkResult PostCallFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) { PostCallApiFunction("vkFlushMappedMemoryRanges"); return VK_SUCCESS; };
        virtual VkResult PreCallInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) { PreCallApiFunction("vkInvalidateMappedMemoryRanges"); return VK_SUCCESS; };
        virtual VkResult PostCallInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) { PostCallApiFunction("vkInvalidateMappedMemoryRanges"); return VK_SUCCESS; };
        virtual void PreCallGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes) { PreCallApiFunction("vkGetDeviceMemoryCommitment");};
        virtual void PostCallGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes) { PostCallApiFunction("vkGetDeviceMemoryCommitment");};
        virtual VkResult PreCallBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) { PreCallApiFunction("vkBindBufferMemory"); return VK_SUCCESS; };
        virtual VkResult PostCallBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) { PostCallApiFunction("vkBindBufferMemory"); return VK_SUCCESS; };
        virtual VkResult PreCallBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) { PreCallApiFunction("vkBindImageMemory"); return VK_SUCCESS; };
        virtual VkResult PostCallBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) { PostCallApiFunction("vkBindImageMemory"); return VK_SUCCESS; };
        virtual void PreCallGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) { PreCallApiFunction("vkGetBufferMemoryRequirements");};
        virtual void PostCallGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) { PostCallApiFunction("vkGetBufferMemoryRequirements");};
        virtual void PreCallGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) { PreCallApiFunction("vkGetImageMemoryRequirements");};
        virtual void PostCallGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) { PostCallApiFunction("vkGetImageMemoryRequirements");};
        virtual void PreCallGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements) { PreCallApiFunction("vkGetImageSparseMemoryRequirements");};
        virtual void PostCallGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements) { PostCallApiFunction("vkGetImageSparseMemoryRequirements");};
        virtual void PreCallGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceSparseImageFormatProperties");};
        virtual void PostCallGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceSparseImageFormatProperties");};
        virtual VkResult PreCallQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence) { PreCallApiFunction("vkQueueBindSparse"); return VK_SUCCESS; };
        virtual VkResult PostCallQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence) { PostCallApiFunction("vkQueueBindSparse"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) { PreCallApiFunction("vkCreateFence"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) { PostCallApiFunction("vkCreateFence"); return VK_SUCCESS; };
        virtual void PreCallDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyFence");};
        virtual void PostCallDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyFence");};
        virtual VkResult PreCallResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) { PreCallApiFunction("vkResetFences"); return VK_SUCCESS; };
        virtual VkResult PostCallResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) { PostCallApiFunction("vkResetFences"); return VK_SUCCESS; };
        virtual VkResult PreCallGetFenceStatus(VkDevice device, VkFence fence) { PreCallApiFunction("vkGetFenceStatus"); return VK_SUCCESS; };
        virtual VkResult PostCallGetFenceStatus(VkDevice device, VkFence fence) { PostCallApiFunction("vkGetFenceStatus"); return VK_SUCCESS; };
        virtual VkResult PreCallWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) { PreCallApiFunction("vkWaitForFences"); return VK_SUCCESS; };
        virtual VkResult PostCallWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) { PostCallApiFunction("vkWaitForFences"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) { PreCallApiFunction("vkCreateSemaphore"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) { PostCallApiFunction("vkCreateSemaphore"); return VK_SUCCESS; };
        virtual void PreCallDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroySemaphore");};
        virtual void PostCallDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroySemaphore");};
        virtual VkResult PreCallCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) { PreCallApiFunction("vkCreateEvent"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) { PostCallApiFunction("vkCreateEvent"); return VK_SUCCESS; };
        virtual void PreCallDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyEvent");};
        virtual void PostCallDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyEvent");};
        virtual VkResult PreCallGetEventStatus(VkDevice device, VkEvent event) { PreCallApiFunction("vkGetEventStatus"); return VK_SUCCESS; };
        virtual VkResult PostCallGetEventStatus(VkDevice device, VkEvent event) { PostCallApiFunction("vkGetEventStatus"); return VK_SUCCESS; };
        virtual VkResult PreCallSetEvent(VkDevice device, VkEvent event) { PreCallApiFunction("vkSetEvent"); return VK_SUCCESS; };
        virtual VkResult PostCallSetEvent(VkDevice device, VkEvent event) { PostCallApiFunction("vkSetEvent"); return VK_SUCCESS; };
        virtual VkResult PreCallResetEvent(VkDevice device, VkEvent event) { PreCallApiFunction("vkResetEvent"); return VK_SUCCESS; };
        virtual VkResult PostCallResetEvent(VkDevice device, VkEvent event) { PostCallApiFunction("vkResetEvent"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) { PreCallApiFunction("vkCreateQueryPool"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) { PostCallApiFunction("vkCreateQueryPool"); return VK_SUCCESS; };
        virtual void PreCallDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyQueryPool");};
        virtual void PostCallDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyQueryPool");};
        virtual VkResult PreCallGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) { PreCallApiFunction("vkGetQueryPoolResults"); return VK_SUCCESS; };
        virtual VkResult PostCallGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) { PostCallApiFunction("vkGetQueryPoolResults"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) { PreCallApiFunction("vkCreateBuffer"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) { PostCallApiFunction("vkCreateBuffer"); return VK_SUCCESS; };
        virtual void PreCallDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyBuffer");};
        virtual void PostCallDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyBuffer");};
        virtual VkResult PreCallCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView) { PreCallApiFunction("vkCreateBufferView"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView) { PostCallApiFunction("vkCreateBufferView"); return VK_SUCCESS; };
        virtual void PreCallDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyBufferView");};
        virtual void PostCallDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyBufferView");};
        virtual VkResult PreCallCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) { PreCallApiFunction("vkCreateImage"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) { PostCallApiFunction("vkCreateImage"); return VK_SUCCESS; };
        virtual void PreCallDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyImage");};
        virtual void PostCallDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyImage");};
        virtual void PreCallGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) { PreCallApiFunction("vkGetImageSubresourceLayout");};
        virtual void PostCallGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) { PostCallApiFunction("vkGetImageSubresourceLayout");};
        virtual VkResult PreCallCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) { PreCallApiFunction("vkCreateImageView"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) { PostCallApiFunction("vkCreateImageView"); return VK_SUCCESS; };
        virtual void PreCallDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyImageView");};
        virtual void PostCallDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyImageView");};
        virtual VkResult PreCallCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) { PreCallApiFunction("vkCreateShaderModule"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) { PostCallApiFunction("vkCreateShaderModule"); return VK_SUCCESS; };
        virtual void PreCallDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyShaderModule");};
        virtual void PostCallDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyShaderModule");};
        virtual VkResult PreCallCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache) { PreCallApiFunction("vkCreatePipelineCache"); return VK_SUCCESS; };
        virtual VkResult PostCallCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache) { PostCallApiFunction("vkCreatePipelineCache"); return VK_SUCCESS; };
        virtual void PreCallDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyPipelineCache");};
        virtual void PostCallDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyPipelineCache");};
        virtual VkResult PreCallGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData) { PreCallApiFunction("vkGetPipelineCacheData"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData) { PostCallApiFunction("vkGetPipelineCacheData"); return VK_SUCCESS; };
        virtual VkResult PreCallMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) { PreCallApiFunction("vkMergePipelineCaches"); return VK_SUCCESS; };
        virtual VkResult PostCallMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) { PostCallApiFunction("vkMergePipelineCaches"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) { PreCallApiFunction("vkCreateGraphicsPipelines"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) { PostCallApiFunction("vkCreateGraphicsPipelines"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) { PreCallApiFunction("vkCreateComputePipelines"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) { PostCallApiFunction("vkCreateComputePipelines"); return VK_SUCCESS; };
        virtual void PreCallDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyPipeline");};
        virtual void PostCallDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyPipeline");};
        virtual VkResult PreCallCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) { PreCallApiFunction("vkCreatePipelineLayout"); return VK_SUCCESS; };
        virtual VkResult PostCallCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) { PostCallApiFunction("vkCreatePipelineLayout"); return VK_SUCCESS; };
        virtual void PreCallDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyPipelineLayout");};
        virtual void PostCallDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyPipelineLayout");};
        virtual VkResult PreCallCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) { PreCallApiFunction("vkCreateSampler"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) { PostCallApiFunction("vkCreateSampler"); return VK_SUCCESS; };
        virtual void PreCallDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroySampler");};
        virtual void PostCallDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroySampler");};
        virtual VkResult PreCallCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) { PreCallApiFunction("vkCreateDescriptorSetLayout"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) { PostCallApiFunction("vkCreateDescriptorSetLayout"); return VK_SUCCESS; };
        virtual void PreCallDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDescriptorSetLayout");};
        virtual void PostCallDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDescriptorSetLayout");};
        virtual VkResult PreCallCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) { PreCallApiFunction("vkCreateDescriptorPool"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) { PostCallApiFunction("vkCreateDescriptorPool"); return VK_SUCCESS; };
        virtual void PreCallDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDescriptorPool");};
        virtual void PostCallDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDescriptorPool");};
        virtual VkResult PreCallResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) { PreCallApiFunction("vkResetDescriptorPool"); return VK_SUCCESS; };
        virtual VkResult PostCallResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) { PostCallApiFunction("vkResetDescriptorPool"); return VK_SUCCESS; };
        virtual VkResult PreCallAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) { PreCallApiFunction("vkAllocateDescriptorSets"); return VK_SUCCESS; };
        virtual VkResult PostCallAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) { PostCallApiFunction("vkAllocateDescriptorSets"); return VK_SUCCESS; };
        virtual VkResult PreCallFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) { PreCallApiFunction("vkFreeDescriptorSets"); return VK_SUCCESS; };
        virtual VkResult PostCallFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) { PostCallApiFunction("vkFreeDescriptorSets"); return VK_SUCCESS; };
        virtual void PreCallUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) { PreCallApiFunction("vkUpdateDescriptorSets");};
        virtual void PostCallUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) { PostCallApiFunction("vkUpdateDescriptorSets");};
        virtual VkResult PreCallCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) { PreCallApiFunction("vkCreateFramebuffer"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) { PostCallApiFunction("vkCreateFramebuffer"); return VK_SUCCESS; };
        virtual void PreCallDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyFramebuffer");};
        virtual void PostCallDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyFramebuffer");};
        virtual VkResult PreCallCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) { PreCallApiFunction("vkCreateRenderPass"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) { PostCallApiFunction("vkCreateRenderPass"); return VK_SUCCESS; };
        virtual void PreCallDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyRenderPass");};
        virtual void PostCallDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyRenderPass");};
        virtual void PreCallGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity) { PreCallApiFunction("vkGetRenderAreaGranularity");};
        virtual void PostCallGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity) { PostCallApiFunction("vkGetRenderAreaGranularity");};
        virtual VkResult PreCallCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) { PreCallApiFunction("vkCreateCommandPool"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) { PostCallApiFunction("vkCreateCommandPool"); return VK_SUCCESS; };
        virtual void PreCallDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyCommandPool");};
        virtual void PostCallDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyCommandPool");};
        virtual VkResult PreCallResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) { PreCallApiFunction("vkResetCommandPool"); return VK_SUCCESS; };
        virtual VkResult PostCallResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) { PostCallApiFunction("vkResetCommandPool"); return VK_SUCCESS; };
        virtual VkResult PreCallAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) { PreCallApiFunction("vkAllocateCommandBuffers"); return VK_SUCCESS; };
        virtual VkResult PostCallAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) { PostCallApiFunction("vkAllocateCommandBuffers"); return VK_SUCCESS; };
        virtual void PreCallFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) { PreCallApiFunction("vkFreeCommandBuffers");};
        virtual void PostCallFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) { PostCallApiFunction("vkFreeCommandBuffers");};
        virtual VkResult PreCallBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) { PreCallApiFunction("vkBeginCommandBuffer"); return VK_SUCCESS; };
        virtual VkResult PostCallBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) { PostCallApiFunction("vkBeginCommandBuffer"); return VK_SUCCESS; };
        virtual VkResult PreCallEndCommandBuffer(VkCommandBuffer commandBuffer) { PreCallApiFunction("vkEndCommandBuffer"); return VK_SUCCESS; };
        virtual VkResult PostCallEndCommandBuffer(VkCommandBuffer commandBuffer) { PostCallApiFunction("vkEndCommandBuffer"); return VK_SUCCESS; };
        virtual VkResult PreCallResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) { PreCallApiFunction("vkResetCommandBuffer"); return VK_SUCCESS; };
        virtual VkResult PostCallResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) { PostCallApiFunction("vkResetCommandBuffer"); return VK_SUCCESS; };
        virtual void PreCallCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) { PreCallApiFunction("vkCmdBindPipeline");};
        virtual void PostCallCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) { PostCallApiFunction("vkCmdBindPipeline");};
        virtual void PreCallCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) { PreCallApiFunction("vkCmdSetViewport");};
        virtual void PostCallCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) { PostCallApiFunction("vkCmdSetViewport");};
        virtual void PreCallCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) { PreCallApiFunction("vkCmdSetScissor");};
        virtual void PostCallCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) { PostCallApiFunction("vkCmdSetScissor");};
        virtual void PreCallCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) { PreCallApiFunction("vkCmdSetLineWidth");};
        virtual void PostCallCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) { PostCallApiFunction("vkCmdSetLineWidth");};
        virtual void PreCallCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) { PreCallApiFunction("vkCmdSetDepthBias");};
        virtual void PostCallCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) { PostCallApiFunction("vkCmdSetDepthBias");};
        virtual void PreCallCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) { PreCallApiFunction("vkCmdSetBlendConstants");};
        virtual void PostCallCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) { PostCallApiFunction("vkCmdSetBlendConstants");};
        virtual void PreCallCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) { PreCallApiFunction("vkCmdSetDepthBounds");};
        virtual void PostCallCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) { PostCallApiFunction("vkCmdSetDepthBounds");};
        virtual void PreCallCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask) { PreCallApiFunction("vkCmdSetStencilCompareMask");};
        virtual void PostCallCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask) { PostCallApiFunction("vkCmdSetStencilCompareMask");};
        virtual void PreCallCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask) { PreCallApiFunction("vkCmdSetStencilWriteMask");};
        virtual void PostCallCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask) { PostCallApiFunction("vkCmdSetStencilWriteMask");};
        virtual void PreCallCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference) { PreCallApiFunction("vkCmdSetStencilReference");};
        virtual void PostCallCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference) { PostCallApiFunction("vkCmdSetStencilReference");};
        virtual void PreCallCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) { PreCallApiFunction("vkCmdBindDescriptorSets");};
        virtual void PostCallCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) { PostCallApiFunction("vkCmdBindDescriptorSets");};
        virtual void PreCallCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) { PreCallApiFunction("vkCmdBindIndexBuffer");};
        virtual void PostCallCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) { PostCallApiFunction("vkCmdBindIndexBuffer");};
        virtual void PreCallCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) { PreCallApiFunction("vkCmdBindVertexBuffers");};
        virtual void PostCallCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) { PostCallApiFunction("vkCmdBindVertexBuffers");};
        virtual void PreCallCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) { PreCallApiFunction("vkCmdDraw");};
        virtual void PostCallCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) { PostCallApiFunction("vkCmdDraw");};
        virtual void PreCallCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) { PreCallApiFunction("vkCmdDrawIndexed");};
        virtual void PostCallCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) { PostCallApiFunction("vkCmdDrawIndexed");};
        virtual void PreCallCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) { PreCallApiFunction("vkCmdDrawIndirect");};
        virtual void PostCallCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) { PostCallApiFunction("vkCmdDrawIndirect");};
        virtual void PreCallCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) { PreCallApiFunction("vkCmdDrawIndexedIndirect");};
        virtual void PostCallCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) { PostCallApiFunction("vkCmdDrawIndexedIndirect");};
        virtual void PreCallCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { PreCallApiFunction("vkCmdDispatch");};
        virtual void PostCallCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { PostCallApiFunction("vkCmdDispatch");};
        virtual void PreCallCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) { PreCallApiFunction("vkCmdDispatchIndirect");};
        virtual void PostCallCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) { PostCallApiFunction("vkCmdDispatchIndirect");};
        virtual void PreCallCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) { PreCallApiFunction("vkCmdCopyBuffer");};
        virtual void PostCallCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) { PostCallApiFunction("vkCmdCopyBuffer");};
        virtual void PreCallCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions) { PreCallApiFunction("vkCmdCopyImage");};
        virtual void PostCallCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions) { PostCallApiFunction("vkCmdCopyImage");};
        virtual void PreCallCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter) { PreCallApiFunction("vkCmdBlitImage");};
        virtual void PostCallCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter) { PostCallApiFunction("vkCmdBlitImage");};
        virtual void PreCallCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) { PreCallApiFunction("vkCmdCopyBufferToImage");};
        virtual void PostCallCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) { PostCallApiFunction("vkCmdCopyBufferToImage");};
        virtual void PreCallCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions) { PreCallApiFunction("vkCmdCopyImageToBuffer");};
        virtual void PostCallCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions) { PostCallApiFunction("vkCmdCopyImageToBuffer");};
        virtual void PreCallCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) { PreCallApiFunction("vkCmdUpdateBuffer");};
        virtual void PostCallCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) { PostCallApiFunction("vkCmdUpdateBuffer");};
        virtual void PreCallCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) { PreCallApiFunction("vkCmdFillBuffer");};
        virtual void PostCallCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) { PostCallApiFunction("vkCmdFillBuffer");};
        virtual void PreCallCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) { PreCallApiFunction("vkCmdClearColorImage");};
        virtual void PostCallCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) { PostCallApiFunction("vkCmdClearColorImage");};
        virtual void PreCallCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) { PreCallApiFunction("vkCmdClearDepthStencilImage");};
        virtual void PostCallCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) { PostCallApiFunction("vkCmdClearDepthStencilImage");};
        virtual void PreCallCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects) { PreCallApiFunction("vkCmdClearAttachments");};
        virtual void PostCallCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects) { PostCallApiFunction("vkCmdClearAttachments");};
        virtual void PreCallCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions) { PreCallApiFunction("vkCmdResolveImage");};
        virtual void PostCallCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions) { PostCallApiFunction("vkCmdResolveImage");};
        virtual void PreCallCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) { PreCallApiFunction("vkCmdSetEvent");};
        virtual void PostCallCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) { PostCallApiFunction("vkCmdSetEvent");};
        virtual void PreCallCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) { PreCallApiFunction("vkCmdResetEvent");};
        virtual void PostCallCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) { PostCallApiFunction("vkCmdResetEvent");};
        virtual void PreCallCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) { PreCallApiFunction("vkCmdWaitEvents");};
        virtual void PostCallCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) { PostCallApiFunction("vkCmdWaitEvents");};
        virtual void PreCallCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) { PreCallApiFunction("vkCmdPipelineBarrier");};
        virtual void PostCallCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) { PostCallApiFunction("vkCmdPipelineBarrier");};
        virtual void PreCallCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) { PreCallApiFunction("vkCmdBeginQuery");};
        virtual void PostCallCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) { PostCallApiFunction("vkCmdBeginQuery");};
        virtual void PreCallCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) { PreCallApiFunction("vkCmdEndQuery");};
        virtual void PostCallCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) { PostCallApiFunction("vkCmdEndQuery");};
        virtual void PreCallCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) { PreCallApiFunction("vkCmdResetQueryPool");};
        virtual void PostCallCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) { PostCallApiFunction("vkCmdResetQueryPool");};
        virtual void PreCallCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) { PreCallApiFunction("vkCmdWriteTimestamp");};
        virtual void PostCallCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) { PostCallApiFunction("vkCmdWriteTimestamp");};
        virtual void PreCallCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) { PreCallApiFunction("vkCmdCopyQueryPoolResults");};
        virtual void PostCallCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) { PostCallApiFunction("vkCmdCopyQueryPoolResults");};
        virtual void PreCallCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) { PreCallApiFunction("vkCmdPushConstants");};
        virtual void PostCallCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) { PostCallApiFunction("vkCmdPushConstants");};
        virtual void PreCallCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) { PreCallApiFunction("vkCmdBeginRenderPass");};
        virtual void PostCallCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) { PostCallApiFunction("vkCmdBeginRenderPass");};
        virtual void PreCallCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) { PreCallApiFunction("vkCmdNextSubpass");};
        virtual void PostCallCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) { PostCallApiFunction("vkCmdNextSubpass");};
        virtual void PreCallCmdEndRenderPass(VkCommandBuffer commandBuffer) { PreCallApiFunction("vkCmdEndRenderPass");};
        virtual void PostCallCmdEndRenderPass(VkCommandBuffer commandBuffer) { PostCallApiFunction("vkCmdEndRenderPass");};
        virtual void PreCallCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) { PreCallApiFunction("vkCmdExecuteCommands");};
        virtual void PostCallCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) { PostCallApiFunction("vkCmdExecuteCommands");};
        virtual VkResult PreCallBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos) { PreCallApiFunction("vkBindBufferMemory2"); return VK_SUCCESS; };
        virtual VkResult PostCallBindBufferMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos) { PostCallApiFunction("vkBindBufferMemory2"); return VK_SUCCESS; };
        virtual VkResult PreCallBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos) { PreCallApiFunction("vkBindImageMemory2"); return VK_SUCCESS; };
        virtual VkResult PostCallBindImageMemory2(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos) { PostCallApiFunction("vkBindImageMemory2"); return VK_SUCCESS; };
        virtual void PreCallGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) { PreCallApiFunction("vkGetDeviceGroupPeerMemoryFeatures");};
        virtual void PostCallGetDeviceGroupPeerMemoryFeatures(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) { PostCallApiFunction("vkGetDeviceGroupPeerMemoryFeatures");};
        virtual void PreCallCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask) { PreCallApiFunction("vkCmdSetDeviceMask");};
        virtual void PostCallCmdSetDeviceMask(VkCommandBuffer commandBuffer, uint32_t deviceMask) { PostCallApiFunction("vkCmdSetDeviceMask");};
        virtual void PreCallCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { PreCallApiFunction("vkCmdDispatchBase");};
        virtual void PostCallCmdDispatchBase(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { PostCallApiFunction("vkCmdDispatchBase");};
        virtual VkResult PreCallEnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) { PreCallApiFunction("vkEnumeratePhysicalDeviceGroups"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumeratePhysicalDeviceGroups(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) { PostCallApiFunction("vkEnumeratePhysicalDeviceGroups"); return VK_SUCCESS; };
        virtual void PreCallGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PreCallApiFunction("vkGetImageMemoryRequirements2");};
        virtual void PostCallGetImageMemoryRequirements2(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PostCallApiFunction("vkGetImageMemoryRequirements2");};
        virtual void PreCallGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PreCallApiFunction("vkGetBufferMemoryRequirements2");};
        virtual void PostCallGetBufferMemoryRequirements2(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PostCallApiFunction("vkGetBufferMemoryRequirements2");};
        virtual void PreCallGetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) { PreCallApiFunction("vkGetImageSparseMemoryRequirements2");};
        virtual void PostCallGetImageSparseMemoryRequirements2(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) { PostCallApiFunction("vkGetImageSparseMemoryRequirements2");};
        virtual void PreCallGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) { PreCallApiFunction("vkGetPhysicalDeviceFeatures2");};
        virtual void PostCallGetPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) { PostCallApiFunction("vkGetPhysicalDeviceFeatures2");};
        virtual void PreCallGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceProperties2");};
        virtual void PostCallGetPhysicalDeviceProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceProperties2");};
        virtual void PreCallGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceFormatProperties2");};
        virtual void PostCallGetPhysicalDeviceFormatProperties2(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceFormatProperties2");};
        virtual VkResult PreCallGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceImageFormatProperties2"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceImageFormatProperties2"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) { PreCallApiFunction("vkGetPhysicalDeviceQueueFamilyProperties2");};
        virtual void PostCallGetPhysicalDeviceQueueFamilyProperties2(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) { PostCallApiFunction("vkGetPhysicalDeviceQueueFamilyProperties2");};
        virtual void PreCallGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) { PreCallApiFunction("vkGetPhysicalDeviceMemoryProperties2");};
        virtual void PostCallGetPhysicalDeviceMemoryProperties2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) { PostCallApiFunction("vkGetPhysicalDeviceMemoryProperties2");};
        virtual void PreCallGetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceSparseImageFormatProperties2");};
        virtual void PostCallGetPhysicalDeviceSparseImageFormatProperties2(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceSparseImageFormatProperties2");};
        virtual void PreCallTrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags) { PreCallApiFunction("vkTrimCommandPool");};
        virtual void PostCallTrimCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags) { PostCallApiFunction("vkTrimCommandPool");};
        virtual void PreCallGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) { PreCallApiFunction("vkGetDeviceQueue2");};
        virtual void PostCallGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) { PostCallApiFunction("vkGetDeviceQueue2");};
        virtual VkResult PreCallCreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion) { PreCallApiFunction("vkCreateSamplerYcbcrConversion"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateSamplerYcbcrConversion(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion) { PostCallApiFunction("vkCreateSamplerYcbcrConversion"); return VK_SUCCESS; };
        virtual void PreCallDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroySamplerYcbcrConversion");};
        virtual void PostCallDestroySamplerYcbcrConversion(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroySamplerYcbcrConversion");};
        virtual VkResult PreCallCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) { PreCallApiFunction("vkCreateDescriptorUpdateTemplate"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDescriptorUpdateTemplate(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) { PostCallApiFunction("vkCreateDescriptorUpdateTemplate"); return VK_SUCCESS; };
        virtual void PreCallDestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDescriptorUpdateTemplate");};
        virtual void PostCallDestroyDescriptorUpdateTemplate(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDescriptorUpdateTemplate");};
        virtual void PreCallUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData) { PreCallApiFunction("vkUpdateDescriptorSetWithTemplate");};
        virtual void PostCallUpdateDescriptorSetWithTemplate(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData) { PostCallApiFunction("vkUpdateDescriptorSetWithTemplate");};
        virtual void PreCallGetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalBufferProperties");};
        virtual void PostCallGetPhysicalDeviceExternalBufferProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalBufferProperties");};
        virtual void PreCallGetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalFenceProperties");};
        virtual void PostCallGetPhysicalDeviceExternalFenceProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalFenceProperties");};
        virtual void PreCallGetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalSemaphoreProperties");};
        virtual void PostCallGetPhysicalDeviceExternalSemaphoreProperties(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalSemaphoreProperties");};
        virtual void PreCallGetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport) { PreCallApiFunction("vkGetDescriptorSetLayoutSupport");};
        virtual void PostCallGetDescriptorSetLayoutSupport(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport) { PostCallApiFunction("vkGetDescriptorSetLayoutSupport");};
        virtual void PreCallDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroySurfaceKHR");};
        virtual void PostCallDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroySurfaceKHR");};
        virtual VkResult PreCallGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported) { PreCallApiFunction("vkGetPhysicalDeviceSurfaceSupportKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported) { PostCallApiFunction("vkGetPhysicalDeviceSurfaceSupportKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) { PreCallApiFunction("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) { PostCallApiFunction("vkGetPhysicalDeviceSurfaceCapabilitiesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) { PreCallApiFunction("vkGetPhysicalDeviceSurfaceFormatsKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats) { PostCallApiFunction("vkGetPhysicalDeviceSurfaceFormatsKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) { PreCallApiFunction("vkGetPhysicalDeviceSurfacePresentModesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes) { PostCallApiFunction("vkGetPhysicalDeviceSurfacePresentModesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) { PreCallApiFunction("vkCreateSwapchainKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) { PostCallApiFunction("vkCreateSwapchainKHR"); return VK_SUCCESS; };
        virtual void PreCallDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroySwapchainKHR");};
        virtual void PostCallDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroySwapchainKHR");};
        virtual VkResult PreCallGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) { PreCallApiFunction("vkGetSwapchainImagesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) { PostCallApiFunction("vkGetSwapchainImagesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) { PreCallApiFunction("vkAcquireNextImageKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) { PostCallApiFunction("vkAcquireNextImageKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) { PreCallApiFunction("vkQueuePresentKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) { PostCallApiFunction("vkQueuePresentKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) { PreCallApiFunction("vkGetDeviceGroupPresentCapabilitiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetDeviceGroupPresentCapabilitiesKHR(VkDevice device, VkDeviceGroupPresentCapabilitiesKHR* pDeviceGroupPresentCapabilities) { PostCallApiFunction("vkGetDeviceGroupPresentCapabilitiesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR* pModes) { PreCallApiFunction("vkGetDeviceGroupSurfacePresentModesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetDeviceGroupSurfacePresentModesKHR(VkDevice device, VkSurfaceKHR surface, VkDeviceGroupPresentModeFlagsKHR* pModes) { PostCallApiFunction("vkGetDeviceGroupSurfacePresentModesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects) { PreCallApiFunction("vkGetPhysicalDevicePresentRectanglesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDevicePresentRectanglesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pRectCount, VkRect2D* pRects) { PostCallApiFunction("vkGetPhysicalDevicePresentRectanglesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex) { PreCallApiFunction("vkAcquireNextImage2KHR"); return VK_SUCCESS; };
        virtual VkResult PostCallAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex) { PostCallApiFunction("vkAcquireNextImage2KHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPropertiesKHR* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceDisplayPropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceDisplayPropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPropertiesKHR* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceDisplayPropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceDisplayPlanePropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceDisplayPlanePropertiesKHR(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkDisplayPlanePropertiesKHR* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceDisplayPlanePropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex, uint32_t* pDisplayCount, VkDisplayKHR* pDisplays) { PreCallApiFunction("vkGetDisplayPlaneSupportedDisplaysKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetDisplayPlaneSupportedDisplaysKHR(VkPhysicalDevice physicalDevice, uint32_t planeIndex, uint32_t* pDisplayCount, VkDisplayKHR* pDisplays) { PostCallApiFunction("vkGetDisplayPlaneSupportedDisplaysKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, uint32_t* pPropertyCount, VkDisplayModePropertiesKHR* pProperties) { PreCallApiFunction("vkGetDisplayModePropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetDisplayModePropertiesKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, uint32_t* pPropertyCount, VkDisplayModePropertiesKHR* pProperties) { PostCallApiFunction("vkGetDisplayModePropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, const VkDisplayModeCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode) { PreCallApiFunction("vkCreateDisplayModeKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDisplayModeKHR(VkPhysicalDevice physicalDevice, VkDisplayKHR display, const VkDisplayModeCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDisplayModeKHR* pMode) { PostCallApiFunction("vkCreateDisplayModeKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode, uint32_t planeIndex, VkDisplayPlaneCapabilitiesKHR* pCapabilities) { PreCallApiFunction("vkGetDisplayPlaneCapabilitiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetDisplayPlaneCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkDisplayModeKHR mode, uint32_t planeIndex, VkDisplayPlaneCapabilitiesKHR* pCapabilities) { PostCallApiFunction("vkGetDisplayPlaneCapabilitiesKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateDisplayPlaneSurfaceKHR(VkInstance instance, const VkDisplaySurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateDisplayPlaneSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDisplayPlaneSurfaceKHR(VkInstance instance, const VkDisplaySurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateDisplayPlaneSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallCreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount, const VkSwapchainCreateInfoKHR* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchains) { PreCallApiFunction("vkCreateSharedSwapchainsKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateSharedSwapchainsKHR(VkDevice device, uint32_t swapchainCount, const VkSwapchainCreateInfoKHR* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchains) { PostCallApiFunction("vkCreateSharedSwapchainsKHR"); return VK_SUCCESS; };
#ifdef VK_USE_PLATFORM_XLIB_KHR
        virtual VkResult PreCallCreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateXlibSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateXlibSurfaceKHR(VkInstance instance, const VkXlibSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateXlibSurfaceKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
        virtual VkBool32 PreCallGetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, Display* dpy, VisualID visualID) { PreCallApiFunction("vkGetPhysicalDeviceXlibPresentationSupportKHR"); return VK_TRUE; };
        virtual VkBool32 PostCallGetPhysicalDeviceXlibPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, Display* dpy, VisualID visualID) { PostCallApiFunction("vkGetPhysicalDeviceXlibPresentationSupportKHR"); return VK_TRUE; };
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
        virtual VkResult PreCallCreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateXcbSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateXcbSurfaceKHR(VkInstance instance, const VkXcbSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateXcbSurfaceKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
        virtual VkBool32 PreCallGetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, xcb_connection_t* connection, xcb_visualid_t visual_id) { PreCallApiFunction("vkGetPhysicalDeviceXcbPresentationSupportKHR"); return VK_TRUE; };
        virtual VkBool32 PostCallGetPhysicalDeviceXcbPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, xcb_connection_t* connection, xcb_visualid_t visual_id) { PostCallApiFunction("vkGetPhysicalDeviceXcbPresentationSupportKHR"); return VK_TRUE; };
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        virtual VkResult PreCallCreateWaylandSurfaceKHR(VkInstance instance, const VkWaylandSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateWaylandSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateWaylandSurfaceKHR(VkInstance instance, const VkWaylandSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateWaylandSurfaceKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
        virtual VkBool32 PreCallGetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, struct wl_display* display) { PreCallApiFunction("vkGetPhysicalDeviceWaylandPresentationSupportKHR"); return VK_TRUE; };
        virtual VkBool32 PostCallGetPhysicalDeviceWaylandPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, struct wl_display* display) { PostCallApiFunction("vkGetPhysicalDeviceWaylandPresentationSupportKHR"); return VK_TRUE; };
#endif
#ifdef VK_USE_PLATFORM_MIR_KHR
        virtual VkResult PreCallCreateMirSurfaceKHR(VkInstance instance, const VkMirSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateMirSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateMirSurfaceKHR(VkInstance instance, const VkMirSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateMirSurfaceKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_MIR_KHR
        virtual VkBool32 PreCallGetPhysicalDeviceMirPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, MirConnection* connection) { PreCallApiFunction("vkGetPhysicalDeviceMirPresentationSupportKHR"); return VK_TRUE; };
        virtual VkBool32 PostCallGetPhysicalDeviceMirPresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, MirConnection* connection) { PostCallApiFunction("vkGetPhysicalDeviceMirPresentationSupportKHR"); return VK_TRUE; };
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        virtual VkResult PreCallCreateAndroidSurfaceKHR(VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateAndroidSurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateAndroidSurfaceKHR(VkInstance instance, const VkAndroidSurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateAndroidSurfaceKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateWin32SurfaceKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateWin32SurfaceKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkBool32 PreCallGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) { PreCallApiFunction("vkGetPhysicalDeviceWin32PresentationSupportKHR"); return VK_TRUE; };
        virtual VkBool32 PostCallGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) { PostCallApiFunction("vkGetPhysicalDeviceWin32PresentationSupportKHR"); return VK_TRUE; };
#endif
        virtual void PreCallGetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) { PreCallApiFunction("vkGetPhysicalDeviceFeatures2KHR");};
        virtual void PostCallGetPhysicalDeviceFeatures2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) { PostCallApiFunction("vkGetPhysicalDeviceFeatures2KHR");};
        virtual void PreCallGetPhysicalDeviceProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceProperties2KHR");};
        virtual void PostCallGetPhysicalDeviceProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties2* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceProperties2KHR");};
        virtual void PreCallGetPhysicalDeviceFormatProperties2KHR(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceFormatProperties2KHR");};
        virtual void PostCallGetPhysicalDeviceFormatProperties2KHR(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties2* pFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceFormatProperties2KHR");};
        virtual VkResult PreCallGetPhysicalDeviceImageFormatProperties2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceImageFormatProperties2KHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceImageFormatProperties2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo, VkImageFormatProperties2* pImageFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceImageFormatProperties2KHR"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceQueueFamilyProperties2KHR(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) { PreCallApiFunction("vkGetPhysicalDeviceQueueFamilyProperties2KHR");};
        virtual void PostCallGetPhysicalDeviceQueueFamilyProperties2KHR(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties2* pQueueFamilyProperties) { PostCallApiFunction("vkGetPhysicalDeviceQueueFamilyProperties2KHR");};
        virtual void PreCallGetPhysicalDeviceMemoryProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) { PreCallApiFunction("vkGetPhysicalDeviceMemoryProperties2KHR");};
        virtual void PostCallGetPhysicalDeviceMemoryProperties2KHR(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* pMemoryProperties) { PostCallApiFunction("vkGetPhysicalDeviceMemoryProperties2KHR");};
        virtual void PreCallGetPhysicalDeviceSparseImageFormatProperties2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties) { PreCallApiFunction("vkGetPhysicalDeviceSparseImageFormatProperties2KHR");};
        virtual void PostCallGetPhysicalDeviceSparseImageFormatProperties2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSparseImageFormatInfo2* pFormatInfo, uint32_t* pPropertyCount, VkSparseImageFormatProperties2* pProperties) { PostCallApiFunction("vkGetPhysicalDeviceSparseImageFormatProperties2KHR");};
        virtual void PreCallGetDeviceGroupPeerMemoryFeaturesKHR(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) { PreCallApiFunction("vkGetDeviceGroupPeerMemoryFeaturesKHR");};
        virtual void PostCallGetDeviceGroupPeerMemoryFeaturesKHR(VkDevice device, uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex, VkPeerMemoryFeatureFlags* pPeerMemoryFeatures) { PostCallApiFunction("vkGetDeviceGroupPeerMemoryFeaturesKHR");};
        virtual void PreCallCmdSetDeviceMaskKHR(VkCommandBuffer commandBuffer, uint32_t deviceMask) { PreCallApiFunction("vkCmdSetDeviceMaskKHR");};
        virtual void PostCallCmdSetDeviceMaskKHR(VkCommandBuffer commandBuffer, uint32_t deviceMask) { PostCallApiFunction("vkCmdSetDeviceMaskKHR");};
        virtual void PreCallCmdDispatchBaseKHR(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { PreCallApiFunction("vkCmdDispatchBaseKHR");};
        virtual void PostCallCmdDispatchBaseKHR(VkCommandBuffer commandBuffer, uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) { PostCallApiFunction("vkCmdDispatchBaseKHR");};
        virtual void PreCallTrimCommandPoolKHR(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags) { PreCallApiFunction("vkTrimCommandPoolKHR");};
        virtual void PostCallTrimCommandPoolKHR(VkDevice device, VkCommandPool commandPool, VkCommandPoolTrimFlags flags) { PostCallApiFunction("vkTrimCommandPoolKHR");};
        virtual VkResult PreCallEnumeratePhysicalDeviceGroupsKHR(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) { PreCallApiFunction("vkEnumeratePhysicalDeviceGroupsKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallEnumeratePhysicalDeviceGroupsKHR(VkInstance instance, uint32_t* pPhysicalDeviceGroupCount, VkPhysicalDeviceGroupProperties* pPhysicalDeviceGroupProperties) { PostCallApiFunction("vkEnumeratePhysicalDeviceGroupsKHR"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceExternalBufferPropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalBufferPropertiesKHR");};
        virtual void PostCallGetPhysicalDeviceExternalBufferPropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalBufferInfo* pExternalBufferInfo, VkExternalBufferProperties* pExternalBufferProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalBufferPropertiesKHR");};
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallGetMemoryWin32HandleKHR(VkDevice device, const VkMemoryGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) { PreCallApiFunction("vkGetMemoryWin32HandleKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryWin32HandleKHR(VkDevice device, const VkMemoryGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) { PostCallApiFunction("vkGetMemoryWin32HandleKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallGetMemoryWin32HandlePropertiesKHR(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, HANDLE handle, VkMemoryWin32HandlePropertiesKHR* pMemoryWin32HandleProperties) { PreCallApiFunction("vkGetMemoryWin32HandlePropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryWin32HandlePropertiesKHR(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, HANDLE handle, VkMemoryWin32HandlePropertiesKHR* pMemoryWin32HandleProperties) { PostCallApiFunction("vkGetMemoryWin32HandlePropertiesKHR"); return VK_SUCCESS; };
#endif
        virtual VkResult PreCallGetMemoryFdKHR(VkDevice device, const VkMemoryGetFdInfoKHR* pGetFdInfo, int* pFd) { PreCallApiFunction("vkGetMemoryFdKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryFdKHR(VkDevice device, const VkMemoryGetFdInfoKHR* pGetFdInfo, int* pFd) { PostCallApiFunction("vkGetMemoryFdKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetMemoryFdPropertiesKHR(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, int fd, VkMemoryFdPropertiesKHR* pMemoryFdProperties) { PreCallApiFunction("vkGetMemoryFdPropertiesKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryFdPropertiesKHR(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, int fd, VkMemoryFdPropertiesKHR* pMemoryFdProperties) { PostCallApiFunction("vkGetMemoryFdPropertiesKHR"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceExternalSemaphorePropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");};
        virtual void PostCallGetPhysicalDeviceExternalSemaphorePropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalSemaphoreInfo* pExternalSemaphoreInfo, VkExternalSemaphoreProperties* pExternalSemaphoreProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalSemaphorePropertiesKHR");};
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallImportSemaphoreWin32HandleKHR(VkDevice device, const VkImportSemaphoreWin32HandleInfoKHR* pImportSemaphoreWin32HandleInfo) { PreCallApiFunction("vkImportSemaphoreWin32HandleKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallImportSemaphoreWin32HandleKHR(VkDevice device, const VkImportSemaphoreWin32HandleInfoKHR* pImportSemaphoreWin32HandleInfo) { PostCallApiFunction("vkImportSemaphoreWin32HandleKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallGetSemaphoreWin32HandleKHR(VkDevice device, const VkSemaphoreGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) { PreCallApiFunction("vkGetSemaphoreWin32HandleKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetSemaphoreWin32HandleKHR(VkDevice device, const VkSemaphoreGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) { PostCallApiFunction("vkGetSemaphoreWin32HandleKHR"); return VK_SUCCESS; };
#endif
        virtual VkResult PreCallImportSemaphoreFdKHR(VkDevice device, const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo) { PreCallApiFunction("vkImportSemaphoreFdKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallImportSemaphoreFdKHR(VkDevice device, const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo) { PostCallApiFunction("vkImportSemaphoreFdKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetSemaphoreFdKHR(VkDevice device, const VkSemaphoreGetFdInfoKHR* pGetFdInfo, int* pFd) { PreCallApiFunction("vkGetSemaphoreFdKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetSemaphoreFdKHR(VkDevice device, const VkSemaphoreGetFdInfoKHR* pGetFdInfo, int* pFd) { PostCallApiFunction("vkGetSemaphoreFdKHR"); return VK_SUCCESS; };
        virtual void PreCallCmdPushDescriptorSetKHR(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites) { PreCallApiFunction("vkCmdPushDescriptorSetKHR");};
        virtual void PostCallCmdPushDescriptorSetKHR(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t set, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites) { PostCallApiFunction("vkCmdPushDescriptorSetKHR");};
        virtual void PreCallCmdPushDescriptorSetWithTemplateKHR(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData) { PreCallApiFunction("vkCmdPushDescriptorSetWithTemplateKHR");};
        virtual void PostCallCmdPushDescriptorSetWithTemplateKHR(VkCommandBuffer commandBuffer, VkDescriptorUpdateTemplate descriptorUpdateTemplate, VkPipelineLayout layout, uint32_t set, const void* pData) { PostCallApiFunction("vkCmdPushDescriptorSetWithTemplateKHR");};
        virtual VkResult PreCallCreateDescriptorUpdateTemplateKHR(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) { PreCallApiFunction("vkCreateDescriptorUpdateTemplateKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDescriptorUpdateTemplateKHR(VkDevice device, const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorUpdateTemplate* pDescriptorUpdateTemplate) { PostCallApiFunction("vkCreateDescriptorUpdateTemplateKHR"); return VK_SUCCESS; };
        virtual void PreCallDestroyDescriptorUpdateTemplateKHR(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDescriptorUpdateTemplateKHR");};
        virtual void PostCallDestroyDescriptorUpdateTemplateKHR(VkDevice device, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDescriptorUpdateTemplateKHR");};
        virtual void PreCallUpdateDescriptorSetWithTemplateKHR(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData) { PreCallApiFunction("vkUpdateDescriptorSetWithTemplateKHR");};
        virtual void PostCallUpdateDescriptorSetWithTemplateKHR(VkDevice device, VkDescriptorSet descriptorSet, VkDescriptorUpdateTemplate descriptorUpdateTemplate, const void* pData) { PostCallApiFunction("vkUpdateDescriptorSetWithTemplateKHR");};
        virtual VkResult PreCallGetSwapchainStatusKHR(VkDevice device, VkSwapchainKHR swapchain) { PreCallApiFunction("vkGetSwapchainStatusKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetSwapchainStatusKHR(VkDevice device, VkSwapchainKHR swapchain) { PostCallApiFunction("vkGetSwapchainStatusKHR"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceExternalFencePropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalFencePropertiesKHR");};
        virtual void PostCallGetPhysicalDeviceExternalFencePropertiesKHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceExternalFenceInfo* pExternalFenceInfo, VkExternalFenceProperties* pExternalFenceProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalFencePropertiesKHR");};
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallImportFenceWin32HandleKHR(VkDevice device, const VkImportFenceWin32HandleInfoKHR* pImportFenceWin32HandleInfo) { PreCallApiFunction("vkImportFenceWin32HandleKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallImportFenceWin32HandleKHR(VkDevice device, const VkImportFenceWin32HandleInfoKHR* pImportFenceWin32HandleInfo) { PostCallApiFunction("vkImportFenceWin32HandleKHR"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallGetFenceWin32HandleKHR(VkDevice device, const VkFenceGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) { PreCallApiFunction("vkGetFenceWin32HandleKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetFenceWin32HandleKHR(VkDevice device, const VkFenceGetWin32HandleInfoKHR* pGetWin32HandleInfo, HANDLE* pHandle) { PostCallApiFunction("vkGetFenceWin32HandleKHR"); return VK_SUCCESS; };
#endif
        virtual VkResult PreCallImportFenceFdKHR(VkDevice device, const VkImportFenceFdInfoKHR* pImportFenceFdInfo) { PreCallApiFunction("vkImportFenceFdKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallImportFenceFdKHR(VkDevice device, const VkImportFenceFdInfoKHR* pImportFenceFdInfo) { PostCallApiFunction("vkImportFenceFdKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetFenceFdKHR(VkDevice device, const VkFenceGetFdInfoKHR* pGetFdInfo, int* pFd) { PreCallApiFunction("vkGetFenceFdKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetFenceFdKHR(VkDevice device, const VkFenceGetFdInfoKHR* pGetFdInfo, int* pFd) { PostCallApiFunction("vkGetFenceFdKHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, VkSurfaceCapabilities2KHR* pSurfaceCapabilities) { PreCallApiFunction("vkGetPhysicalDeviceSurfaceCapabilities2KHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfaceCapabilities2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, VkSurfaceCapabilities2KHR* pSurfaceCapabilities) { PostCallApiFunction("vkGetPhysicalDeviceSurfaceCapabilities2KHR"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats) { PreCallApiFunction("vkGetPhysicalDeviceSurfaceFormats2KHR"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfaceFormats2KHR(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, uint32_t* pSurfaceFormatCount, VkSurfaceFormat2KHR* pSurfaceFormats) { PostCallApiFunction("vkGetPhysicalDeviceSurfaceFormats2KHR"); return VK_SUCCESS; };
        virtual void PreCallGetImageMemoryRequirements2KHR(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PreCallApiFunction("vkGetImageMemoryRequirements2KHR");};
        virtual void PostCallGetImageMemoryRequirements2KHR(VkDevice device, const VkImageMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PostCallApiFunction("vkGetImageMemoryRequirements2KHR");};
        virtual void PreCallGetBufferMemoryRequirements2KHR(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PreCallApiFunction("vkGetBufferMemoryRequirements2KHR");};
        virtual void PostCallGetBufferMemoryRequirements2KHR(VkDevice device, const VkBufferMemoryRequirementsInfo2* pInfo, VkMemoryRequirements2* pMemoryRequirements) { PostCallApiFunction("vkGetBufferMemoryRequirements2KHR");};
        virtual void PreCallGetImageSparseMemoryRequirements2KHR(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) { PreCallApiFunction("vkGetImageSparseMemoryRequirements2KHR");};
        virtual void PostCallGetImageSparseMemoryRequirements2KHR(VkDevice device, const VkImageSparseMemoryRequirementsInfo2* pInfo, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements2* pSparseMemoryRequirements) { PostCallApiFunction("vkGetImageSparseMemoryRequirements2KHR");};
        virtual VkResult PreCallCreateSamplerYcbcrConversionKHR(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion) { PreCallApiFunction("vkCreateSamplerYcbcrConversionKHR"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateSamplerYcbcrConversionKHR(VkDevice device, const VkSamplerYcbcrConversionCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSamplerYcbcrConversion* pYcbcrConversion) { PostCallApiFunction("vkCreateSamplerYcbcrConversionKHR"); return VK_SUCCESS; };
        virtual void PreCallDestroySamplerYcbcrConversionKHR(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroySamplerYcbcrConversionKHR");};
        virtual void PostCallDestroySamplerYcbcrConversionKHR(VkDevice device, VkSamplerYcbcrConversion ycbcrConversion, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroySamplerYcbcrConversionKHR");};
        virtual VkResult PreCallBindBufferMemory2KHR(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos) { PreCallApiFunction("vkBindBufferMemory2KHR"); return VK_SUCCESS; };
        virtual VkResult PostCallBindBufferMemory2KHR(VkDevice device, uint32_t bindInfoCount, const VkBindBufferMemoryInfo* pBindInfos) { PostCallApiFunction("vkBindBufferMemory2KHR"); return VK_SUCCESS; };
        virtual VkResult PreCallBindImageMemory2KHR(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos) { PreCallApiFunction("vkBindImageMemory2KHR"); return VK_SUCCESS; };
        virtual VkResult PostCallBindImageMemory2KHR(VkDevice device, uint32_t bindInfoCount, const VkBindImageMemoryInfo* pBindInfos) { PostCallApiFunction("vkBindImageMemory2KHR"); return VK_SUCCESS; };
        virtual void PreCallGetDescriptorSetLayoutSupportKHR(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport) { PreCallApiFunction("vkGetDescriptorSetLayoutSupportKHR");};
        virtual void PostCallGetDescriptorSetLayoutSupportKHR(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, VkDescriptorSetLayoutSupport* pSupport) { PostCallApiFunction("vkGetDescriptorSetLayoutSupportKHR");};
        virtual VkResult PreCallCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) { PreCallApiFunction("vkCreateDebugReportCallbackEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) { PostCallApiFunction("vkCreateDebugReportCallbackEXT"); return VK_SUCCESS; };
        virtual void PreCallDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDebugReportCallbackEXT");};
        virtual void PostCallDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDebugReportCallbackEXT");};
        virtual void PreCallDebugReportMessageEXT(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage) { PreCallApiFunction("vkDebugReportMessageEXT");};
        virtual void PostCallDebugReportMessageEXT(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage) { PostCallApiFunction("vkDebugReportMessageEXT");};
        virtual VkResult PreCallDebugMarkerSetObjectTagEXT(VkDevice device, const VkDebugMarkerObjectTagInfoEXT* pTagInfo) { PreCallApiFunction("vkDebugMarkerSetObjectTagEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallDebugMarkerSetObjectTagEXT(VkDevice device, const VkDebugMarkerObjectTagInfoEXT* pTagInfo) { PostCallApiFunction("vkDebugMarkerSetObjectTagEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallDebugMarkerSetObjectNameEXT(VkDevice device, const VkDebugMarkerObjectNameInfoEXT* pNameInfo) { PreCallApiFunction("vkDebugMarkerSetObjectNameEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallDebugMarkerSetObjectNameEXT(VkDevice device, const VkDebugMarkerObjectNameInfoEXT* pNameInfo) { PostCallApiFunction("vkDebugMarkerSetObjectNameEXT"); return VK_SUCCESS; };
        virtual void PreCallCmdDebugMarkerBeginEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) { PreCallApiFunction("vkCmdDebugMarkerBeginEXT");};
        virtual void PostCallCmdDebugMarkerBeginEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) { PostCallApiFunction("vkCmdDebugMarkerBeginEXT");};
        virtual void PreCallCmdDebugMarkerEndEXT(VkCommandBuffer commandBuffer) { PreCallApiFunction("vkCmdDebugMarkerEndEXT");};
        virtual void PostCallCmdDebugMarkerEndEXT(VkCommandBuffer commandBuffer) { PostCallApiFunction("vkCmdDebugMarkerEndEXT");};
        virtual void PreCallCmdDebugMarkerInsertEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) { PreCallApiFunction("vkCmdDebugMarkerInsertEXT");};
        virtual void PostCallCmdDebugMarkerInsertEXT(VkCommandBuffer commandBuffer, const VkDebugMarkerMarkerInfoEXT* pMarkerInfo) { PostCallApiFunction("vkCmdDebugMarkerInsertEXT");};
        virtual void PreCallCmdDrawIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride) { PreCallApiFunction("vkCmdDrawIndirectCountAMD");};
        virtual void PostCallCmdDrawIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride) { PostCallApiFunction("vkCmdDrawIndirectCountAMD");};
        virtual void PreCallCmdDrawIndexedIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride) { PreCallApiFunction("vkCmdDrawIndexedIndirectCountAMD");};
        virtual void PostCallCmdDrawIndexedIndirectCountAMD(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer, VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride) { PostCallApiFunction("vkCmdDrawIndexedIndirectCountAMD");};
        virtual VkResult PreCallGetShaderInfoAMD(VkDevice device, VkPipeline pipeline, VkShaderStageFlagBits shaderStage, VkShaderInfoTypeAMD infoType, size_t* pInfoSize, void* pInfo) { PreCallApiFunction("vkGetShaderInfoAMD"); return VK_SUCCESS; };
        virtual VkResult PostCallGetShaderInfoAMD(VkDevice device, VkPipeline pipeline, VkShaderStageFlagBits shaderStage, VkShaderInfoTypeAMD infoType, size_t* pInfoSize, void* pInfo) { PostCallApiFunction("vkGetShaderInfoAMD"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPhysicalDeviceExternalImageFormatPropertiesNV(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkExternalMemoryHandleTypeFlagsNV externalHandleType, VkExternalImageFormatPropertiesNV* pExternalImageFormatProperties) { PreCallApiFunction("vkGetPhysicalDeviceExternalImageFormatPropertiesNV"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceExternalImageFormatPropertiesNV(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkExternalMemoryHandleTypeFlagsNV externalHandleType, VkExternalImageFormatPropertiesNV* pExternalImageFormatProperties) { PostCallApiFunction("vkGetPhysicalDeviceExternalImageFormatPropertiesNV"); return VK_SUCCESS; };
#ifdef VK_USE_PLATFORM_WIN32_KHR
        virtual VkResult PreCallGetMemoryWin32HandleNV(VkDevice device, VkDeviceMemory memory, VkExternalMemoryHandleTypeFlagsNV handleType, HANDLE* pHandle) { PreCallApiFunction("vkGetMemoryWin32HandleNV"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryWin32HandleNV(VkDevice device, VkDeviceMemory memory, VkExternalMemoryHandleTypeFlagsNV handleType, HANDLE* pHandle) { PostCallApiFunction("vkGetMemoryWin32HandleNV"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_VI_NN
        virtual VkResult PreCallCreateViSurfaceNN(VkInstance instance, const VkViSurfaceCreateInfoNN* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateViSurfaceNN"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateViSurfaceNN(VkInstance instance, const VkViSurfaceCreateInfoNN* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateViSurfaceNN"); return VK_SUCCESS; };
#endif
        virtual void PreCallCmdProcessCommandsNVX(VkCommandBuffer commandBuffer, const VkCmdProcessCommandsInfoNVX* pProcessCommandsInfo) { PreCallApiFunction("vkCmdProcessCommandsNVX");};
        virtual void PostCallCmdProcessCommandsNVX(VkCommandBuffer commandBuffer, const VkCmdProcessCommandsInfoNVX* pProcessCommandsInfo) { PostCallApiFunction("vkCmdProcessCommandsNVX");};
        virtual void PreCallCmdReserveSpaceForCommandsNVX(VkCommandBuffer commandBuffer, const VkCmdReserveSpaceForCommandsInfoNVX* pReserveSpaceInfo) { PreCallApiFunction("vkCmdReserveSpaceForCommandsNVX");};
        virtual void PostCallCmdReserveSpaceForCommandsNVX(VkCommandBuffer commandBuffer, const VkCmdReserveSpaceForCommandsInfoNVX* pReserveSpaceInfo) { PostCallApiFunction("vkCmdReserveSpaceForCommandsNVX");};
        virtual VkResult PreCallCreateIndirectCommandsLayoutNVX(VkDevice device, const VkIndirectCommandsLayoutCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkIndirectCommandsLayoutNVX* pIndirectCommandsLayout) { PreCallApiFunction("vkCreateIndirectCommandsLayoutNVX"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateIndirectCommandsLayoutNVX(VkDevice device, const VkIndirectCommandsLayoutCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkIndirectCommandsLayoutNVX* pIndirectCommandsLayout) { PostCallApiFunction("vkCreateIndirectCommandsLayoutNVX"); return VK_SUCCESS; };
        virtual void PreCallDestroyIndirectCommandsLayoutNVX(VkDevice device, VkIndirectCommandsLayoutNVX indirectCommandsLayout, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyIndirectCommandsLayoutNVX");};
        virtual void PostCallDestroyIndirectCommandsLayoutNVX(VkDevice device, VkIndirectCommandsLayoutNVX indirectCommandsLayout, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyIndirectCommandsLayoutNVX");};
        virtual VkResult PreCallCreateObjectTableNVX(VkDevice device, const VkObjectTableCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkObjectTableNVX* pObjectTable) { PreCallApiFunction("vkCreateObjectTableNVX"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateObjectTableNVX(VkDevice device, const VkObjectTableCreateInfoNVX* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkObjectTableNVX* pObjectTable) { PostCallApiFunction("vkCreateObjectTableNVX"); return VK_SUCCESS; };
        virtual void PreCallDestroyObjectTableNVX(VkDevice device, VkObjectTableNVX objectTable, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyObjectTableNVX");};
        virtual void PostCallDestroyObjectTableNVX(VkDevice device, VkObjectTableNVX objectTable, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyObjectTableNVX");};
        virtual VkResult PreCallRegisterObjectsNVX(VkDevice device, VkObjectTableNVX objectTable, uint32_t objectCount, const VkObjectTableEntryNVX* const*    ppObjectTableEntries, const uint32_t* pObjectIndices) { PreCallApiFunction("vkRegisterObjectsNVX"); return VK_SUCCESS; };
        virtual VkResult PostCallRegisterObjectsNVX(VkDevice device, VkObjectTableNVX objectTable, uint32_t objectCount, const VkObjectTableEntryNVX* const*    ppObjectTableEntries, const uint32_t* pObjectIndices) { PostCallApiFunction("vkRegisterObjectsNVX"); return VK_SUCCESS; };
        virtual VkResult PreCallUnregisterObjectsNVX(VkDevice device, VkObjectTableNVX objectTable, uint32_t objectCount, const VkObjectEntryTypeNVX* pObjectEntryTypes, const uint32_t* pObjectIndices) { PreCallApiFunction("vkUnregisterObjectsNVX"); return VK_SUCCESS; };
        virtual VkResult PostCallUnregisterObjectsNVX(VkDevice device, VkObjectTableNVX objectTable, uint32_t objectCount, const VkObjectEntryTypeNVX* pObjectEntryTypes, const uint32_t* pObjectIndices) { PostCallApiFunction("vkUnregisterObjectsNVX"); return VK_SUCCESS; };
        virtual void PreCallGetPhysicalDeviceGeneratedCommandsPropertiesNVX(VkPhysicalDevice physicalDevice, VkDeviceGeneratedCommandsFeaturesNVX* pFeatures, VkDeviceGeneratedCommandsLimitsNVX* pLimits) { PreCallApiFunction("vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX");};
        virtual void PostCallGetPhysicalDeviceGeneratedCommandsPropertiesNVX(VkPhysicalDevice physicalDevice, VkDeviceGeneratedCommandsFeaturesNVX* pFeatures, VkDeviceGeneratedCommandsLimitsNVX* pLimits) { PostCallApiFunction("vkGetPhysicalDeviceGeneratedCommandsPropertiesNVX");};
        virtual void PreCallCmdSetViewportWScalingNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportWScalingNV* pViewportWScalings) { PreCallApiFunction("vkCmdSetViewportWScalingNV");};
        virtual void PostCallCmdSetViewportWScalingNV(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewportWScalingNV* pViewportWScalings) { PostCallApiFunction("vkCmdSetViewportWScalingNV");};
        virtual VkResult PreCallReleaseDisplayEXT(VkPhysicalDevice physicalDevice, VkDisplayKHR display) { PreCallApiFunction("vkReleaseDisplayEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallReleaseDisplayEXT(VkPhysicalDevice physicalDevice, VkDisplayKHR display) { PostCallApiFunction("vkReleaseDisplayEXT"); return VK_SUCCESS; };
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
        virtual VkResult PreCallAcquireXlibDisplayEXT(VkPhysicalDevice physicalDevice, Display* dpy, VkDisplayKHR display) { PreCallApiFunction("vkAcquireXlibDisplayEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallAcquireXlibDisplayEXT(VkPhysicalDevice physicalDevice, Display* dpy, VkDisplayKHR display) { PostCallApiFunction("vkAcquireXlibDisplayEXT"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT
        virtual VkResult PreCallGetRandROutputDisplayEXT(VkPhysicalDevice physicalDevice, Display* dpy, RROutput rrOutput, VkDisplayKHR* pDisplay) { PreCallApiFunction("vkGetRandROutputDisplayEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallGetRandROutputDisplayEXT(VkPhysicalDevice physicalDevice, Display* dpy, RROutput rrOutput, VkDisplayKHR* pDisplay) { PostCallApiFunction("vkGetRandROutputDisplayEXT"); return VK_SUCCESS; };
#endif
        virtual VkResult PreCallGetPhysicalDeviceSurfaceCapabilities2EXT(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilities2EXT* pSurfaceCapabilities) { PreCallApiFunction("vkGetPhysicalDeviceSurfaceCapabilities2EXT"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPhysicalDeviceSurfaceCapabilities2EXT(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilities2EXT* pSurfaceCapabilities) { PostCallApiFunction("vkGetPhysicalDeviceSurfaceCapabilities2EXT"); return VK_SUCCESS; };
        virtual VkResult PreCallDisplayPowerControlEXT(VkDevice device, VkDisplayKHR display, const VkDisplayPowerInfoEXT* pDisplayPowerInfo) { PreCallApiFunction("vkDisplayPowerControlEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallDisplayPowerControlEXT(VkDevice device, VkDisplayKHR display, const VkDisplayPowerInfoEXT* pDisplayPowerInfo) { PostCallApiFunction("vkDisplayPowerControlEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallRegisterDeviceEventEXT(VkDevice device, const VkDeviceEventInfoEXT* pDeviceEventInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) { PreCallApiFunction("vkRegisterDeviceEventEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallRegisterDeviceEventEXT(VkDevice device, const VkDeviceEventInfoEXT* pDeviceEventInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) { PostCallApiFunction("vkRegisterDeviceEventEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallRegisterDisplayEventEXT(VkDevice device, VkDisplayKHR display, const VkDisplayEventInfoEXT* pDisplayEventInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) { PreCallApiFunction("vkRegisterDisplayEventEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallRegisterDisplayEventEXT(VkDevice device, VkDisplayKHR display, const VkDisplayEventInfoEXT* pDisplayEventInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) { PostCallApiFunction("vkRegisterDisplayEventEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallGetSwapchainCounterEXT(VkDevice device, VkSwapchainKHR swapchain, VkSurfaceCounterFlagBitsEXT counter, uint64_t* pCounterValue) { PreCallApiFunction("vkGetSwapchainCounterEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallGetSwapchainCounterEXT(VkDevice device, VkSwapchainKHR swapchain, VkSurfaceCounterFlagBitsEXT counter, uint64_t* pCounterValue) { PostCallApiFunction("vkGetSwapchainCounterEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallGetRefreshCycleDurationGOOGLE(VkDevice device, VkSwapchainKHR swapchain, VkRefreshCycleDurationGOOGLE* pDisplayTimingProperties) { PreCallApiFunction("vkGetRefreshCycleDurationGOOGLE"); return VK_SUCCESS; };
        virtual VkResult PostCallGetRefreshCycleDurationGOOGLE(VkDevice device, VkSwapchainKHR swapchain, VkRefreshCycleDurationGOOGLE* pDisplayTimingProperties) { PostCallApiFunction("vkGetRefreshCycleDurationGOOGLE"); return VK_SUCCESS; };
        virtual VkResult PreCallGetPastPresentationTimingGOOGLE(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pPresentationTimingCount, VkPastPresentationTimingGOOGLE* pPresentationTimings) { PreCallApiFunction("vkGetPastPresentationTimingGOOGLE"); return VK_SUCCESS; };
        virtual VkResult PostCallGetPastPresentationTimingGOOGLE(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pPresentationTimingCount, VkPastPresentationTimingGOOGLE* pPresentationTimings) { PostCallApiFunction("vkGetPastPresentationTimingGOOGLE"); return VK_SUCCESS; };
        virtual void PreCallCmdSetDiscardRectangleEXT(VkCommandBuffer commandBuffer, uint32_t firstDiscardRectangle, uint32_t discardRectangleCount, const VkRect2D* pDiscardRectangles) { PreCallApiFunction("vkCmdSetDiscardRectangleEXT");};
        virtual void PostCallCmdSetDiscardRectangleEXT(VkCommandBuffer commandBuffer, uint32_t firstDiscardRectangle, uint32_t discardRectangleCount, const VkRect2D* pDiscardRectangles) { PostCallApiFunction("vkCmdSetDiscardRectangleEXT");};
        virtual void PreCallSetHdrMetadataEXT(VkDevice device, uint32_t swapchainCount, const VkSwapchainKHR* pSwapchains, const VkHdrMetadataEXT* pMetadata) { PreCallApiFunction("vkSetHdrMetadataEXT");};
        virtual void PostCallSetHdrMetadataEXT(VkDevice device, uint32_t swapchainCount, const VkSwapchainKHR* pSwapchains, const VkHdrMetadataEXT* pMetadata) { PostCallApiFunction("vkSetHdrMetadataEXT");};
#ifdef VK_USE_PLATFORM_IOS_MVK
        virtual VkResult PreCallCreateIOSSurfaceMVK(VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateIOSSurfaceMVK"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateIOSSurfaceMVK(VkInstance instance, const VkIOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateIOSSurfaceMVK"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
        virtual VkResult PreCallCreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PreCallApiFunction("vkCreateMacOSSurfaceMVK"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateMacOSSurfaceMVK(VkInstance instance, const VkMacOSSurfaceCreateInfoMVK* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) { PostCallApiFunction("vkCreateMacOSSurfaceMVK"); return VK_SUCCESS; };
#endif
        virtual VkResult PreCallSetDebugUtilsObjectNameEXT(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo) { PreCallApiFunction("vkSetDebugUtilsObjectNameEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallSetDebugUtilsObjectNameEXT(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo) { PostCallApiFunction("vkSetDebugUtilsObjectNameEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallSetDebugUtilsObjectTagEXT(VkDevice device, const VkDebugUtilsObjectTagInfoEXT* pTagInfo) { PreCallApiFunction("vkSetDebugUtilsObjectTagEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallSetDebugUtilsObjectTagEXT(VkDevice device, const VkDebugUtilsObjectTagInfoEXT* pTagInfo) { PostCallApiFunction("vkSetDebugUtilsObjectTagEXT"); return VK_SUCCESS; };
        virtual void PreCallQueueBeginDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo) { PreCallApiFunction("vkQueueBeginDebugUtilsLabelEXT");};
        virtual void PostCallQueueBeginDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo) { PostCallApiFunction("vkQueueBeginDebugUtilsLabelEXT");};
        virtual void PreCallQueueEndDebugUtilsLabelEXT(VkQueue queue) { PreCallApiFunction("vkQueueEndDebugUtilsLabelEXT");};
        virtual void PostCallQueueEndDebugUtilsLabelEXT(VkQueue queue) { PostCallApiFunction("vkQueueEndDebugUtilsLabelEXT");};
        virtual void PreCallQueueInsertDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo) { PreCallApiFunction("vkQueueInsertDebugUtilsLabelEXT");};
        virtual void PostCallQueueInsertDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo) { PostCallApiFunction("vkQueueInsertDebugUtilsLabelEXT");};
        virtual void PreCallCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) { PreCallApiFunction("vkCmdBeginDebugUtilsLabelEXT");};
        virtual void PostCallCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) { PostCallApiFunction("vkCmdBeginDebugUtilsLabelEXT");};
        virtual void PreCallCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer) { PreCallApiFunction("vkCmdEndDebugUtilsLabelEXT");};
        virtual void PostCallCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer) { PostCallApiFunction("vkCmdEndDebugUtilsLabelEXT");};
        virtual void PreCallCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) { PreCallApiFunction("vkCmdInsertDebugUtilsLabelEXT");};
        virtual void PostCallCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo) { PostCallApiFunction("vkCmdInsertDebugUtilsLabelEXT");};
        virtual VkResult PreCallCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) { PreCallApiFunction("vkCreateDebugUtilsMessengerEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) { PostCallApiFunction("vkCreateDebugUtilsMessengerEXT"); return VK_SUCCESS; };
        virtual void PreCallDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyDebugUtilsMessengerEXT");};
        virtual void PostCallDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyDebugUtilsMessengerEXT");};
        virtual void PreCallSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) { PreCallApiFunction("vkSubmitDebugUtilsMessageEXT");};
        virtual void PostCallSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) { PostCallApiFunction("vkSubmitDebugUtilsMessageEXT");};
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        virtual VkResult PreCallGetAndroidHardwareBufferPropertiesANDROID(VkDevice device, const struct AHardwareBuffer* buffer, VkAndroidHardwareBufferPropertiesANDROID* pProperties) { PreCallApiFunction("vkGetAndroidHardwareBufferPropertiesANDROID"); return VK_SUCCESS; };
        virtual VkResult PostCallGetAndroidHardwareBufferPropertiesANDROID(VkDevice device, const struct AHardwareBuffer* buffer, VkAndroidHardwareBufferPropertiesANDROID* pProperties) { PostCallApiFunction("vkGetAndroidHardwareBufferPropertiesANDROID"); return VK_SUCCESS; };
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
        virtual VkResult PreCallGetMemoryAndroidHardwareBufferANDROID(VkDevice device, const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo, struct AHardwareBuffer** pBuffer) { PreCallApiFunction("vkGetMemoryAndroidHardwareBufferANDROID"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryAndroidHardwareBufferANDROID(VkDevice device, const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo, struct AHardwareBuffer** pBuffer) { PostCallApiFunction("vkGetMemoryAndroidHardwareBufferANDROID"); return VK_SUCCESS; };
#endif
        virtual void PreCallCmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT* pSampleLocationsInfo) { PreCallApiFunction("vkCmdSetSampleLocationsEXT");};
        virtual void PostCallCmdSetSampleLocationsEXT(VkCommandBuffer commandBuffer, const VkSampleLocationsInfoEXT* pSampleLocationsInfo) { PostCallApiFunction("vkCmdSetSampleLocationsEXT");};
        virtual void PreCallGetPhysicalDeviceMultisamplePropertiesEXT(VkPhysicalDevice physicalDevice, VkSampleCountFlagBits samples, VkMultisamplePropertiesEXT* pMultisampleProperties) { PreCallApiFunction("vkGetPhysicalDeviceMultisamplePropertiesEXT");};
        virtual void PostCallGetPhysicalDeviceMultisamplePropertiesEXT(VkPhysicalDevice physicalDevice, VkSampleCountFlagBits samples, VkMultisamplePropertiesEXT* pMultisampleProperties) { PostCallApiFunction("vkGetPhysicalDeviceMultisamplePropertiesEXT");};
        virtual VkResult PreCallCreateValidationCacheEXT(VkDevice device, const VkValidationCacheCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkValidationCacheEXT* pValidationCache) { PreCallApiFunction("vkCreateValidationCacheEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallCreateValidationCacheEXT(VkDevice device, const VkValidationCacheCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkValidationCacheEXT* pValidationCache) { PostCallApiFunction("vkCreateValidationCacheEXT"); return VK_SUCCESS; };
        virtual void PreCallDestroyValidationCacheEXT(VkDevice device, VkValidationCacheEXT validationCache, const VkAllocationCallbacks* pAllocator) { PreCallApiFunction("vkDestroyValidationCacheEXT");};
        virtual void PostCallDestroyValidationCacheEXT(VkDevice device, VkValidationCacheEXT validationCache, const VkAllocationCallbacks* pAllocator) { PostCallApiFunction("vkDestroyValidationCacheEXT");};
        virtual VkResult PreCallMergeValidationCachesEXT(VkDevice device, VkValidationCacheEXT dstCache, uint32_t srcCacheCount, const VkValidationCacheEXT* pSrcCaches) { PreCallApiFunction("vkMergeValidationCachesEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallMergeValidationCachesEXT(VkDevice device, VkValidationCacheEXT dstCache, uint32_t srcCacheCount, const VkValidationCacheEXT* pSrcCaches) { PostCallApiFunction("vkMergeValidationCachesEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallGetValidationCacheDataEXT(VkDevice device, VkValidationCacheEXT validationCache, size_t* pDataSize, void* pData) { PreCallApiFunction("vkGetValidationCacheDataEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallGetValidationCacheDataEXT(VkDevice device, VkValidationCacheEXT validationCache, size_t* pDataSize, void* pData) { PostCallApiFunction("vkGetValidationCacheDataEXT"); return VK_SUCCESS; };
        virtual VkResult PreCallGetMemoryHostPointerPropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void* pHostPointer, VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties) { PreCallApiFunction("vkGetMemoryHostPointerPropertiesEXT"); return VK_SUCCESS; };
        virtual VkResult PostCallGetMemoryHostPointerPropertiesEXT(VkDevice device, VkExternalMemoryHandleTypeFlagBits handleType, const void* pHostPointer, VkMemoryHostPointerPropertiesEXT* pMemoryHostPointerProperties) { PostCallApiFunction("vkGetMemoryHostPointerPropertiesEXT"); return VK_SUCCESS; };
        virtual void PreCallCmdWriteBufferMarkerAMD(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker) { PreCallApiFunction("vkCmdWriteBufferMarkerAMD");};
        virtual void PostCallCmdWriteBufferMarkerAMD(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkBuffer dstBuffer, VkDeviceSize dstOffset, uint32_t marker) { PostCallApiFunction("vkCmdWriteBufferMarkerAMD");};
};


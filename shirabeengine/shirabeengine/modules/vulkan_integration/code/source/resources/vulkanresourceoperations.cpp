//
// Created by dotti on 09.11.19.
//
#include "vulkan_integration/resources/vulkanresourceoperations.h"

namespace engine::vulkan
{
    EEngineStatus createTexture(Shared<IVkGlobalContext> &aVulkanContext, STexture const &aTexture)
    {
        Shared<CVulkanTextureResource> resource = makeShared<CVulkanTextureResource>();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus createTextureView(Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus createBuffer(Shared<IVkGlobalContext> &aVulkanContext, SBuffer const &aBuffer)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus createBufferView       (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    EEngineStatus createRenderPass       (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    EEngineStatus createFrameBuffer      (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    EEngineStatus createGraphicsPipeline (Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
    EEngineStatus loadTexture            (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    EEngineStatus loadBuffer             (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    EEngineStatus bindTexture            (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    EEngineStatus bindTextureView        (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    EEngineStatus bindBuffer             (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    EEngineStatus bindBufferView         (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    EEngineStatus bindRenderPass         (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    EEngineStatus bindFrameBuffer        (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    EEngineStatus bindGraphicsPipeline   (Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
    EEngineStatus transferTexture        (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    EEngineStatus transferBuffer         (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    EEngineStatus unbindTexture          (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    EEngineStatus unbindTextureView      (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    EEngineStatus unbindBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    EEngineStatus unbindBufferView       (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    EEngineStatus unbindRenderPass       (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    EEngineStatus unbindFrameBuffer      (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    EEngineStatus unbindGraphicsPipeline (Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
    EEngineStatus unloadTexture          (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    EEngineStatus unloadBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    EEngineStatus destroyTexture         (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    EEngineStatus destroyTextureView     (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    EEngineStatus destroyBuffer          (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    EEngineStatus destroyBufferView      (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    EEngineStatus destroyRenderPass      (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    EEngineStatus destroyFrameBuffer     (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    EEngineStatus destroyGraphicsPipeline(Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
}

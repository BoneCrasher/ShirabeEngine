//
// Created by dotti on 06.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_VULKANRESOURCEOPERATIONS_H__
#define __SHIRABEDEVELOPMENT_VULKANRESOURCEOPERATIONS_H__

#include <base/cpp_tools.h>

#include "rhi/resource_management/resourcetypes.h"

#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_resources/resources/types/vulkanbufferresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanbufferviewresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanimageresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanimageviewresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanrenderpassresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanframebufferresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanpipelineresource.h"
#include "rhi/vulkan_resources/resources/types/vulkanshadermoduleresource.h"

#include "rhi/resource_management/cresourcemanager.h"

namespace engine::rhi
{
    using namespace vulkan;

    // TODO: Mesh, Material

    //static EEngineStatus createImage          (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus createTextureView      (Shared<IVkGlobalContext> &aVulkanContext, SRHIImageView const &aTextureView, SRHIImage const &aTexture);
    //static EEngineStatus createBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus createBufferView       (Shared<IVkGlobalContext> &aVulkanContext, SRHIBufferView  const &aBufferView, SRHIBuffer const &aBuffer);
    //static EEngineStatus createRenderPass       (Shared<IVkGlobalContext> &aVulkanContext, SRHIRenderPass  const &aRenderPass);
    //static EEngineStatus createFrameBuffer      (Shared<IVkGlobalContext> &aVulkanContext, SRHIFrameBuffer const &aFrameBuffer, SRHIRenderPass const &aRenderPass);
    //static EEngineStatus createGraphicsPipeline (Shared<IVkGlobalContext> &aVulkanContext, SRHIPipeline    const &aPipeline);
    //static EEngineStatus loadTexture            (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus loadBuffer             (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus bindTexture            (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus bindTextureView        (Shared<IVkGlobalContext> &aVulkanContext, SRHIImageView const &aTextureView, SRHIImage const &aTexture);
    //static EEngineStatus bindBuffer             (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus bindBufferView         (Shared<IVkGlobalContext> &aVulkanContext, SRHIBufferView  const &aBufferView, SRHIBuffer const &aBuffer);
    //static EEngineStatus bindRenderPass         (Shared<IVkGlobalContext> &aVulkanContext, SRHIRenderPass  const &aRenderPass);
    //static EEngineStatus bindFrameBuffer        (Shared<IVkGlobalContext> &aVulkanContext, SRHIFrameBuffer const &aFrameBuffer, SRHIRenderPass const &aRenderPass);
    //static EEngineStatus bindGraphicsPipeline   (Shared<IVkGlobalContext> &aVulkanContext, SRHIPipeline    const &aPipeline);
    //static EEngineStatus transferTexture        (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus transferBuffer         (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus unbindTexture          (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus unbindTextureView      (Shared<IVkGlobalContext> &aVulkanContext, SRHIImageView const &aTextureView, SRHIImage const &aTexture);
    //static EEngineStatus unbindBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus unbindBufferView       (Shared<IVkGlobalContext> &aVulkanContext, SRHIBufferView  const &aBufferView, SRHIBuffer const &aBuffer);
    //static EEngineStatus unbindRenderPass       (Shared<IVkGlobalContext> &aVulkanContext, SRHIRenderPass  const &aRenderPass);
    //static EEngineStatus unbindFrameBuffer      (Shared<IVkGlobalContext> &aVulkanContext, SRHIFrameBuffer const &aFrameBuffer, SRHIRenderPass const &aRenderPass);
    //static EEngineStatus unbindGraphicsPipeline (Shared<IVkGlobalContext> &aVulkanContext, SRHIPipeline    const &aPipeline);
    //static EEngineStatus unloadTexture          (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus unloadBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus destroyTexture         (Shared<IVkGlobalContext> &aVulkanContext, SRHIImage     const &aTexture);
    //static EEngineStatus destroyTextureView     (Shared<IVkGlobalContext> &aVulkanContext, SRHIImageView const &aTextureView, SRHIImage const &aTexture);
    //static EEngineStatus destroyBuffer          (Shared<IVkGlobalContext> &aVulkanContext, SRHIBuffer      const &aBuffer);
    //static EEngineStatus destroyBufferView      (Shared<IVkGlobalContext> &aVulkanContext, SRHIBufferView  const &aBufferView, SRHIBuffer const &aBuffer);
    //static EEngineStatus destroyRenderPass      (Shared<IVkGlobalContext> &aVulkanContext, SRHIRenderPass  const &aRenderPass);
    //static EEngineStatus destroyFrameBuffer     (Shared<IVkGlobalContext> &aVulkanContext, SRHIFrameBuffer const &aFrameBuffer, SRHIRenderPass const &aRenderPass);
    //static EEngineStatus destroyGraphicsPipeline(Shared<IVkGlobalContext> &aVulkanContext, SRHIPipeline    const &aPipeline);
//
    //#define CaseIfResourceTypesMatches(...) \
    //    if constexpr(std::is_same_v<variadic_tuple_t<TRHIResource, TLogicalResourceDependencies...>, variadic_tuple_t<__VA_ARGS__>>)
//
    //#define ForwardResourceTypesInstances(aResourceIdentifier, aDependencyIdentifier) \
    //    std::forward<TRHIResource>(aResourceIdentifier), std::forward<TLogicalResourceDependencies>(aDependencyIdentifier)...
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceCreator
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        { return createImage(ForwardResourceTypesInstances(aResource, aDependencies)); }
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        { return createTextureView(ForwardResourceTypesInstances(aResource, aDependencies)); }
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {}
    //    }
    //};
    ////<-----------------------------------------------------------------------------
//
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceLoader
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {}
    //    }
    //};
    ////<-----------------------------------------------------------------------------
//
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceBinder
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {}
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {
    //            SVulkanState &state = mVulkanEnvironment->getState();
//
    //            VkClearValue clearColor = { 0.0f, 0.5f, 0.5f, 1.0f };
//
    //            VkRenderPassBeginInfo vkRenderPassBeginInfo {};
    //            vkRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //            vkRenderPassBeginInfo.pNext             = nullptr;
    //            vkRenderPassBeginInfo.renderPass        = renderPass.handle;
    //            vkRenderPassBeginInfo.framebuffer       = frameBuffer.handle;
    //            vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };
    //            vkRenderPassBeginInfo.renderArea.extent = state.swapChain.selectedExtents;
    //            vkRenderPassBeginInfo.clearValueCount   = 1;
    //            vkRenderPassBeginInfo.pClearValues      = &clearColor;
//
    //            vkCmdBeginRenderPass(state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    //        }
//
    //        return bindResourceImpl(std::forward<TLogicalResourceTypes>(aInstances)...);
    //    }
    //};
    ////<-----------------------------------------------------------------------------
//
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceTransferrer
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {
    //        }
    //    }
    //};
    ////<-----------------------------------------------------------------------------
//
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceUnbinder
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {
    //        }
    //    }
    //};
    ////<-----------------------------------------------------------------------------
//
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceUnloader
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {
    //        }
    //    }
    //};
    ////<-----------------------------------------------------------------------------
//
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
    //template <typename TRHIResource, typename... TLogicalResourceDependencies>
    //struct SResourceDestructor
    //{
    //    CEngineResult<> operator()(TRHIResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
    //    {
    //        CaseIfResourceTypesMatches(SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIImageView, SRHIImage)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIBufferView, SRHIBuffer)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIPipeline)
    //        {
    //        }
    //        CaseIfResourceTypesMatches(SRHIRenderPass, SRHIFrameBuffer)
    //        {
    //        }
    //    }
    //};
    ////<-----------------------------------------------------------------------------
    ////
    ////<-----------------------------------------------------------------------------
//
    //#undef ForwardResourceTypesInstances
    //#undef CaseIfResourceTypesMatches
}

#endif //__SHIRABEDEVELOPMENT_VULKANRESOURCEOPERATIONS_H__

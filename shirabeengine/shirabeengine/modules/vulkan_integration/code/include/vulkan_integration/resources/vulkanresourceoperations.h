//
// Created by dotti on 06.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_VULKANRESOURCEOPERATIONS_H__
#define __SHIRABEDEVELOPMENT_VULKANRESOURCEOPERATIONS_H__

#include <base/cpp_tools.h>
#include <resources/resourcetypes.h>
#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkanbufferviewresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/resources/types/vulkanrenderpassresource.h"
#include "vulkan_integration/resources/types/vulkanframebufferresource.h"
#include "vulkan_integration/resources/types/vulkanpipelineresource.h"
#include "vulkan_integration/resources/ivkglobalcontext.h"

namespace engine::resources
{
    using namespace vulkan;

    // TODO: Mesh, Material

    static EEngineStatus createTexture          (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus createTextureView      (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    static EEngineStatus createBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus createBufferView       (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    static EEngineStatus createRenderPass       (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    static EEngineStatus createFrameBuffer      (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    static EEngineStatus createGraphicsPipeline (Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
    static EEngineStatus loadTexture            (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus loadBuffer             (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus bindTexture            (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus bindTextureView        (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    static EEngineStatus bindBuffer             (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus bindBufferView         (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    static EEngineStatus bindRenderPass         (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    static EEngineStatus bindFrameBuffer        (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    static EEngineStatus bindGraphicsPipeline   (Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
    static EEngineStatus transferTexture        (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus transferBuffer         (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus unbindTexture          (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus unbindTextureView      (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    static EEngineStatus unbindBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus unbindBufferView       (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    static EEngineStatus unbindRenderPass       (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    static EEngineStatus unbindFrameBuffer      (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    static EEngineStatus unbindGraphicsPipeline (Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);
    static EEngineStatus unloadTexture          (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus unloadBuffer           (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus destroyTexture         (Shared<IVkGlobalContext> &aVulkanContext, STexture     const &aTexture);
    static EEngineStatus destroyTextureView     (Shared<IVkGlobalContext> &aVulkanContext, STextureView const &aTextureView, STexture const &aTexture);
    static EEngineStatus destroyBuffer          (Shared<IVkGlobalContext> &aVulkanContext, SBuffer      const &aBuffer);
    static EEngineStatus destroyBufferView      (Shared<IVkGlobalContext> &aVulkanContext, SBufferView  const &aBufferView, SBuffer const &aBuffer);
    static EEngineStatus destroyRenderPass      (Shared<IVkGlobalContext> &aVulkanContext, SRenderPass  const &aRenderPass);
    static EEngineStatus destroyFrameBuffer     (Shared<IVkGlobalContext> &aVulkanContext, SFrameBuffer const &aFrameBuffer, SRenderPass const &aRenderPass);
    static EEngineStatus destroyGraphicsPipeline(Shared<IVkGlobalContext> &aVulkanContext, SPipeline    const &aPipeline);

    #define CaseIfResourceTypesMatches(...) \
        if constexpr(std::is_same_v<variadic_tuple_t<TLogicalResource, TLogicalResourceDependencies...>, variadic_tuple_t<__VA_ARGS__>>)

    #define ForwardResourceTypesInstances(aResourceIdentifier, aDependencyIdentifier) \
        std::forward<TLogicalResource>(aResourceIdentifier), std::forward<TLogicalResourceDependencies>(aDependencyIdentifier)...
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceCreator
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            { return createTexture(ForwardResourceTypesInstances(aResource, aDependencies)); }
            CaseIfResourceTypesMatches(STextureView, STexture)
            { return createTextureView(ForwardResourceTypesInstances(aResource, aDependencies)); }
            CaseIfResourceTypesMatches(SBuffer)
            {}
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {}
            CaseIfResourceTypesMatches(SPipeline)
            {}
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {}
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceLoader
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            {}
            CaseIfResourceTypesMatches(STextureView, STexture)
            {}
            CaseIfResourceTypesMatches(SBuffer)
            {}
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {}
            CaseIfResourceTypesMatches(SPipeline)
            {}
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {}
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceBinder
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            {}
            CaseIfResourceTypesMatches(STextureView, STexture)
            {}
            CaseIfResourceTypesMatches(SBuffer)
            {}
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {}
            CaseIfResourceTypesMatches(SPipeline)
            {}
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {
                SVulkanState &state = mVulkanEnvironment->getState();

                VkClearValue clearColor = { 0.0f, 0.5f, 0.5f, 1.0f };

                VkRenderPassBeginInfo vkRenderPassBeginInfo {};
                vkRenderPassBeginInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                vkRenderPassBeginInfo.pNext             = nullptr;
                vkRenderPassBeginInfo.renderPass        = renderPass.handle;
                vkRenderPassBeginInfo.framebuffer       = frameBuffer.handle;
                vkRenderPassBeginInfo.renderArea.offset = { 0, 0 };
                vkRenderPassBeginInfo.renderArea.extent = state.swapChain.selectedExtents;
                vkRenderPassBeginInfo.clearValueCount   = 1;
                vkRenderPassBeginInfo.pClearValues      = &clearColor;

                vkCmdBeginRenderPass(state.commandBuffers.at(state.swapChain.currentSwapChainImageIndex), &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            }

            return bindResourceImpl(std::forward<TLogicalResourceTypes>(aInstances)...);
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceTransferrer
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            {
            }
            CaseIfResourceTypesMatches(STextureView, STexture)
            {
            }
            CaseIfResourceTypesMatches(SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SPipeline)
            {
            }
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {
            }
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceUnbinder
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            {
            }
            CaseIfResourceTypesMatches(STextureView, STexture)
            {
            }
            CaseIfResourceTypesMatches(SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SPipeline)
            {
            }
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {
            }
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceUnloader
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            {
            }
            CaseIfResourceTypesMatches(STextureView, STexture)
            {
            }
            CaseIfResourceTypesMatches(SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SPipeline)
            {
            }
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {
            }
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TLogicalResource, typename... TLogicalResourceDependencies>
    struct SResourceDestructor
    {
        CEngineResult<> operator()(TLogicalResource &&aResource, TLogicalResourceDependencies &&...aDependencies)
        {
            CaseIfResourceTypesMatches(STexture)
            {
            }
            CaseIfResourceTypesMatches(STextureView, STexture)
            {
            }
            CaseIfResourceTypesMatches(SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SBufferView, SBuffer)
            {
            }
            CaseIfResourceTypesMatches(SPipeline)
            {
            }
            CaseIfResourceTypesMatches(SRenderPass, SFrameBuffer)
            {
            }
        }
    };
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------

    #undef ForwardResourceTypesInstances
    #undef CaseIfResourceTypesMatches
}

#endif //__SHIRABEDEVELOPMENT_VULKANRESOURCEOPERATIONS_H__

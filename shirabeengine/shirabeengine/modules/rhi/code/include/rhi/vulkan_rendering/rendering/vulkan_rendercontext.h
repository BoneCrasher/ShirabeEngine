//
// Created by dottideveloper on 28.12.21.
//

#ifndef __SHIRABEDEVELOPMENT_VULKAN_RENDERCONTEXT_H__
#define __SHIRABEDEVELOPMENT_VULKAN_RENDERCONTEXT_H__

#include <vulkan/vulkan.hpp>
#include <platform/platform.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include "rhi/vulkan_resources/resources/types/all.h"

namespace engine::rhi
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    struct SSampledImageBinding
    {
        OptRef_t<RHIImageViewResourceState_t> imageView;
        OptRef_t<RHIImageResourceState_t>     image;
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto clearAttachments(Shared<CVulkanEnvironment>     aVulkanEnvironment
                          , Shared<CRHIResourceManager>       aResourceManager
                          , Shared<asset::CAssetStorage>   aAssetStorage
                          , SRenderGraphRenderContextState &aState
                          , std::string             const &aRenderPassId) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto nextSubpass(Shared<CVulkanEnvironment>     aVulkanEnvironment
                     , Shared<CRHIResourceManager>       aResourceManager
                     , Shared<asset::CAssetStorage>   aAssetStorage
                     , SRenderGraphRenderContextState &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto copyImage(Shared<CVulkanEnvironment>       aVulkanEnvironment
                   , Shared<CRHIResourceManager>         aResourceManager
                   , Shared<asset::CAssetStorage>     aAssetStorage
                   , SRenderGraphRenderContextState   &aState
                   , SRenderGraphImage const &aSourceImageId
                   , SRenderGraphImage const &aTargetImageId) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto performImageLayoutTransfer(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                    , Shared<CRHIResourceManager>         aResourceManager
                                    , Shared<asset::CAssetStorage>     aAssetStorage
                                    , SRenderGraphRenderContextState   &aState
                                    , SRenderGraphImage const &aTexture
                                    , CRange                    const &aArrayRange
                                    , CRange                    const &aMipRange
                                    , VkImageAspectFlags        const &aAspectFlags
                                    , VkImageLayout             const &aSourceLayout
                                    , VkImageLayout             const &aTargetLayout) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto copyImageToBackBuffer(Shared<CVulkanEnvironment>           aVulkanEnvironment
                               , Shared<CRHIResourceManager>             aResourceManager
                               , Shared<asset::CAssetStorage>         aAssetStorage
                               , SRenderGraphRenderContextState const &aState
                               , SRenderGraphImage     const &aSourceImageId) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto beginFrameCommandBuffers(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                  , Shared<CRHIResourceManager>             aResourceManager
                                  , Shared<asset::CAssetStorage>         aAssetStorage
                                  , SRenderGraphRenderContextState const &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto endFrameCommandBuffers(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                , Shared<CRHIResourceManager>             aResourceManager
                                , Shared<asset::CAssetStorage>         aAssetStorage
                                , SRenderGraphRenderContextState const &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto bindRenderPass(Shared<CVulkanEnvironment>           aVulkanEnvironment
                        , Shared<CRHIResourceManager>             aResourceManager
                        , Shared<asset::CAssetStorage>         aAssetStorage
                        , SRenderGraphRenderContextState const &aRenderContextState
                        , ResourceId_t                  const &aRenderPassId
                        , ResourceId_t                  const &aFrameBufferId) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto unbindRenderPass(Shared<CVulkanEnvironment>     aVulkanEnvironment
                          , Shared<CRHIResourceManager>       aResourceManager
                          , Shared<asset::CAssetStorage>   aAssetStorage
                          , SRenderGraphRenderContextState &aState
                          , ResourceId_t            const &aFrameBufferId
                          , ResourceId_t            const &aRenderPassId) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto beginGraphicsFrame(Shared<CVulkanEnvironment>     aVulkanEnvironment
                            , Shared<CRHIResourceManager>       aResourceManager
                            , Shared<asset::CAssetStorage>   aAssetStorage
                            , SRenderGraphRenderContextState &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto endGraphicsFrame(Shared<CVulkanEnvironment>     aVulkanEnvironment
                          , Shared<CRHIResourceManager>       aResourceManager
                          , Shared<asset::CAssetStorage>   aAssetStorage
                          , SRenderGraphRenderContextState &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    auto present(Shared<CVulkanEnvironment>     aVulkanEnvironment
                 , Shared<CRHIResourceManager>       aResourceManager
                 , Shared<asset::CAssetStorage>   aAssetStorage
                 , SRenderGraphRenderContextState &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto useMesh(Shared<CVulkanEnvironment>           aVulkanEnvironment
                 , Shared<CRHIResourceManager>             aResourceManager
                 , Shared<asset::CAssetStorage>         aAssetStorage
                 , SRenderGraphRenderContextState       &aState
                 , SRenderGraphMesh               const &aMesh) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto useMaterialWithPipeline(Shared<CVulkanEnvironment>       aVulkanEnvironment
                                 , Shared<CRHIResourceManager>         aResourceManager
                                 , Shared<asset::CAssetStorage>     aAssetStorage
                                 , SRenderGraphRenderContextState   &aState
                                 , SRenderGraphMaterial       const &aMaterial
                                 , SRenderGraphPipeline       const &aPipeline) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto bindPipeline(Shared<CVulkanEnvironment>     aVulkanEnvironment
                      , Shared<CRHIResourceManager>       aResourceManager
                      , Shared<asset::CAssetStorage>   aAssetStorage
                      , SRenderGraphRenderContextState &aState
                      , SRenderGraphPipeline     const &aPipeline) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto drawIndexed(Shared<CVulkanEnvironment>           aVulkanEnvironment
                     , Shared<CRHIResourceManager>             aResourceManager
                     , Shared<asset::CAssetStorage>         aAssetStorage
                     , SRenderGraphRenderContextState       &aState
                     , VkDeviceSize                  const  aIndexCount) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto drawQuad(Shared<CVulkanEnvironment>     aVulkanEnvironment
                  , Shared<CRHIResourceManager>       aResourceManager
                  , Shared<asset::CAssetStorage>   aAssetStorage
                  , SRenderGraphRenderContextState &aState) -> EEngineStatus;
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    auto drawFullscreenQuadWithMaterial(Shared<CVulkanEnvironment>           aVulkanEnvironment
                                        , Shared<CRHIResourceManager>             aResourceManager
                                        , Shared<asset::CAssetStorage>         aAssetStorage
                                        , SRenderGraphRenderContextState       &aState
                                        , SRenderGraphMaterial           const &aMaterial) -> EEngineStatus;
}

#endif //__SHIRABEDEVELOPMENT_VULKAN_RENDERCONTEXT_H__

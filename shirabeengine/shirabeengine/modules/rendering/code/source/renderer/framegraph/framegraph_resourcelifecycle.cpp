#include <resources/resourcetypes.h>

#include "renderer/framegraph/framegraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeGraphResources(SFrameGraphRenderContextState &aRenderContextState
                                                       , SFrameGraphRenderContext      &aRenderContext
                                                       , std::vector<PassUID_t>        &aPassExecutionOrder)
        {
            CEngineResult<> const setUpRenderPassAndFrameBuffer =
                    initializeRenderPassAndFrameBuffer(aRenderContextState
                                                     , aRenderContext
                                                     , aPassExecutionOrder
                                                     , sRenderPassResourceId
                                                     , sFrameBufferResourceId);
            if(not setUpRenderPassAndFrameBuffer.successful())
            {
                return setUpRenderPassAndFrameBuffer;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeGraphResources(SFrameGraphRenderContextState &aRenderContextState
                                                         , SFrameGraphRenderContext      &aRenderContext)
        {
            CEngineResult<> const setUpRenderPassAndFrameBuffer =
                    deinitializeRenderPassAndFrameBuffer(aRenderContextState
                                                       , aRenderContext
                                                       , sRenderPassResourceId
                                                       , sFrameBufferResourceId);
            if(not setUpRenderPassAndFrameBuffer.successful())
            {
                return setUpRenderPassAndFrameBuffer;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeRenderPassAndFrameBuffer(
                SFrameGraphRenderContextState         &aRenderContextState,
                SFrameGraphRenderContext              &aRenderContext,
                std::vector<PassUID_t>          const &aPassExecutionOrder,
                std::string                     const &aRenderPassId,
                std::string                     const &aFrameBufferId)
        {
            auto const &attachments           = mResourceData.getAttachments();
            auto const &attachmentResourceIds = attachments.getAttachementImageViewResourceIds();
            auto const &assignment            = attachments.getAttachmentPassToViewAssignment();

            // Make sure that all texture views and their subjacent textures are created upfront!
            CEngineResult<> const initialization = initializeResources(aRenderContextState, aRenderContext, attachmentResourceIds);
            if(not initialization.successful())
            {
                EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to initialize resource list.");
                return initialization;
            }

            CEngineResult<> const renderPassCreation = aRenderContext.createRenderPass(aRenderContextState, aRenderPassId, aPassExecutionOrder, attachments, mResourceData);
            EngineStatusPrintOnError(renderPassCreation.result(), logTag(), "Failed to create render pass.");

            CEngineResult<> const renderPassInit = aRenderContext.initializeRenderPass(aRenderContextState, aRenderPassId, aPassExecutionOrder, attachments, mResourceData);
            EngineStatusPrintOnError(renderPassInit.result(), logTag(), "Failed to initialize render pass.");

            CEngineResult<> const frameBufferCreation = aRenderContext.createFrameBuffer(aRenderContextState, aFrameBufferId, aRenderPassId);
            EngineStatusPrintOnError(frameBufferCreation.result(), logTag(), "Failed to create framebuffer.");

            CEngineResult<> const frameBufferInit = aRenderContext.initializeFrameBuffer(aRenderContextState, aFrameBufferId, aRenderPassId, mResourceData);
            EngineStatusPrintOnError(frameBufferInit.result(), logTag(), "Failed to initialize framebuffer.");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeRenderPassAndFrameBuffer(
                SFrameGraphRenderContextState       &aRenderContextState,
                SFrameGraphRenderContext            &aRenderContext,
                std::string                   const &aRenderPassId,
                std::string                   const &aFrameBufferId)
        {
            SFrameGraphAttachmentCollection const &attachments           = mResourceData.getAttachments();
            FrameGraphResourceIdList        const &attachmentResourceIds = attachments.getAttachementImageResourceIds();

            CEngineResult<> const frameBufferDestruction = aRenderContext.destroyFrameBuffer(aRenderContextState, aFrameBufferId);
            EngineStatusPrintOnError(frameBufferDestruction.result(), logTag(), "Failed to destroy frame buffer.");

            CEngineResult<> const renderPassDestruction = aRenderContext.destroyRenderPass(aRenderContextState, aRenderPassId);
            EngineStatusPrintOnError(renderPassDestruction.result(), logTag(), "Failed to destroy render pass.");

            CEngineResult<> const deinitialization = deinitializeResources(aRenderContextState, aRenderContext, attachmentResourceIds);
            EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize resources.");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeResources(
                SFrameGraphRenderContextState  &aRenderContextState,
                SFrameGraphRenderContext       &aRenderContext,
                FrameGraphResourceIdList const &aResourceIds)
        {
            CEngineResult<> initialization { EEngineStatus::Ok };
            CEngineResult<> result         { EEngineStatus::Ok };

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                CEngineResult<Shared<SFrameGraphResource>> subjacentFetch;

                Shared<SFrameGraphResource>       subjacent   = nullptr;
                Shared<SFrameGraphDynamicTexture> texture     = nullptr;
                Shared<SFrameGraphTextureView>    textureView = nullptr;

                auto it = mInstantiatedResources.end();

                Shared<SFrameGraphResource> const resource = mResourceData.getResource<SFrameGraphResource>(id).data();
                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                {
                    texture = std::static_pointer_cast<SFrameGraphDynamicTexture>(resource);

                    it = std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), texture->resourceId);
                    if(mInstantiatedResources.end() == it)
                    {
                        initialization = initializeTexture(aRenderContextState, aRenderContext, texture);
                        if(not initialization.successful())
                        {
                            EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to initialize texture.");
                            result = initialization;
                        }
                        else
                        {
                            mInstantiatedResources.push_back(texture->resourceId);
                        }
                    }

                    break;
                }
                case EFrameGraphResourceType::TextureView:
                {
                    subjacentFetch = mResourceData.getResourceMutable<SFrameGraphResource>(resource->subjacentResource);
                    if(not subjacentFetch.successful())
                    {
                        EngineStatusPrintOnError(subjacentFetch.result(), logTag(), "Failed to fetch subjacent resource.");
                        result = CEngineResult<>(subjacentFetch.result());
                    }

                    subjacent   = subjacentFetch.data();
                    texture     = std::static_pointer_cast<SFrameGraphDynamicTexture>(subjacent);
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);                    

                    it = std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), textureView->resourceId);
                    if(mInstantiatedResources.end() == it)
                    {
                        bool const subjacentTextureCreated = (mInstantiatedResources.end() != std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), texture->resourceId));
                        if(not texture->isExternalResource && not subjacentTextureCreated)
                        {
                            initialization = initializeResources(aRenderContextState, aRenderContext, {texture->resourceId});
                            if(not initialization.successful())
                            {
                                EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to initialize texture.");
                                result = initialization;
                                break;
                            }
                            else
                            {
                                mInstantiatedResources.push_back(textureView->resourceId);
                            }
                        }

                        // Don't forget to create the texture view...
                        CEngineResult<> const textureViewInitialization = initializeTextureView(aRenderContextState, aRenderContext, texture, textureView);
                        if(not textureViewInitialization.successful())
                        {
                            EngineStatusPrintOnError(textureViewInitialization.result(), logTag(), "Failed to initialize texture view.");
                            result = textureViewInitialization;
                            break;
                        }
                        else
                        {
                            mInstantiatedResources.push_back(textureView->resourceId);
                        }
                    }
                }
                break;
                default:
                    break;
                }
            }

            return result;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeResources(
                SFrameGraphRenderContextState  &aRenderContextState,
                SFrameGraphRenderContext       &aRenderContext,
                FrameGraphResourceIdList const &aResourceIds)
        {
            SHIRABE_UNUSED(aRenderContext);

            std::function<CEngineResult<>(FrameGraphResourceId_t const&, bool)> recurse = nullptr;

            recurse = [&, this] (FrameGraphResourceId_t const &aId, bool aIncludeSubjacent) -> CEngineResult<>
            {
                SHIRABE_UNUSED(aIncludeSubjacent);

                CEngineResult<> deinitialized = { EEngineStatus::Ok };

                Shared<SFrameGraphResource>       resource    = mResourceData.getResourceMutable<SFrameGraphResource>(aId).data();
                Shared<SFrameGraphResource>       subjacent   = nullptr;
                Shared<SFrameGraphDynamicTexture> texture     = nullptr;
                Shared<SFrameGraphTextureView>    textureView = nullptr;

                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    //if(aIncludeSubjacent)
                    //{
                    //    texture = std::static_pointer_cast<SFrameGraphDynamicTexture>(resource);
//
                    //    if(not texture->isExternalResource and resource->referenceCount == 0)
                    //    {
                    //        deinitialized = deinitializeTexture(aRenderContext, texture);
//
                    //        auto const condition = [&] (FrameGraphResourceId_t const &aId) -> bool
                    //        {
                    //            return (aId == texture->resourceId);
                    //        };
//
                    //        auto const iterator = std::remove_if(mInstantiatedResources.begin(),
                    //                                             mInstantiatedResources.end(),
                    //                                             condition);
                    //    }
                    //}
                    break;
                case EFrameGraphResourceType::TextureView:
                    // Decrease the texture view's count
                    --(resource->referenceCount);

                    std::cout << CString::format("TextureView Id {} -> RefCount: {}\n", resource->resourceId, resource->referenceCount);

                    if(resource->referenceCount == 0)
                    {
                        CEngineResult<Shared<SFrameGraphResource>> subjacentFetch = mResourceData.getResource<SFrameGraphResource>(resource->subjacentResource);
                        if(not subjacentFetch.successful())
                        {
                            EngineStatusPrintOnError(subjacentFetch.result(), logTag(), "Failed to fetch subjacent resource.");
                            return CEngineResult<>(subjacentFetch.result());
                        }

                        subjacent   = subjacentFetch.data();
                        texture     = std::static_pointer_cast<SFrameGraphDynamicTexture>(subjacent);
                        textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);

                        deinitialized = deinitializeTextureView(aRenderContextState, aRenderContext, texture, textureView);

                        auto const condition = [&] (FrameGraphResourceId_t const &aId) -> bool
                        {
                            return (aId == textureView->resourceId);
                        };

                        auto const iterator = std::remove_if(mInstantiatedResources.begin(),
                                                             mInstantiatedResources.end(),
                                                             condition);

                        --(texture->referenceCount);

                        // std::cout << CString::format("Texture Id {} -> RefCount: {}\n", texture->resourceId, texture->referenceCount);

                        if(texture->referenceCount == 0)
                        {
                            deinitialized = recurse(texture->resourceId, true);
                        }
                    }

                    break;
                default:
                    break;
                }

                return deinitialized;
            };

            // Fire!!!
            CEngineResult<> deinitialized = EEngineStatus::Ok;

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                deinitialized = recurse(id, false);
                if(not deinitialized.successful())
                {
                    break;
                }
            }

            return deinitialized;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeTexture(
                SFrameGraphRenderContextState           &aRenderContextState,
                SFrameGraphRenderContext                &aRenderContext,
                Shared<SFrameGraphDynamicTexture> const &aTexture)
        {
            EEngineStatus initialization = EEngineStatus::Ok;

            if(aTexture->isExternalResource)
                initialization = aRenderContext.importDynamicTexture(aRenderContextState, *aTexture);
            else
                initialization = aRenderContext.createDynamicTexture(aRenderContextState, *aTexture);

            EngineStatusPrintOnError(initialization, logTag(), "Failed to load texture for FrameGraphExecution.");

            return initialization;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeTextureView(
                SFrameGraphRenderContextState           &aRenderContextState,
                SFrameGraphRenderContext                &aRenderContext,
                Shared<SFrameGraphDynamicTexture> const &aTexture,
                Shared<SFrameGraphTextureView>    const &aTextureView)
        {
            bool const isForwardedOrAccepted =
                    ((aTextureView->mode.check(EFrameGraphViewAccessMode::Forward)) ||
                     (aTextureView->mode.check(EFrameGraphViewAccessMode::Accept)));
            if(isForwardedOrAccepted)
            {
                return { EEngineStatus::Ok }; // Nothing to be done and no error.
            }

            EEngineStatus creation = aRenderContext.createTextureView(aRenderContextState, *aTexture, *aTextureView);
            EngineStatusPrintOnError(creation, logTag(), "Failed to load texture view for FrameGraphExecution.");

            return creation;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeTextureView(
                SFrameGraphRenderContextState           &aRenderContextState,
                SFrameGraphRenderContext                &aRenderContext,
                Shared<SFrameGraphDynamicTexture> const &aTexture,
                Shared<SFrameGraphTextureView>    const &aTextureView)
        {
            SHIRABE_UNUSED(aTexture);

            bool const isForwardedOrAccepted =
                    ((aTextureView->mode.check(EFrameGraphViewAccessMode::Forward)) ||
                     (aTextureView->mode.check(EFrameGraphViewAccessMode::Accept)));
            if(isForwardedOrAccepted)
            {
                return { EEngineStatus::Ok }; // Nothing to be done and no error.
            }

            EEngineStatus destruction = aRenderContext.destroyTextureView(aRenderContextState, *aTextureView);
            EngineStatusPrintOnError(destruction, logTag(), "Failed to unload texture view for FrameGraphExecution.");

            return destruction;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeTexture(
                SFrameGraphRenderContextState           &aRenderContextState,
                SFrameGraphRenderContext                &aRenderContext,
                Shared<SFrameGraphDynamicTexture> const &aTexture)
        {
            SHIRABE_UNUSED(aRenderContext);

            if(aTexture->isExternalResource)
            {
                return { EEngineStatus::Ok };
            }

            EEngineStatus destruction = aRenderContext.destroyDynamicTexture(aRenderContextState, *aTexture);
            EngineStatusPrintOnError(destruction, logTag(), "Failed to unload texture for FrameGraphExecution.");

            return destruction;
        }
        //<-----------------------------------------------------------------------------
    }
}

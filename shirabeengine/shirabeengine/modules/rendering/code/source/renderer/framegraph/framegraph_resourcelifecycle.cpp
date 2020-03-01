#include <resources/resourcetypes.h>

#include "renderer/framegraph/framegraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeGraphResources(SFrameGraphResourceContext &aResourceContext
                                                       , std::vector<PassUID_t>     &aPassExecutionOrder)
        {
            CEngineResult<> const setUpRenderPassAndFrameBuffer =
                    initializeRenderPassAndFrameBuffer(aResourceContext
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
        CEngineResult<> CGraph::deinitializeGraphResources(SFrameGraphResourceContext &aResourceContext)
        {
            CEngineResult<> const setUpRenderPassAndFrameBuffer =
                    deinitializeRenderPassAndFrameBuffer(aResourceContext
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
                SFrameGraphResourceContext            &aResourceContext,
                std::vector<PassUID_t>          const &aPassExecutionOrder,
                std::string                     const &aRenderPassId,
                std::string                     const &aFrameBufferId)
        {

            auto const &attachments           = mResourceData.getAttachments();
            auto const &attachmentResourceIds = attachments.getAttachementImageViewResourceIds();
            auto const &assignment            = attachments.getAttachmentPassToViewAssignment();

            // Make sure that all texture views and their subjacent textures are created upfront!
            CEngineResult<> const initialization = initializeResources(aResourceContext, attachmentResourceIds);
            if(not initialization.successful())
            {
                EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to initialize resource list.");
                return initialization;
            }

            CEngineResult<> const renderPassCreation = aResourceContext.createRenderPass(aRenderPassId, aPassExecutionOrder, attachments, mResourceData);
            EngineStatusPrintOnError(renderPassCreation.result(), logTag(), "Failed to create render pass.");

            CEngineResult<> const renderPassInit = aResourceContext.initializeRenderPass(aRenderPassId, aPassExecutionOrder, attachments, mResourceData);
            EngineStatusPrintOnError(renderPassInit.result(), logTag(), "Failed to initialize render pass.");

            CEngineResult<> const frameBufferCreation = aResourceContext.createFrameBuffer(aFrameBufferId, aRenderPassId);
            EngineStatusPrintOnError(frameBufferCreation.result(), logTag(), "Failed to create framebuffer.");

            CEngineResult<> const frameBufferInit = aResourceContext.initializeFrameBuffer(aFrameBufferId, aRenderPassId, mResourceData);
            EngineStatusPrintOnError(frameBufferInit.result(), logTag(), "Failed to initialize framebuffer.");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeRenderPassAndFrameBuffer(
                SFrameGraphResourceContext &aResourceContext,
                std::string          const &aRenderPassId,
                std::string          const &aFrameBufferId)
        {
            SFrameGraphAttachmentCollection const &attachments           = mResourceData.getAttachments();
            FrameGraphResourceIdList        const &attachmentResourceIds = attachments.getAttachementImageResourceIds();

            CEngineResult<> const frameBufferDestruction = aResourceContext.destroyFrameBuffer(aFrameBufferId);
            EngineStatusPrintOnError(frameBufferDestruction.result(), logTag(), "Failed to destroy frame buffer.");

            CEngineResult<> const renderPassDestruction = aResourceContext.destroyRenderPass(aRenderPassId);
            EngineStatusPrintOnError(renderPassDestruction.result(), logTag(), "Failed to destroy render pass.");

            CEngineResult<> const deinitialization = deinitializeResources(aResourceContext, attachmentResourceIds);
            EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize resources.");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeResources(
                SFrameGraphResourceContext     &aResourceContext,
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
                        initialization = initializeTexture(aResourceContext, texture);
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
                            initialization = initializeResources(aResourceContext, {texture->resourceId});
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
                        CEngineResult<> const textureViewInitialization = initializeTextureView(aResourceContext, texture, textureView);
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
                SFrameGraphResourceContext     &aResourceContext,
                FrameGraphResourceIdList const &aResourceIds)
        {
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

                        deinitialized = deinitializeTextureView(aResourceContext, texture, textureView);

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
                SFrameGraphResourceContext              &aResourceContext,
                Shared<SFrameGraphDynamicTexture> const &aTexture)
        {
            EEngineStatus initialization = EEngineStatus::Ok;

            if(aTexture->isExternalResource)
                initialization = aResourceContext.importDynamicTexture(*aTexture);
            else
                initialization = aResourceContext.createDynamicTexture(*aTexture);

            EngineStatusPrintOnError(initialization, logTag(), "Failed to load texture for FrameGraphExecution.");

            return initialization;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeTextureView(
                SFrameGraphResourceContext              &aResourceContext,
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

            EEngineStatus creation = aResourceContext.createTextureView(*aTexture, *aTextureView);
            EngineStatusPrintOnError(creation, logTag(), "Failed to load texture view for FrameGraphExecution.");

            return creation;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeTextureView(
                SFrameGraphResourceContext              &aResourceContext,
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

            EEngineStatus destruction = aResourceContext.destroyTextureView(*aTextureView);
            EngineStatusPrintOnError(destruction, logTag(), "Failed to unload texture view for FrameGraphExecution.");

            return destruction;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeTexture(
                SFrameGraphResourceContext              &aResourceContext,
                Shared<SFrameGraphDynamicTexture> const &aTexture)
        {
            if(aTexture->isExternalResource)
            {
                return { EEngineStatus::Ok };
            }

            EEngineStatus destruction = aResourceContext.destroyDynamicTexture(*aTexture);
            EngineStatusPrintOnError(destruction, logTag(), "Failed to unload texture for FrameGraphExecution.");

            return destruction;
        }
        //<-----------------------------------------------------------------------------
    }
}

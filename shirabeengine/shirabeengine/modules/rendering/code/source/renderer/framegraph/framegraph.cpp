#include <resources/resourcetypes.h>

#include "renderer/framegraph/framegraph.h"

namespace engine
{
    namespace framegraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CGraph::CAccessor::CAccessor(CGraph const *aGraph)
            : m_graph(aGraph)
        { }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AdjacencyListMap_t<PassUID_t> const &CGraph::CAccessor::passAdjacency() const
        {
            return m_graph->mPassAdjacency;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::stack<PassUID_t> const &CGraph::CAccessor::passExecutionOrder() const
        {
            return m_graph->mPassExecutionOrder;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceIdList const &CGraph::CAccessor::resources() const
        {
            return m_graph->mResources;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphMutableResources const &CGraph::CAccessor::resourceData() const
        {
            return m_graph->mResourceData;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

        AdjacencyListMap_t<FrameGraphResourceId_t> const &CGraph::CAccessor::resourceAdjacency() const
        {
            return m_graph->mResourceAdjacency;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::stack<FrameGraphResourceId_t> const &CGraph::CAccessor::resourceOrder() const
        {
            return m_graph->mResourceOrder;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> const &CGraph::CAccessor::passToResourceAdjacency() const
        {
            return m_graph->mPassToResourceAdjacency;
        }

#endif
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CGraph::EGraphMode CGraph::CAccessor::graphMode() const
        {
            return m_graph->mGraphMode;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::CAccessor::renderToBackBuffer() const
        {
            return m_graph->mRenderToBackBuffer;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceId_t const &CGraph::CAccessor::outputTextureResourceId() const
        {
            return m_graph->mOutputTextureResourceId;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CGraph::CMutableAccessor::CMutableAccessor(CGraph *aGraph)
            : CAccessor(aGraph)
            , mGraph(aGraph)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AdjacencyListMap_t<PassUID_t> &CGraph::CMutableAccessor::mutablePassAdjacency()
        {
            return mGraph->mPassAdjacency;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::stack<PassUID_t> &CGraph::CMutableAccessor::mutablePassExecutionOrder()
        {
            return mGraph->mPassExecutionOrder;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceIdList &CGraph::CMutableAccessor::mutableResources()
        {
            return mGraph->mResources;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphMutableResources &CGraph::CMutableAccessor::mutableResourceData()
        {
            return mGraph->mResourceData;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION

        AdjacencyListMap_t<FrameGraphResourceId_t> &CGraph::CMutableAccessor::mutableResourceAdjacency()
        {
            return mGraph->mResourceAdjacency;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::stack<FrameGraphResourceId_t> &CGraph::CMutableAccessor::mutableResourceOrder()
        {
            return mGraph->mResourceOrder;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> &CGraph::CMutableAccessor::mutablePassToResourceAdjacency()
        {
            return mGraph->mPassToResourceAdjacency;
        }

#endif
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CGraph::EGraphMode &CGraph::CMutableAccessor::mutableGraphMode()
        {
            return mGraph->mGraphMode;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool &CGraph::CMutableAccessor::mutableRenderToBackBuffer()
        {
            return mGraph->mRenderToBackBuffer;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        FrameGraphResourceId_t &CGraph::CMutableAccessor::mutableOutputTextureResourceId()
        {
            return mGraph->mOutputTextureResourceId;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        bool CGraph::acceptSerializer(IFrameGraphSerializer &aSerializer) const
        {
            aSerializer.serializeGraph(*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::acceptDeserializer(IFrameGraphDeserializer &aDeserializer)
        {
            aDeserializer.deserializeGraph(*this);
        }
#endif
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CGraph& CGraph::operator=(CGraph const &aOther)
        {
            mPasses                  = aOther.mPasses;
            mPassAdjacency           = aOther.mPassAdjacency;
            mPassExecutionOrder      = aOther.mPassExecutionOrder;
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            mResourceAdjacency       = aOther.mResourceAdjacency;
            mResourceOrder           = aOther.mResourceOrder;
            mPassToResourceAdjacency = aOther.mPassToResourceAdjacency;
#endif
            mGraphMode               = EGraphMode::Compute;
            mRenderToBackBuffer      = false;

            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::execute(Shared<IFrameGraphRenderContext> &aRenderContext)
        {
            assert(aRenderContext != nullptr);

            if(EGraphMode::Graphics == mGraphMode)
            {
                if(mRenderToBackBuffer)
                {
                    aRenderContext->bindSwapChain(sSwapChainResourceId);
                }

                CEngineResult<> const setUpRenderPassAndFrameBuffer = initializeRenderPassAndFrameBuffer(aRenderContext, sRenderPassResourceId, sFrameBufferResourceId);
                if(not setUpRenderPassAndFrameBuffer.successful())
                {
                    return setUpRenderPassAndFrameBuffer;
                }

                //
                // All descriptor resources need to be created and pushed to the GPU up front
                //
                // for(auto const &bufferId : mResourceData.buffers())
                // {
                //     auto const &bufferResource = mResourceData.getMutable<SFrameGraphBuffer>(bufferId).data();
                //     aRenderContext->createBuffer(*bufferResource);
                // }
            }

            // In any case...
            aRenderContext->beginCommandBuffer();

            if(EGraphMode::Graphics == mGraphMode)
            {
                aRenderContext->bindRenderPass(sRenderPassResourceId, sFrameBufferResourceId);
            }

            std::stack<PassUID_t> copy = mPassExecutionOrder;
            while(not copy.empty())
            {
                aRenderContext->beginPass();

                PassUID_t                             const passUID  = copy.top();
                Shared<CPassBase>            const pass     = mPasses.at(passUID);
                Unique<CPassBase::CAccessor> const accessor = pass->getAccessor(CPassKey<CGraph>());

                CEngineResult<> executed = pass->execute(mResourceData, aRenderContext);
                if(not executed.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to execute pass %0", pass->passUID()));
                    break;
                }

                if(1 < copy.size()) // Implicit last pass '0' and effective last pass --> 2 passes
                {
                    aRenderContext->endPass();
                }

                copy.pop();
            }

            if(EGraphMode::Graphics == mGraphMode && mRenderToBackBuffer)
            {
                aRenderContext->unbindRenderPass(sRenderPassResourceId, sFrameBufferResourceId);

                CEngineResult<Shared<SFrameGraphTextureView>> sourceResourceFetch = mResourceData.get<SFrameGraphTextureView>(mOutputTextureResourceId);
                if(not sourceResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture view."));
                    return {sourceResourceFetch.result()};
                }

                CEngineResult<Shared<SFrameGraphTexture>> parentResourceFetch = mResourceData.get<SFrameGraphTexture>(sourceResourceFetch.data()->subjacentResource);
                if(not parentResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture."));
                    return {sourceResourceFetch.result()};
                }

                aRenderContext->copyImageToBackBuffer(*(parentResourceFetch.data()));
            }

            // In any case...
            aRenderContext->commitCommandBuffer();

            if(EGraphMode::Graphics == mGraphMode)
            {
                if(mRenderToBackBuffer)
                {
                    aRenderContext->present();
                }

                CEngineResult<> const cleanedUpRenderPassAndFrameBuffer = deinitializeRenderPassAndFrameBuffer(aRenderContext, sFrameBufferResourceId, sRenderPassResourceId);
                if(not cleanedUpRenderPassAndFrameBuffer.successful())
                {
                    return cleanedUpRenderPassAndFrameBuffer;
                }
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeResources(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList         const &aResourceIds)
        {
            CEngineResult<> initialization { EEngineStatus::Ok };
            CEngineResult<> result         { EEngineStatus::Ok };

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                CEngineResult<Shared<SFrameGraphResource>> subjacentFetch;

                Shared<SFrameGraphResource>    subjacent   = nullptr;
                Shared<SFrameGraphTexture>     texture     = nullptr;
                Shared<SFrameGraphTextureView> textureView = nullptr;

                FrameGraphResourceIdList::const_iterator it = mInstantiatedResources.end();

                Shared<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                {
                    texture = std::static_pointer_cast<SFrameGraphTexture>(resource);

                    it = std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), texture->resourceId);
                    if(mInstantiatedResources.end() == it)
                    {
                        initialization = initializeTexture(aRenderContext, texture);
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
                    subjacentFetch = mResourceData.getMutable<SFrameGraphResource>(resource->subjacentResource);
                    if(not subjacentFetch.successful())
                    {
                        EngineStatusPrintOnError(subjacentFetch.result(), logTag(), "Failed to fetch subjacent resource.");
                        result = CEngineResult<>(subjacentFetch.result());
                    }

                    subjacent   = subjacentFetch.data();
                    texture     = std::static_pointer_cast<SFrameGraphTexture>(subjacent);
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);                    

                    it = std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), textureView->resourceId);
                    if(mInstantiatedResources.end() == it)
                    {
                        bool const subjacentTextureCreated = (mInstantiatedResources.end() != std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), texture->resourceId));
                        if(not texture->isExternalResource && not subjacentTextureCreated)
                        {
                            initialization = initializeResources(aRenderContext, {texture->resourceId});
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
                        CEngineResult<> const textureViewInitialization = initializeTextureView(aRenderContext, texture, textureView);
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
        CEngineResult<> CGraph::initializeRenderPassAndFrameBuffer(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                std::string                      const &aRenderPassId,
                std::string                      const &aFrameBufferId)
        {
            std::vector<Shared<SFrameGraphTexture>>     textureReferences{};
            std::vector<Shared<SFrameGraphTextureView>> textureViewReferences{};

            SFrameGraphAttachmentCollection const &attachments           = mResourceData.getAttachments();
            FrameGraphResourceIdList        const &attachmentResourceIds = attachments  .getAttachementResourceIds();

            // Make sure that all texture views and their subjacent textures are created upfront!
            CEngineResult<> const initialization = initializeResources(aRenderContext, attachmentResourceIds);
            if(not initialization.successful())
            {
                EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to initialize resource list.");
                return initialization;
            }

            CEngineResult<> const renderPassCreation = aRenderContext->createRenderPass(aRenderPassId, attachments, mResourceData);
            EngineStatusPrintOnError(renderPassCreation.result(), logTag(), "Failed to create render pass.");

            CEngineResult<> const frameBufferCreation = aRenderContext->createFrameBuffer(aFrameBufferId, aRenderPassId);
            EngineStatusPrintOnError(frameBufferCreation.result(), logTag(), "Failed to create framebuffer.");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeRenderPassAndFrameBuffer(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                std::string                      const &aRenderPassId,
                std::string                      const &aFrameBufferId)
        {
            SFrameGraphAttachmentCollection const &attachments           = mResourceData.getAttachments();
            FrameGraphResourceIdList        const &attachmentResourceIds = attachments.getAttachementResourceIds();

            CEngineResult<> const frameBufferDestruction = aRenderContext->destroyFrameBuffer(aFrameBufferId);
            EngineStatusPrintOnError(frameBufferDestruction.result(), logTag(), "Failed to destroy frame buffer.");

            CEngineResult<> const renderPassDestruction = aRenderContext->destroyRenderPass(aRenderPassId);
            EngineStatusPrintOnError(renderPassDestruction.result(), logTag(), "Failed to destroy render pass.");

            CEngineResult<> const deinitialization = deinitializeResources(aRenderContext, attachmentResourceIds);
            EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize resources.");

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::bindResources(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            CEngineResult<> binding = EEngineStatus::Ok;

            for(FrameGraphResourceId_t const &id : aResourceIds)
            {
                Shared<SFrameGraphTextureView> textureView = nullptr;

                Shared<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    break;
                case EFrameGraphResourceType::TextureView:
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);
                    binding     = aRenderContext->bindTextureView(*textureView);
                    break;
                default:
                    break;
                }

                // Break out on first error.
                if(not binding.successful())
                {
                    break;
                }
            }

            return binding;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::unbindResources(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            CEngineResult<> unbinding = EEngineStatus::Ok;

            for(FrameGraphResourceId_t const &id : aResourceIds)
            {
                Shared<SFrameGraphTextureView> textureView = nullptr;

                Shared<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    break;
                case EFrameGraphResourceType::TextureView:
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);
                    unbinding   = aRenderContext->unbindTextureView(*textureView);
                    break;
                default:
                    break;
                }

                if(not unbinding.successful())
                {
                    break;
                }
            }

            return unbinding;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeResources(
                Shared<IFrameGraphRenderContext>      &aRenderContext,
                FrameGraphResourceIdList                  const&aResourceIds)
        {
            SHIRABE_UNUSED(aRenderContext);

            std::function<CEngineResult<>(FrameGraphResourceId_t const&, bool)> recurse = nullptr;

            recurse = [&, this] (FrameGraphResourceId_t const &aId, bool aIncludeSubjacent) -> CEngineResult<>
            {
                SHIRABE_UNUSED(aIncludeSubjacent);

                CEngineResult<> deinitialized = { EEngineStatus::Ok };

                Shared<SFrameGraphResource>    resource    = mResourceData.getMutable<SFrameGraphResource>(aId).data();
                Shared<SFrameGraphResource>    subjacent   = nullptr;
                Shared<SFrameGraphTexture>     texture     = nullptr;
                Shared<SFrameGraphTextureView> textureView = nullptr;

                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    if(aIncludeSubjacent)
                    {
                        texture = std::static_pointer_cast<SFrameGraphTexture>(resource);

                        if(not texture->isExternalResource and resource->referenceCount == 0)
                        {
                            deinitialized = deinitializeTexture(aRenderContext, texture);

                            auto const condition = [&] (FrameGraphResourceId_t const &aId) -> bool
                            {
                                return (aId == texture->resourceId);
                            };

                            auto const iterator = std::remove_if(mInstantiatedResources.begin(),
                                                                 mInstantiatedResources.end(),
                                                                 condition);
                        }
                    }
                    break;
                case EFrameGraphResourceType::TextureView:
                    // Decrease the texture view's count
                    --(resource->referenceCount);

                    std::cout << CString::format("TextureView Id %0 -> RefCount: %1\n", resource->resourceId, resource->referenceCount);

                    if(resource->referenceCount == 0)
                    {
                        CEngineResult<Shared<SFrameGraphResource> const> subjacentFetch = mResourceData.get<SFrameGraphResource>(resource->subjacentResource);
                        if(not subjacentFetch.successful())
                        {
                            EngineStatusPrintOnError(subjacentFetch.result(), logTag(), "Failed to fetch subjacent resource.");
                            return CEngineResult<>(subjacentFetch.result());
                        }

                        subjacent   = subjacentFetch.data();
                        texture     = std::static_pointer_cast<SFrameGraphTexture>(subjacent);
                        textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);

                        deinitialized = deinitializeTextureView(aRenderContext, texture, textureView);

                        auto const condition = [&] (FrameGraphResourceId_t const &aId) -> bool
                        {
                            return (aId == textureView->resourceId);
                        };

                        auto const iterator = std::remove_if(mInstantiatedResources.begin(),
                                                             mInstantiatedResources.end(),
                                                             condition);

                        --(texture->referenceCount);

                        // std::cout << CString::format("Texture Id %0 -> RefCount: %1\n", texture->resourceId, texture->referenceCount);

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
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                Shared<SFrameGraphTexture>       const &aTexture)
        {
            CEngineResult<> initialization = EEngineStatus::Ok;

            if(aTexture->isExternalResource)
                initialization = aRenderContext->importTexture(*aTexture);
            else
                initialization = aRenderContext->createTexture(*aTexture);

            EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to load texture for FrameGraphExecution.");

            return initialization;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeTextureView(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                Shared<SFrameGraphTexture>       const &aTexture,
                Shared<SFrameGraphTextureView>   const &aTextureView)
        {
            bool const isForwardedOrAccepted =
                    ((aTextureView->mode.check(EFrameGraphViewAccessMode::Forward)) ||
                     (aTextureView->mode.check(EFrameGraphViewAccessMode::Accept)));
            if(isForwardedOrAccepted)
            {
                return { EEngineStatus::Ok }; // Nothing to be done and no error.
            }

            CEngineResult<> creation = aRenderContext->createTextureView(*aTexture, *aTextureView);
            EngineStatusPrintOnError(creation.result(), logTag(), "Failed to load texture view for FrameGraphExecution.");

            return creation;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeTextureView(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                Shared<SFrameGraphTexture>       const &aTexture,
                Shared<SFrameGraphTextureView>   const &aTextureView)
        {
            SHIRABE_UNUSED(aTexture);

            bool const isForwardedOrAccepted =
                    ((aTextureView->mode.check(EFrameGraphViewAccessMode::Forward)) ||
                     (aTextureView->mode.check(EFrameGraphViewAccessMode::Accept)));
            if(isForwardedOrAccepted)
            {
                return { EEngineStatus::Ok }; // Nothing to be done and no error.
            }

            CEngineResult<> destruction = aRenderContext->destroyTextureView(*aTextureView);
            EngineStatusPrintOnError(destruction.result(), logTag(), "Failed to unload texture view for FrameGraphExecution.");

            return destruction;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeTexture(
                Shared<IFrameGraphRenderContext>       &aRenderContext,
                Shared<SFrameGraphTexture>       const &aTexture)
        {
            SHIRABE_UNUSED(aRenderContext);

            if(aTexture->isExternalResource)
            {
                return { EEngineStatus::Ok };
            }

            CEngineResult<> destruction = aRenderContext->destroyTexture(*aTexture);
            EngineStatusPrintOnError(destruction.result(), logTag(), "Failed to unload texture for FrameGraphExecution.");

            return destruction;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        PassMap const &CGraph::passes() const
        {
            return mPasses;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::addPass(Shared<CPassBase> const &aPass)
        {
            if(mPasses.end() != mPasses.find(aPass->passUID()))
            {
                return EEngineStatus::Error;
            }

            mPasses[aPass->passUID()] = aPass;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

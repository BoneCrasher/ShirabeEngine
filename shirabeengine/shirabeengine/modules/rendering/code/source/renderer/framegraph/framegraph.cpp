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
            if(&aOther == this)
            {
                return (*this);
            }

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
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeGraphResources()
        {
            CEngineResult<> const setUpRenderPassAndFrameBuffer = initializeRenderPassAndFrameBuffer(renderContextState, aRenderContext, executionOrder, sRenderPassResourceId, sFrameBufferResourceId);
            if(not setUpRenderPassAndFrameBuffer.successful())
            {
                return setUpRenderPassAndFrameBuffer;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeGraphResources()
        {
            CEngineResult<> const setUpRenderPassAndFrameBuffer = deinitializeRenderPassAndFrameBuffer(renderContextState, aRenderContext, executionOrder, sRenderPassResourceId, sFrameBufferResourceId);
            if(not setUpRenderPassAndFrameBuffer.successful())
            {
                return setUpRenderPassAndFrameBuffer;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::execute(SFrameGraphDataSource    const &aDataSource
                                      , SFrameGraphRenderContext       &aRenderContext)
        {
            // TODO: As soon as we have passgroups, the currentSubpassIndex comes from the passgroup.
            SFrameGraphRenderContextState renderContextState {};

            std::vector<PassUID_t> executionOrder {};
            {
                std::stack<PassUID_t> copy = mPassExecutionOrder;
                while(not copy.empty())
                {
                    executionOrder.push_back(copy.top());
                    copy.pop();
                }
            }

            // Preload all meshes for the frame and spawn resource tasks where necessary.
            RefIndex_t const &meshReferenceIndex = mResourceData.meshes();
            for(auto const &reference : meshReferenceIndex)
            {
                auto [fetchSuccessCode, mesh] = mResourceData.getMutable<SFrameGraphMesh>(reference);
                if(CheckEngineError(fetchSuccessCode))
                {
                    CLog::Error(logTag(), "Failed to fetch mesh w/ id {}", reference);
                    continue;
                }

                EEngineStatus attributeBufferStatus = EEngineStatus::Ok;
                EEngineStatus indexBufferStatus     = EEngineStatus::Ok;
                attributeBufferStatus = aRenderContext.initializeBuffer(renderContextState, mesh->attributeBuffer);
                if(not CheckEngineError(attributeBufferStatus))
                {
                    attributeBufferStatus = aRenderContext.transferBuffer(renderContextState, mesh->attributeBuffer);
                }
                indexBufferStatus = aRenderContext.initializeBuffer(renderContextState, mesh->indexBuffer);
                if(not CheckEngineError(indexBufferStatus))
                {
                    indexBufferStatus = aRenderContext.transferBuffer(renderContextState, mesh->indexBuffer);
                }
            }

            RefIndex_t const &materialReferenceIndex = mResourceData.materials();
            for(auto const &reference : materialReferenceIndex)
            {
                auto [fetchSuccessCode, material] = mResourceData.getMutable<SFrameGraphMaterial>(reference);
                if(CheckEngineError(fetchSuccessCode))
                {
                    CLog::Error(logTag(), "Failed to fetch material w/ id {}", reference);
                    continue;
                }
            }

            if(EGraphMode::Graphics == mGraphMode)
            {
                aRenderContext.beginGraphicsFrame(renderContextState);
            }

            // In any case...
            aRenderContext.beginFrameCommandBuffers(renderContextState);

            if(EGraphMode::Graphics == mGraphMode)
            {
                aRenderContext.bindRenderPass(renderContextState, sRenderPassResourceId, sFrameBufferResourceId, mResourceData);
            }

            std::stack<PassUID_t> copy = mPassExecutionOrder;
            while(not copy.empty())
            {
                PassUID_t                    const passUID  = copy.top();
                Shared<CPassBase>            const pass     = mPasses.at(passUID);
                Unique<CPassBase::CAccessor> const accessor = pass->getAccessor(CPassKey<CGraph>());

                aRenderContext.beginPass(renderContextState);
                aRenderContext.clearAttachments(renderContextState, sRenderPassResourceId);

                CEngineResult<> executed = pass->execute(aDataSource, mResourceData, renderContextState, aRenderContext);
                if(not executed.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to execute pass {}", pass->passUID()));
                    break;
                }

                if(1 < copy.size()) // Implicit last pass '0' and effective last pass --> 2 passes
                {
                    aRenderContext.endPass(renderContextState);
                }

                copy.pop();
            }

            if(EGraphMode::Graphics == mGraphMode && mRenderToBackBuffer)
            {
                aRenderContext.unbindRenderPass(renderContextState, sRenderPassResourceId, sFrameBufferResourceId);

                CEngineResult<Shared<SFrameGraphTextureView>> sourceResourceFetch = mResourceData.get<SFrameGraphTextureView>(mOutputTextureResourceId);
                if(not sourceResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture view."));
                    return {sourceResourceFetch.result()};
                }

                CEngineResult<Shared<SFrameGraphDynamicTexture>> parentResourceFetch = mResourceData.get<SFrameGraphDynamicTexture>(sourceResourceFetch.data()->subjacentResource);
                if(not parentResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture."));
                    return {sourceResourceFetch.result()};
                }

                aRenderContext.copyImageToBackBuffer(renderContextState, *(parentResourceFetch.data()));
            }

            // In any case...
            aRenderContext.endFrameCommandBuffers(renderContextState);

            if(EGraphMode::Graphics == mGraphMode)
            {
                aRenderContext.present(renderContextState);
                aRenderContext.endGraphicsFrame(renderContextState);
            }

            return { EEngineStatus::Ok };
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

                Shared<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
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
                    subjacentFetch = mResourceData.getMutable<SFrameGraphResource>(resource->subjacentResource);
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
        CEngineResult<> CGraph::bindResources(
                SFrameGraphRenderContextState  &aRenderContextState,
                SFrameGraphRenderContext       &aRenderContext,
                FrameGraphResourceIdList const &aResourceIds)
        {
            EEngineStatus binding = EEngineStatus::Ok;

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
                    binding     = aRenderContext.bindTextureView(aRenderContextState, *textureView);
                    break;
                default:
                    break;
                }

                // Break out on first error.
                if(CheckEngineError(binding))
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
                SFrameGraphRenderContextState  &aRenderContextState,
                SFrameGraphRenderContext       &aRenderContext,
                FrameGraphResourceIdList const &aResourceIds)
        {
            EEngineStatus unbinding = EEngineStatus::Ok;

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
                    unbinding   = aRenderContext.unbindTextureView(aRenderContextState, *textureView);
                    break;
                default:
                    break;
                }

                if(CheckEngineError(unbinding))
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

                Shared<SFrameGraphResource>    resource    = mResourceData.getMutable<SFrameGraphResource>(aId).data();
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
                        CEngineResult<Shared<SFrameGraphResource> const> subjacentFetch = mResourceData.get<SFrameGraphResource>(resource->subjacentResource);
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

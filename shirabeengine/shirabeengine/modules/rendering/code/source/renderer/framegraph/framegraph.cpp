#include <resources/core/resourcemanagerbase.h>
#include <graphicsapi/resources/types/all.h>

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

            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::execute(CStdSharedPtr_t<IFrameGraphRenderContext> &aRenderContext)
        {
            assert(aRenderContext != nullptr);

            static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
            static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";

            CEngineResult<> const setUpRenderPassAndFrameBuffer = initializeRenderPassAndFrameBuffer(aRenderContext, sFrameBufferResourceId, sRenderPassResourceId);
            if(not setUpRenderPassAndFrameBuffer.successful())
            {
                return setUpRenderPassAndFrameBuffer;
            }

            SFrameGraphAttachmentCollection const &attachments = mResourceData.getAttachments();


            std::stack<PassUID_t> copy = mPassExecutionOrder;
            while(!copy.empty())
            {
                PassUID_t                             const passUID  = copy.top();
                CStdSharedPtr_t<CPassBase>            const pass     = mPasses.at(passUID);
                CStdUniquePtr_t<CPassBase::CAccessor> const accessor = pass->getAccessor(CPassKey<CGraph>());

                // FrameGraphResourceIdList const &passResources = accessor->resourceReferences();

                CEngineResult<> executed = pass->execute(mResourceData, aRenderContext);
                if(not executed.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to execute pass %0", pass->passUID()));
                    return {executed};
                }

                copy.pop();

                if(not copy.empty())
                {
                    if(attachments.getAttachmentPassAssignment().end() != attachments.getAttachmentPassAssignment().find(passUID))
                    {
                        aRenderContext->nextPass();
                    }
                }
            }

            CEngineResult<> const cleanedUpRenderPassAndFrameBuffer = deinitializeRenderPassAndFrameBuffer(aRenderContext, sFrameBufferResourceId, sRenderPassResourceId);
            if(not cleanedUpRenderPassAndFrameBuffer.successful())
            {
                return cleanedUpRenderPassAndFrameBuffer;
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::initializeResources(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            CEngineResult<> initialization { EEngineStatus::Ok };
            CEngineResult<> result         { EEngineStatus::Ok };

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                CEngineResult<CStdSharedPtr_t<SFrameGraphResource>> subjacentFetch;

                CStdSharedPtr_t<SFrameGraphResource>    subjacent   = nullptr;
                CStdSharedPtr_t<SFrameGraphTexture>     texture     = nullptr;
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                FrameGraphResourceIdList::const_iterator it = mInstantiatedResources.end();

                CStdSharedPtr_t<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
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
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                std::string                               const &aFrameBufferId,
                std::string                               const &aRenderPassId)
        {
            std::vector<CStdSharedPtr_t<SFrameGraphTexture>>     textureReferences{};
            std::vector<CStdSharedPtr_t<SFrameGraphTextureView>> textureViewReferences{};

            SFrameGraphAttachmentCollection const &attachments           = mResourceData.getAttachments();
            FrameGraphResourceIdList        const &attachmentResourceIds = attachments.getAttachementResourceIds();

            // Make sure that all texture views and their subjacent textures are created upfront!
            CEngineResult<> const initialization = initializeResources(aRenderContext, attachmentResourceIds);
            if(not initialization.successful())
            {
                EngineStatusPrintOnError(initialization.result(), logTag(), "Failed to initialize resource list.");
                return initialization;
            }

            CEngineResult<> const creation = aRenderContext->createFrameBufferAndRenderPass(aFrameBufferId, aRenderPassId, attachments, mResourceData);
            EngineStatusPrintOnError(creation.result(), logTag(), "Failed to create frame buffer and/or render pass.");

            return creation;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::deinitializeRenderPassAndFrameBuffer(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                std::string                               const &aFrameBufferId,
                std::string                               const &aRenderPassId)
        {
            SFrameGraphAttachmentCollection const &attachments           = mResourceData.getAttachments();
            FrameGraphResourceIdList        const &attachmentResourceIds = attachments.getAttachementResourceIds();

            CEngineResult<> const destruction = aRenderContext->destroyFrameBufferAndRenderPass(aFrameBufferId, aRenderPassId);
            EngineStatusPrintOnError(destruction.result(), logTag(), "Failed to destroy frame buffer and/or render pass.");

            CEngineResult<> const deinitialization = deinitializeResources(aRenderContext, attachmentResourceIds);
            EngineStatusPrintOnError(deinitialization.result(), logTag(), "Failed to deinitialize resources.");

            return deinitialization;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::bindResources(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            CEngineResult<> binding = EEngineStatus::Ok;

            for(FrameGraphResourceId_t const &id : aResourceIds)
            {
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                CStdSharedPtr_t<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
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
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            CEngineResult<> unbinding = EEngineStatus::Ok;

            for(FrameGraphResourceId_t const &id : aResourceIds)
            {
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                CStdSharedPtr_t<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id).data();
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
                CStdSharedPtr_t<IFrameGraphRenderContext>      &aRenderContext,
                FrameGraphResourceIdList                  const&aResourceIds)
        {
            SHIRABE_UNUSED(aRenderContext);

            std::function<CEngineResult<>(FrameGraphResourceId_t const&, bool)> recurse = nullptr;

            recurse = [&, this] (FrameGraphResourceId_t const &aId, bool aIncludeSubjacent) -> CEngineResult<>
            {
                SHIRABE_UNUSED(aIncludeSubjacent);

                CEngineResult<> deinitialized = { EEngineStatus::Ok };

                CStdSharedPtr_t<SFrameGraphResource>    resource    = mResourceData.getMutable<SFrameGraphResource>(aId).data();
                CStdSharedPtr_t<SFrameGraphResource>    subjacent   = nullptr;
                CStdSharedPtr_t<SFrameGraphTexture>     texture     = nullptr;
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

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

                            std::remove_if(
                                        mInstantiatedResources.begin(),
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
                        CEngineResult<CStdSharedPtr_t<SFrameGraphResource> const> subjacentFetch = mResourceData.get<SFrameGraphResource>(resource->subjacentResource);
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

                        std::remove_if(
                                    mInstantiatedResources.begin(),
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
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture)
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
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture,
                CStdSharedPtr_t<SFrameGraphTextureView>   const &aTextureView)
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
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture,
                CStdSharedPtr_t<SFrameGraphTextureView>   const &aTextureView)
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
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture)
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
        CEngineResult<> CGraph::addPass(CStdSharedPtr_t<CPassBase> const &aPass)
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

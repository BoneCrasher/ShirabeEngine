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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CGraph& CGraph::operator=(CGraph const &aOther)
        {
            mPasses                  = aOther.mPasses;
            mPassAdjacency           = aOther.mPassAdjacency;
            mPassExecutionOrder      = aOther.mPassExecutionOrder;
            mResourceAdjacency       = aOther.mResourceAdjacency;
            mResourceOrder           = aOther.mResourceOrder;
            mPassToResourceAdjacency = aOther.mPassToResourceAdjacency;

            return (*this);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::execute(CStdSharedPtr_t<IFrameGraphRenderContext> &aRendercontext)
        {
            assert(aRendercontext != nullptr);

            bool const initialized = initializeResources(aRendercontext, mResources);
            bool const bound       = bindResources(aRendercontext, mResources);

            std::stack<PassUID_t> copy = mPassExecutionOrder;
            while(!copy.empty())
            {
                PassUID_t                             const passUID  = copy.top();
                CStdSharedPtr_t<CPassBase>            const pass     = mPasses.at(passUID);
                CStdUniquePtr_t<CPassBase::CAccessor> const accessor = pass->getAccessor(CPassKey<CGraph>());

                FrameGraphResourceIdList const &passResources = accessor->resourceReferences();

                // bool const initialized = initializeResources(aRendercontext, passResources);
                // bool const bound       = bindResources(aRendercontext, passResources);

                bool executed = pass->execute(mResourceData, aRendercontext);
                if(!executed) {
                    CLog::Error(logTag(), CString::format("Failed to execute pass %0", pass->passUID()));
                }

                // bool const unbound       = unbindResources(aRendercontext, passResources);
                // bool const deinitialized = deinitializeResources(aRendercontext, passResources);

                copy.pop();
            }

            bool const unbound       = unbindResources(aRendercontext, mResources);
            bool const deinitialized = deinitializeResources(aRendercontext, mResources);

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeResources(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            bool initialized = true;

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                CStdSharedPtr_t<SFrameGraphResource>    subjacent   = nullptr;
                CStdSharedPtr_t<SFrameGraphTexture>     texture     = nullptr;
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                FrameGraphResourceIdList::const_iterator it = mInstantiatedResources.end();

                CStdSharedPtr_t<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id);
                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    texture = std::static_pointer_cast<SFrameGraphTexture>(resource);

                    it = std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), texture->resourceId);
                    if(it == mInstantiatedResources.end())
                    {
                        initialized |=
                                initializeTexture(
                                    aRenderContext,
                                    texture);
                        mInstantiatedResources.push_back(texture->resourceId);
                    }

                    break;
                case EFrameGraphResourceType::TextureView:
                    subjacent   = mResourceData.get<SFrameGraphResource>(resource->subjacentResource);
                    texture     = std::static_pointer_cast<SFrameGraphTexture>(subjacent);
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);                    

                    it = std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), textureView->resourceId);
                    if(it == mInstantiatedResources.end())
                    {
                        bool const subjacentTextureCreated = (mInstantiatedResources.end() != std::find(mInstantiatedResources.begin(), mInstantiatedResources.end(), texture->resourceId));
                        if(!texture->isExternalResource && !subjacentTextureCreated)
                        {
                            initialized |= initializeResources(aRenderContext, {texture->resourceId});
                        }

                        initialized |=
                                initializeTextureView(
                                    aRenderContext,
                                    texture,
                                    textureView);
                        mInstantiatedResources.push_back(textureView->resourceId);
                    }
                    break;
                }
            }

            return initialized;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::bindResources(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            EEngineStatus status = EEngineStatus::Ok;

            bool bound = true;

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                CStdSharedPtr_t<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id);
                switch(resource->type) {
                case EFrameGraphResourceType::Texture:
                    break;
                case EFrameGraphResourceType::TextureView:
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);

                    status = aRenderContext->bindTextureView(*textureView);
                    bound |= CheckEngineError(status);
                    break;
                }
            }

            return bound;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::unbindResources(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                FrameGraphResourceIdList                  const &aResourceIds)
        {
            EEngineStatus status = EEngineStatus::Ok;

            bool unbound = true;

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                CStdSharedPtr_t<SFrameGraphResource> const resource = mResourceData.get<SFrameGraphResource>(id);
                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    break;
                case EFrameGraphResourceType::TextureView:
                    textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);

                    status = aRenderContext->unbindTextureView(*textureView);
                    unbound |= CheckEngineError(status);
                    break;
                }
            }

            return unbound;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::deinitializeResources(
                CStdSharedPtr_t<IFrameGraphRenderContext>      &aRenderContext,
                FrameGraphResourceIdList                  const&aResourceIds)
        {
            std::function<bool(FrameGraphResourceId_t const&, bool)> recurse = nullptr;

            recurse = [&, this] (FrameGraphResourceId_t const &aId, bool aIncludeSubjacent) -> bool
            {
                bool deinitialized = true;

                CStdSharedPtr_t<SFrameGraphResource>    resource    = mResourceData.getMutable<SFrameGraphResource>(aId);
                CStdSharedPtr_t<SFrameGraphResource>    subjacent   = nullptr;
                CStdSharedPtr_t<SFrameGraphTexture>     texture     = nullptr;
                CStdSharedPtr_t<SFrameGraphTextureView> textureView = nullptr;

                switch(resource->type)
                {
                case EFrameGraphResourceType::Texture:
                    if(aIncludeSubjacent)
                    {
                        texture = std::static_pointer_cast<SFrameGraphTexture>(resource);

                        if(!texture->isExternalResource && resource->referenceCount == 0)
                        {
                            deinitialized |=
                                    deinitializeTexture(
                                        aRenderContext,
                                        texture);
                            std::remove_if(
                                        mInstantiatedResources.begin(),
                                        mInstantiatedResources.end(),
                                        [&] (FrameGraphResourceId_t const &aId) -> bool
                            {
                                return (aId == texture->resourceId);
                            });
                        }
                    }
                    break;
                case EFrameGraphResourceType::TextureView:
                    // Decrease the texture view's count
                    --(resource->referenceCount);

                    std::cout << CString::format("TextureView Id %0 -> RefCount: %1\n", resource->resourceId, resource->referenceCount);

                    if(resource->referenceCount == 0)
                    {
                        subjacent   = mResourceData.get<SFrameGraphResource>(resource->subjacentResource);
                        texture     = std::static_pointer_cast<SFrameGraphTexture>(subjacent);
                        textureView = std::static_pointer_cast<SFrameGraphTextureView>(resource);

                        deinitialized |=
                                deinitializeTextureView(
                                    aRenderContext,
                                    texture,
                                    textureView);
                        std::remove_if(
                                    mInstantiatedResources.begin(),
                                    mInstantiatedResources.end(),
                                    [&] (FrameGraphResourceId_t const &aId) -> bool
                        {
                            return (aId == textureView->resourceId);
                        });

                        --(texture->referenceCount);

                        std::cout << CString::format("Texture Id %0 -> RefCount: %1\n", texture->resourceId, texture->referenceCount);

                        if(texture->referenceCount == 0)
                        {
                            deinitialized &= recurse(texture->resourceId, true);
                        }
                    }

                    break;
                }

                return deinitialized;
            };

            bool deinitialized = true;

            for(FrameGraphResourceId_t const&id : aResourceIds)
            {
                deinitialized = deinitialized && recurse(id, false);
            }

            return deinitialized;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeTexture(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture)
        {
            EEngineStatus status = EEngineStatus::Ok;

            if(aTexture->isExternalResource)
                status = aRenderContext->importTexture(*aTexture);
            else
                status = aRenderContext->createTexture(*aTexture);

            HandleEngineStatusError(status, "Failed to load texture for FrameGraphExecution.");

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::initializeTextureView(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture,
                CStdSharedPtr_t<SFrameGraphTextureView>   const &aTextureView)
        {
            bool const isForwardedOrAccepted =
                    ((aTextureView->mode.check(EFrameGraphViewAccessMode::Forward)) ||
                     (aTextureView->mode.check(EFrameGraphViewAccessMode::Accept)));
            if(isForwardedOrAccepted)
            {
                return true; // Nothing to be done and no error.
            }

            FrameGraphResourceId_t id = aTextureView->resourceId;

            EEngineStatus status = aRenderContext->createTextureView(*aTexture, *aTextureView);
            HandleEngineStatusError(status, "Failed to load texture view for FrameGraphExecution.");

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::deinitializeTextureView(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture,
                CStdSharedPtr_t<SFrameGraphTextureView>   const &aTextureView)
        {
            bool const isForwardedOrAccepted =
                    ((aTextureView->mode.check(EFrameGraphViewAccessMode::Forward)) ||
                     (aTextureView->mode.check(EFrameGraphViewAccessMode::Accept)));
            if(isForwardedOrAccepted)
            {
                return true; // Nothing to be done and no error.
            }

            EEngineStatus status = aRenderContext->destroyTextureView(*aTextureView);
            HandleEngineStatusError(status, "Failed to unload texture view for FrameGraphExecution.");

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CGraph::deinitializeTexture(
                CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture)
        {
            if(aTexture->isExternalResource)
                return true;

            // EEngineStatus status = renderContext->destroyTexture(*texture);
            // HandleEngineStatusError(status, "Failed to unload texture for FrameGraphExecution.");

            return true;
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
        bool CGraph::addPass(CStdSharedPtr_t<CPassBase> const &aPass)
        {
            if(mPasses.find(aPass->passUID()) != mPasses.end())
                return false;

            mPasses[aPass->passUID()] = aPass;

            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}

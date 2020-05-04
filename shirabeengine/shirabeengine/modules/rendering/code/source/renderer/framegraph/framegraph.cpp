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
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CGraph::execute(SFrameGraphDataSource    const &aDataSource
                                      , SFrameGraphResourceContext     &aResourceContext
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

                CEngineResult<> executed = pass->execute(aDataSource, mResourceData, renderContextState, aResourceContext, aRenderContext);
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

                CEngineResult<Shared<SFrameGraphTextureView>> const sourceResourceFetch = mResourceData.getResource<SFrameGraphTextureView>(mOutputTextureResourceId);
                if(not sourceResourceFetch.successful())
                {
                    CLog::Error(logTag(), CString::format("Failed to copy pass chain output to backbuffer. Invalid texture view."));
                    return {sourceResourceFetch.result()};
                }

                CEngineResult<Shared<SFrameGraphTexture>> const parentResourceFetch = mResourceData.getResource<SFrameGraphTexture>(sourceResourceFetch.data()
                                                                                                                                                       ->subjacentResource);
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

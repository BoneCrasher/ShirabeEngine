#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/passkey.h>
#include <core/datastructures/adjacencytree.h>
#include "renderer/framegraph/framegraphserialization.h"
#include "renderer/framegraph/renderpass.h"
#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/framegraphcontexts.h"

namespace engine
{
    namespace framegraph
    {
        using datastructures::CAdjacencyTree;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        using namespace serialization;
#endif

        /**
         * A CGraph describes the hierarchical structure of the framegraph,
         * i.e. its inputs, outputs, passes, resources and all operations performed.
         */
        class SHIRABE_TEST_EXPORT CGraph
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                : public ISerializable<IRenderGraphSerializer, IRenderGraphDeserializer>
#endif
        {
            SHIRABE_DECLARE_LOG_TAG(CGraph)

        public_enums:
            /**
             * The EGraphMode enum determines resource life cycle management
             * behaviour for graphics and compute pipelines.
             *
             * E.g., whether to create a render pass and frame buffer or not.
             */
            enum class EGraphMode
            {
                Graphics,
                Compute
            };

        public_classes:

        private_static_fields:
            static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
            static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";
            static constexpr char const *sSwapChainResourceId   = "BackBuffer";

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            /**
             * Getter method for the graphvizserializer class to fetch an immutable accessor class.
             *
             * @param aKey PassKey creatable only by graphvizserializer instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            Unique<CAccessor> getAccessor(CPassKey<CRenderGraphGraphVizSerializer> &&aKey) const
            {
                return std::move(std::make_unique<CAccessor>(this));
            }

            /**
             * Getter method for the graphvizserializer class to fetch a mutable accessor class.
             *
             * @param aKey PassKey creatable only by graphvizserializer instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            Unique<CMutableAccessor> getMutableAccessor(CPassKey<CRenderGraphGraphVizSerializer> &&aKey)
            {
                return std::move(std::make_unique<CMutableAccessor>(this));
            }
#endif

        public_methods:
            /**
             * Execute the framegraph, causing it create the respective command buffers.
             *
             * @return True, if successfully executed. False otherwise.
             */
            CEngineResult<> execute(SRenderGraphDataSource      const &aDataSource
                                    , SRenderGraphResourceContext     &aResourceContext
                                    , SRenderGraphRenderContext       &aRenderContext);

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            /**
             * Double-Dispatch accept for the graph to accept any kind of frame graph serializer instance.
             *
             * @param aSerializer The serializer instance to accept.
             * @return            True, if successful. False otherwise.
             */
            virtual bool acceptSerializer(IRenderGraphSerializer &aSerializer) const;

            /**
             * Double-Dispatch accept for the graph to accept any kind of frame graph deserializer instance.
             *
             * @param aDeserializer The deserializer instance to accept.
             * @return            True, if successful. False otherwise.
             */
            virtual bool acceptDeserializer(IRenderGraphDeserializer &aDeserializer);
#endif

            /**
             * Assign another graph to this instance.
             *
             * @param aOther The other graph to assign.
             * @return       See brief.
             */
            SHIRABE_INLINE CGraph &operator=(CGraph const &aOther);

        private_methods:
            bool initializeAttachments(SRenderGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass);
            bool deinitializeAttachments(SRenderGraphResourceContext &aResourceContext, Shared<CRenderPass> aRenderPass);

            bool initializeSubpassResources(SRenderGraphResourceContext &aResourceContext, Shared<CPassBase> aSubpass);
            bool deinitializeSubpassResources(SRenderGraphResourceContext &aResourceContext, Shared<CPassBase> aSubpass);

        public_methods:
            PassMap const &subpasses() const { return mSubpasses; }
            PassMap       &subpasses()       { return mSubpasses; }

            RenderPassMap const &renderPasses() const { return mRenderPasses; }
            RenderPassMap       &renderPasses()       { return mRenderPasses; }

            std::vector<PassUID_t>      const &renderpassExecutionOrder() const  { return mRenderpassExecutionOrder; }
            RenderGraphResourceIdList    const &resources()                const  { return mResources; }
            CRenderGraphMutableResources       &resourceData()                    { return mResourceData; }

            EGraphMode             const &graphMode()                const { return mGraphMode; }
            bool                   const &renderToBackBuffer()       const { return mRenderToBackBuffer; };
            RenderGraphResourceId_t const &outputTextureResourceId()  const { return mOutputTextureResourceId; }

            /**
             * Create a new pass of type TPass with TPassCreationArgs and set it up.
             *
             * @param aUID  The UID of the pass to create.
             * @param aName The name of the pass to create.
             * @param aArgs The arguments reuqired to create the pass.
             * @return      A pointer to the newly created pass or nullptr.
             */
            template
            <
                typename    TPass,
                typename... TPassCreationArgs
            >
            CEngineResult<Shared<TPass>> createPass(
                    PassUID_t         const &aUID,
                    std::string       const &aName,
                    TPassCreationArgs       &&...aArgs);

        private_members:
            friend class CGraphBuilder;

            PassMap                      mSubpasses;
            RenderPassMap                mRenderPasses;
            std::vector<RenderPassUID_t> mRenderpassExecutionOrder;

            RenderGraphResourceIdList    mResources;
            CRenderGraphMutableResources mResourceData;
            RenderGraphResourceIdList    mInstantiatedResources;

            EGraphMode                  mGraphMode;
            bool                        mRenderToBackBuffer;
            RenderGraphResourceId_t      mOutputTextureResourceId;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            AdjacencyListMap_t<RenderGraphResourceId_t>            mResourceAdjacency;
            std::stack<RenderGraphResourceId_t>                    mResourceOrder;
            AdjacencyListMap_t<PassUID_t, RenderGraphResourceId_t> mPassToResourceAdjacency;
#endif

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename    TPass,
                typename... TPassCreationArgs
                >
        CEngineResult<Shared<TPass>> CGraph::createPass(
                PassUID_t         const &aUID,
                std::string       const &aName,
                TPassCreationArgs       &&...aArgs)
        {
            Shared<TPass> pass = makeShared<TPass>(aUID, aName, std::forward<TPassCreationArgs>(aArgs)...);
            if(!pass)
            {
                CLog::WTF(logTag(), "Failed to create pass.");
                return { EEngineStatus::Error };
            }

            return { EEngineStatus::Ok, pass };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif

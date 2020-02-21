#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/passkey.h>
#include "renderer/framegraph/framegraphserialization.h"
#include "renderer/framegraph/pass.h"
#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine
{
    namespace framegraph
    {
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
        using namespace serialization;
#endif

        /**
         * A CGraph describes the hierarchical structure of the framegraph,
         * i.e. its inputs, outputs, passes, resources and all operations performed.
         */
        class SHIRABE_TEST_EXPORT CGraph
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
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
            /**
             * The CAccessor class immutably provides all data members of the graph.
             */
            class SHIRABE_TEST_EXPORT CAccessor
            {
            public_constructors:
                /**
                 * Default-Construct an immutable accessor from aGraph.
                 *
                 * @param aGraph
                 */
                CAccessor(CGraph const *aGraph);

            public_methods:
                AdjacencyListMap_t<PassUID_t>                         const &passAdjacency()           const;
                std::stack<PassUID_t>                                 const &passExecutionOrder()      const;
                FrameGraphResourceIdList                              const &resources()               const;
                CFrameGraphMutableResources                           const &resourceData()            const;
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                AdjacencyListMap_t<FrameGraphResourceId_t>            const &resourceAdjacency()       const;
                std::stack<FrameGraphResourceId_t>                    const &resourceOrder()           const;
                AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> const &passToResourceAdjacency() const;
#endif
                EGraphMode                                                   graphMode()               const;
                bool                                                         renderToBackBuffer()      const;
                FrameGraphResourceId_t                                const &outputTextureResourceId() const;

            private_members:
                CGraph const *m_graph;
            };

            /**
             * The CMutableAccessor class extends the CAccessor and mutably provides all data members of the graph.
             */
            class SHIRABE_TEST_EXPORT CMutableAccessor
                    : public CAccessor
            {
            public_constructors:
                /**
                 * Default-Construct an mutable accessor from aGraph.
                 *
                 * @param aGraph
                 */
                CMutableAccessor(CGraph *aGraph);

            public_methods:
                AdjacencyListMap_t<PassUID_t> &mutablePassAdjacency();
                std::stack<PassUID_t>         &mutablePassExecutionOrder();
                FrameGraphResourceIdList      &mutableResources();
                CFrameGraphMutableResources   &mutableResourceData();
#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
                AdjacencyListMap_t<FrameGraphResourceId_t>            &mutableResourceAdjacency();
                std::stack<FrameGraphResourceId_t>                    &mutableResourceOrder();
                AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> &mutablePassToResourceAdjacency();
#endif                
                EGraphMode                    &mutableGraphMode();
                bool                          &mutableRenderToBackBuffer();
                FrameGraphResourceId_t        &mutableOutputTextureResourceId();

                /**
                 * Create a new pass of type TPass given a uid and name.
                 * This will implicitly setup the pass.
                 *
                 * @tparam TPass             The type of the pass to create.
                 * @tparam TPassCreationArgs The arguments required to create the specific pass.
                 */
                template <
                        typename    TPass,
                        typename... TPassCreationArgs
                        >
                CEngineResult<Shared<TPass>> createPass(
                        PassUID_t         const      &uid,
                        std::string       const      &name,
                        TPassCreationArgs       &&...args);

            private_members:
                CGraph *mGraph;
            };

        private_static_fields:
            static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
            static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";
            static constexpr char const *sSwapChainResourceId   = "BackBuffer";

        public_methods:
            /**
             * Getter method for the graph builder class to fetch an immutable accessor class.
             *
             * @param aKey PassKey creatable only by GraphBuilder instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            Unique<CAccessor> getAccessor(CPassKey<class CGraphBuilder> &&aKey) const
            {
                SHIRABE_UNUSED(aKey);

                return std::make_unique<CAccessor>(this);
            }

            /**
             * Getter method for the graph builder class to fetch a mutable accessor class.
             *
             * @param aKey PassKey creatable only by GraphBuilder instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            Unique<CMutableAccessor> getMutableAccessor(CPassKey<class CGraphBuilder> &&aKey)
            {
                SHIRABE_UNUSED(aKey);

                return std::make_unique<CMutableAccessor>(this);
            }

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            /**
             * Getter method for the graphvizserializer class to fetch an immutable accessor class.
             *
             * @param aKey PassKey creatable only by graphvizserializer instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            Unique<CAccessor> getAccessor(CPassKey<CFrameGraphGraphVizSerializer> &&aKey) const
            {
                return std::move(std::make_unique<CAccessor>(this));
            }

            /**
             * Getter method for the graphvizserializer class to fetch a mutable accessor class.
             *
             * @param aKey PassKey creatable only by graphvizserializer instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            Unique<CMutableAccessor> getMutableAccessor(CPassKey<CFrameGraphGraphVizSerializer> &&aKey)
            {
                return std::move(std::make_unique<CMutableAccessor>(this));
            }
#endif

            /**
             * Execute the framegraph, causing it create the respective command buffers.
             *
             * @return True, if successfully executed. False otherwise.
             */
            CEngineResult<> execute(SFrameGraphRenderContext &aRenderContext);

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            /**
             * Double-Dispatch accept for the graph to accept any kind of frame graph serializer instance.
             *
             * @param aSerializer The serializer instance to accept.
             * @return            True, if successful. False otherwise.
             */
            virtual bool acceptSerializer(IFrameGraphSerializer &aSerializer) const;

            /**
             * Double-Dispatch accept for the graph to accept any kind of frame graph deserializer instance.
             *
             * @param aDeserializer The deserializer instance to accept.
             * @return            True, if successful. False otherwise.
             */
            virtual bool acceptDeserializer(IFrameGraphDeserializer &aDeserializer);
#endif

            /**
             * Assign another graph to this instance.
             *
             * @param aOther The other graph to assign.
             * @return       See brief.
             */
            SHIRABE_INLINE CGraph &operator=(CGraph const &aOther);

            /**
             * Return all passes currently attached to the framegraph.
             *
             * @return See brief.
             */
            PassMap const&passes() const;

        private_methods:
            /**
             * Initialize all resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> initializeResources(
                    SFrameGraphRenderContext       &aRenderContext,
                    FrameGraphResourceIdList const &aResourceIds);

            /**
             * Initialize all subpasses, the render pass and the framebuffer.
             *
             * @param aRenderContext The render context interface to the graphics API.
             * @param aRenderPassId  Unique Id of the render pass to initialize.
             * @param aFrameBufferId Unique Id of the frame buffer to initialize.
             * @return               True, if successful. False, otherwise.
             */
            CEngineResult<> initializeRenderPassAndFrameBuffer(
                    SFrameGraphRenderContext       &aRenderContext,
                    std::vector<PassUID_t>   const &aPassExecutionOrder,
                    std::string              const &aRenderPassId,
                    std::string              const &aFrameBufferId);

            /**
             * Bind all automatic resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> bindResources(
                    SFrameGraphRenderContext       &aRenderContext,
                    FrameGraphResourceIdList const &aResourceIds);

            /**
             * Unbind all automatic resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> unbindResources(
                    SFrameGraphRenderContext       &aRenderContext,
                    FrameGraphResourceIdList const &aResourceIds);

            /**
             * Deinitialize all subpasses, the render pass and the framebuffer.
             *
             * @param aRenderContext The render context interface to the graphics API.
             * @param aRenderPassId  Unique Id of the render pass to deinitialize.
             * @param aFrameBufferId Unique Id of the frame buffer to deinitialize.
             * @return               True, if successful. False, otherwise.
             */
            CEngineResult<> deinitializeRenderPassAndFrameBuffer(
                    SFrameGraphRenderContext       &aRenderContext,
                    std::string              const &aRenderPassId,
                    std::string              const &aFrameBufferId);

            /**
             * Deinitialize all resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> deinitializeResources(
                    SFrameGraphRenderContext       &aRenderContext,
                    FrameGraphResourceIdList const &aResourceIds);

            /**
             * Initialize a texture for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> initializeTexture(
                    SFrameGraphRenderContext         &aRenderContext,
                    Shared<SFrameGraphDynamicTexture> const &aTexture);

            /**
             * Initialize a texture view for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture of the view to create.
             * @param aTextureView  The texture view to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> initializeTextureView(
                    SFrameGraphRenderContext             &aRenderContext,
                    Shared<SFrameGraphDynamicTexture>     const &aTexture,
                    Shared<SFrameGraphTextureView> const &aTextureView);
            /**
             * Initialize a buffer for execution.
             *
             * @param renderContext
             * @return
             */
            CEngineResult<> initializeBuffer(SFrameGraphRenderContext &aRenderContext);

            /**
             * Initialize a buffer view for execution
             * @param renderContext
             * @return
             */
            CEngineResult<> initializeBufferView(SFrameGraphRenderContext &aRenderContext);

            /**
             * Deinitialize a texture for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture to destroy.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> deinitializeTexture(
                    SFrameGraphRenderContext         &aRenderContext,
                    Shared<SFrameGraphDynamicTexture> const &aTexture);

            /**
             * Deinitialize a texture view for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture of the view to destroy.
             * @param aTextureView  The texture view to destroy.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> deinitializeTextureView(
                    SFrameGraphRenderContext             &aRenderContext,
                    Shared<SFrameGraphDynamicTexture>     const &aTexture,
                    Shared<SFrameGraphTextureView> const &aTextureView);

            /**
             * Deinitialize a buffer view for execution.
             *
             * @param renderContext
             * @return
             */
            CEngineResult<> deinitializeBufferView(SFrameGraphRenderContext &aRenderContext);
            /**
             * Deinitialize a buffer view for execution.
             *
             * @param renderContext
             * @return
             */
            CEngineResult<> deinitializeBuffer(SFrameGraphRenderContext &aRenderContext);

            /**
             * Add a new pass to this graph.
             *
             * @param aPass The pass to add.
             * @return      True, if successfully added. False otherwise.
             */
            CEngineResult<> addPass(Shared<CPassBase> const &aPass);

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
            Shared<CResourceManager> mResourceManager;

            PassMap                               mPasses;
            AdjacencyListMap_t<PassUID_t>         mPassAdjacency;
            std::stack<PassUID_t>                 mPassExecutionOrder;
            FrameGraphResourceIdList              mResources;
            CFrameGraphMutableResources           mResourceData;
            FrameGraphResourceIdList              mInstantiatedResources;

            EGraphMode                            mGraphMode;
            bool                                  mRenderToBackBuffer;
            FrameGraphResourceId_t                mOutputTextureResourceId;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            AdjacencyListMap_t<FrameGraphResourceId_t>            mResourceAdjacency;
            std::stack<FrameGraphResourceId_t>                    mResourceOrder;
            AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> mPassToResourceAdjacency;
#endif

        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename   TPass,
                typename... TPassCreationArgs
                >
        CEngineResult<Shared<TPass>> CGraph::CMutableAccessor::createPass(
                PassUID_t         const &aUID,
                std::string       const &aName,
                TPassCreationArgs       &&...aArgs)
        {
            CEngineResult<Shared<TPass>> pass = mGraph->createPass<TPass, TPassCreationArgs...>(aUID, aName, std::forward<TPassCreationArgs>(aArgs)...);
            return pass;
        }
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

            this->addPass(pass);
            return { EEngineStatus::Ok, pass };
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif

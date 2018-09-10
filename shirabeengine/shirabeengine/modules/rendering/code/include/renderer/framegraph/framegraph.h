#ifndef __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__
#define __SHIRABE_FRAMEGRAPH_FRAMEGRAPH_H__

#include <string>
#include <functional>
#include <stack>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/passkey.h>
#include <resources/core/resourcedomaintransfer.h>
#include "renderer/framegraph/framegraphserialization.h"
#include "renderer/framegraph/pass.h"
#include "renderer/framegraph/framegraphdata.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine
{
    namespace framegraph
    {
        using namespace serialization;

        /**
         * A CGraph describes the hierarchical structure of the framegraph,
         * i.e. its inputs, outputs, passes, resources and all operations performed.
         */
        class SHIRABE_TEST_EXPORT CGraph
                : public ISerializable<IFrameGraphSerializer, IFrameGraphDeserializer>
        {
            SHIRABE_DECLARE_LOG_TAG(CGraph);

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
                AdjacencyListMap_t<FrameGraphResourceId_t>            const &resourceAdjacency()       const;
                std::stack<FrameGraphResourceId_t>                    const &resourceOrder()           const;
                AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> const &passToResourceAdjacency() const;

            private_members:
                CGraph const*m_graph;
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
                AdjacencyListMap_t<PassUID_t>                         &mutablePassAdjacency();
                std::stack<PassUID_t>                                 &mutablePassExecutionOrder();
                FrameGraphResourceIdList                              &mutableResources();
                CFrameGraphMutableResources                           &mutableResourceData();
                AdjacencyListMap_t<FrameGraphResourceId_t>            &mutableResourceAdjacency();
                std::stack<FrameGraphResourceId_t>                    &mutableResourceOrder();
                AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> &mutablePassToResourceAdjacency();

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
                CStdSharedPtr_t<TPass> createPass(
                        PassUID_t         const &uid,
                        std::string       const &name,
                        TPassCreationArgs       &&...args);

            private_members:
                CGraph *mGraph;
            };

        public_methods:
            /**
             * Getter method for the graph builder class to fetch an immutable accessor class.
             *
             * @param aKey PassKey creatable only by GraphBuilder instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            CStdUniquePtr_t<CAccessor> getAccessor(CPassKey<class CGraphBuilder> &&aKey) const
            {
                return std::move(std::make_unique<CAccessor>(this));
            }

            /**
             * Getter method for the graph builder class to fetch a mutable accessor class.
             *
             * @param aKey PassKey creatable only by GraphBuilder instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            CStdUniquePtr_t<CMutableAccessor> getMutableAccessor(CPassKey<class CGraphBuilder> &&aKey)
            {
                return std::move(std::make_unique<CMutableAccessor>(this));
            }

            /**
             * Getter method for the graphvizserializer class to fetch an immutable accessor class.
             *
             * @param aKey PassKey creatable only by graphvizserializer instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            CStdUniquePtr_t<CAccessor> getAccessor(CPassKey<CFrameGraphGraphVizSerializer> &&aKey) const
            {
                return std::move(std::make_unique<CAccessor>(this));
            }

            /**
             * Getter method for the graphvizserializer class to fetch a mutable accessor class.
             *
             * @param aKey PassKey creatable only by graphvizserializer instances.
             * @return     A CAccessor instance to access the graph's data.
             */
            CStdUniquePtr_t<CMutableAccessor> getMutableAccessor(CPassKey<CFrameGraphGraphVizSerializer> &&aKey)
            {
                return std::move(std::make_unique<CMutableAccessor>(this));
            }

            /**
             * Execute the framegraph, causing it create the respective command buffers.
             *
             * @return True, if successfully executed. False otherwise.
             */
            bool execute(CStdSharedPtr_t<IFrameGraphRenderContext>&);

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
            bool initializeResources(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    FrameGraphResourceIdList                  const &aResourceIds);

            /**
             * Bind all automatic resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            bool bindResources(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    FrameGraphResourceIdList                  const &aResourceIds);

            /**
             * Unbind all automatic resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            bool unbindResources(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    FrameGraphResourceIdList                  const &aResourceIds);

            /**
             * Deinitialize all resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            bool deinitializeResources(
                    CStdSharedPtr_t<IFrameGraphRenderContext>      &aRenderContext,
                    FrameGraphResourceIdList                  const&aResourceIds);

            /**
             * Initialize a texture for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture to create.
             * @return              True, if sucessful. False otherwise.
             */
            bool initializeTexture(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture);

            /**
             * Initialize a texture view for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture of the view to create.
             * @param aTextureView  The texture view to create.
             * @return              True, if sucessful. False otherwise.
             */
            bool initializeTextureView(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture,
                    CStdSharedPtr_t<SFrameGraphTextureView>   const &aTextureView);
            /**
             * Initialize a buffer for execution.
             *
             * @param renderContext
             * @return
             */
            bool initializeBuffer(
                    CStdSharedPtr_t<IFrameGraphRenderContext> &aRenderContext);

            /**
             * Initialize a buffer view for execution
             * @param renderContext
             * @return
             */
            bool initializeBufferView(
                    CStdSharedPtr_t<IFrameGraphRenderContext> &aRenderContext);

            /**
             * Deinitialize a texture for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture to destroy.
             * @return              True, if sucessful. False otherwise.
             */
            bool deinitializeTexture(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture);

            /**
             * Deinitialize a texture view for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture of the view to destroy.
             * @param aTextureView  The texture view to destroy.
             * @return              True, if sucessful. False otherwise.
             */
            bool deinitializeTextureView(
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aRenderContext,
                    CStdSharedPtr_t<SFrameGraphTexture>       const &aTexture,
                    CStdSharedPtr_t<SFrameGraphTextureView>   const &aTextureView);

            /**
             * Deinitialize a buffer view for execution.
             *
             * @param renderContext
             * @return
             */
            bool deinitializeBufferView(
                    CStdSharedPtr_t<IFrameGraphRenderContext> &aRenderContext);
            /**
             * Deinitialize a buffer view for execution.
             *
             * @param renderContext
             * @return
             */
            bool deinitializeBuffer(
                    CStdSharedPtr_t<IFrameGraphRenderContext> &aRenderContext);

            /**
             * Add a new pass to this graph.
             *
             * @param aPass The pass to add.
             * @return      True, if successfully added. False otherwise.
             */
            bool addPass(CStdSharedPtr_t<CPassBase> const &aPass);

            /**
             * Create a new pass of type TPass with TPassCreationArgs and set it up.
             *
             * @param aUID  The UID of the pass to create.
             * @param aName The name of the pass to create.
             * @param aArgs The arguments reuqired to create the pass.
             * @return      A pointer to the newly created pass or nullptr.
             */
            template <
                    typename    TPass,
                    typename... TPassCreationArgs
                    >
            CStdSharedPtr_t<TPass> createPass(
                    PassUID_t         const &aUID,
                    std::string       const &aName,
                    TPassCreationArgs       &&...aArgs);

        private_members:
            CStdSharedPtr_t<CResourceManagerBase>                 mResourceManager;

            PassMap                                               mPasses;
            AdjacencyListMap_t<PassUID_t>                         mPassAdjacency;
            std::stack<PassUID_t>                                 mPassExecutionOrder;
            FrameGraphResourceIdList                              mResources;
            CFrameGraphMutableResources                           mResourceData;
            AdjacencyListMap_t<FrameGraphResourceId_t>            mResourceAdjacency;
            std::stack<FrameGraphResourceId_t>                    mResourceOrder;
            AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t> mPassToResourceAdjacency;
            FrameGraphResourceIdList                              mInstantiatedResources;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename   TPass,
                typename... TPassCreationArgs
                >
        CStdSharedPtr_t<TPass> CGraph::CMutableAccessor::createPass(
                PassUID_t         const &aUID,
                std::string       const &aName,
                TPassCreationArgs       &&...aArgs)
        {
            CStdSharedPtr_t<TPass> pass = mGraph->createPass<TPass, TPassCreationArgs...>(aUID, aName, std::forward<TPassCreationArgs>(aArgs)...);
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
        CStdSharedPtr_t<TPass>
        CGraph::createPass(
                PassUID_t         const &aUID,
                std::string       const &aName,
                TPassCreationArgs       &&...aArgs)
        {
            CStdSharedPtr_t<TPass> pass = makeCStdSharedPtr<TPass>(aUID, aName, std::forward<TPassCreationArgs>(aArgs)...);
            if(!pass)
            {
                CLog::WTF(logTag(), "Failed to create pass.");
                return nullptr;
            }

            this->addPass(pass);
            return pass;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif

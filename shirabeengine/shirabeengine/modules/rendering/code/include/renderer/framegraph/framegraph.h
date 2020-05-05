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
#include "renderer/framegraph/framegraphcontexts.h"

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
             *
             * @param aRenderContextState
             * @param aRenderContext
             * @param aPassExecutionOrder
             * @return
             */
            CEngineResult<> initializeGraphResources(SFrameGraphResourceContext &aResourceContext);

            /**
             *
             * @param aRenderContextState
             * @param aRenderContext
             * @return
             */
            CEngineResult<> deinitializeGraphResources(SFrameGraphResourceContext &aResourceContext);

            /**
             * Execute the framegraph, causing it create the respective command buffers.
             *
             * @return True, if successfully executed. False otherwise.
             */
            CEngineResult<> execute(SFrameGraphDataSource      const &aDataSource
                                    , SFrameGraphResourceContext     &aResourceContext
                                    , SFrameGraphRenderContext       &aRenderContext);

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

        private_methods:
            /**
             * Initialize all resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> initializeResources(
                    SFrameGraphResourceContext     &aResourceContext,
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
                    SFrameGraphResourceContext     &aResourceContext,
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
                    SFrameGraphResourceContext     &aResourceContext,
                    FrameGraphResourceIdList const &aResourceIds);

            /**
             * Unbind all automatic resources required for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param resourceIds   The list of ids of resources to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> unbindResources(
                    SFrameGraphResourceContext     &aResourceContext,
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
                    SFrameGraphResourceContext     &aResourceContext,
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
                    SFrameGraphResourceContext     &aResourceContext,
                    FrameGraphResourceIdList const &aResourceIds);

            /**
             * Initialize a texture for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> initializeTexture(
                    SFrameGraphResourceContext              &aResourceContext,
                    Shared<SFrameGraphTexture> const &aTexture);

            /**
             * Initialize a texture view for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture of the view to create.
             * @param aTextureView  The texture view to create.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> initializeTextureView(
                SFrameGraphResourceContext              &aResourceContext,
                    Shared<SFrameGraphTexture> const &aTexture,
                    Shared<SFrameGraphTextureView>    const &aTextureView);
            /**
             * Initialize a buffer for execution.
             *
             * @param renderContext
             * @return
             */
            CEngineResult<> initializeBuffer(SFrameGraphResourceContext &aResourceContext);

            /**
             * Initialize a buffer view for execution
             * @param renderContext
             * @return
             */
            CEngineResult<> initializeBufferView(SFrameGraphResourceContext &aResourceContext);

            /**
             * Deinitialize a texture for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture to destroy.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> deinitializeTexture(
                    SFrameGraphResourceContext              &aResourceContext,
                    Shared<SFrameGraphTexture> const &aTexture);

            /**
             * Deinitialize a texture view for execution.
             *
             * @param renderContext The render context interfacing to the graphics API.
             * @param aTexture      The texture of the view to destroy.
             * @param aTextureView  The texture view to destroy.
             * @return              True, if sucessful. False otherwise.
             */
            CEngineResult<> deinitializeTextureView(
                SFrameGraphResourceContext              &aResourceContext,
                    Shared<SFrameGraphTexture> const &aTexture,
                    Shared<SFrameGraphTextureView>    const &aTextureView);

            /**
             * Deinitialize a buffer view for execution.
             *
             * @param renderContext
             * @return
             */
            CEngineResult<> deinitializeBufferView(SFrameGraphResourceContext &aResourceContext);
            /**
             * Deinitialize a buffer view for execution.
             *
             * @param renderContext
             * @return
             */
            CEngineResult<> deinitializeBuffer(SFrameGraphResourceContext &aResourceContext);

        public_methods:

            PassMap const &passes() const { return mPasses; }
            PassMap       &passes()       { return mPasses; }

            AdjacencyListMap_t<PassUID_t> const &passAdjacency() const  { return mPassAdjacency; }
            AdjacencyListMap_t<PassUID_t>       &mutablePassAdjacency() { return mPassAdjacency; }

            std::stack<PassUID_t> const &passExecutionOrder() const  { return mPassExecutionOrder; }
            std::stack<PassUID_t>       &mutablePassExecutionOrder() { return mPassExecutionOrder; }

            FrameGraphResourceIdList const &resources() const  { return mResources; }
            FrameGraphResourceIdList       &mutableResources() { return mResources; }

            CFrameGraphMutableResources &resourceData() { return mResourceData; }

            EGraphMode const &graphMode()  const { return mGraphMode; }
            EGraphMode       &mutableGraphMode() { return mGraphMode; }
            bool const &renderToBackBuffer()   const { return mRenderToBackBuffer; };
            bool        &mutableRenderToBackBuffer() { return mRenderToBackBuffer; };
            FrameGraphResourceId_t const &outputTextureResourceId()  const { return mOutputTextureResourceId; }
            FrameGraphResourceId_t       &mutableOutputTextureResourceId() { return mOutputTextureResourceId; }

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

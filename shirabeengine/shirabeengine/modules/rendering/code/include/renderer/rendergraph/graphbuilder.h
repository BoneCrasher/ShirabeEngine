#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <stack>
#include <utility>

#include <log/log.h>
#include <core/enginetypehelper.h>
#include <core/datastructures/adjacencytree.h>
#include <core/random.h>
#include <core/uid.h>
#include <wsi/display.h>
#include "renderer/rendergraph/rendergraph.h"
#include "renderer/rendergraph/renderpass.h"

namespace engine
{
    namespace framegraph
    {

        using namespace engine::core;
        using namespace engine::resources;

        /**
         * The CGraphBuilder is contains and collects all passes, their data and relations and
         * implements graph compilation, yielding an executable render graph.
         */
        class SHIRABE_TEST_EXPORT CGraphBuilder
        {
            SHIRABE_DECLARE_LOG_TAG(CGraphBuilder);

        public_constructors:
            /**
             * Default-Construct an empty graph builder.
             */
            CGraphBuilder(CResourceManager const &aResourceManager);

        public_destructors:
            /**
             * Destroy and run...
             */
            ~CGraphBuilder() = default;

        public_methods:
            /**
             * Initialize the graph builder, which will setup all prerequisites and spawn a
             * pseudo-pass to which all non-pass related imports and exports are attached.
             *
             * @param aApplicationEnvironment The application environment to source from within the
             *                                graph builder and pass setup routines.
             * @param aDisplay                WSI display to bind against the current hardware monitor config.
             * @return                        True, if successful. False otherwise.
             */
            CEngineResult<> initialize(
                    Shared<os::SApplicationEnvironment> const &aApplicationEnvironment,
                    Shared<wsi::CWSIDisplay>            const &aDisplay);

            /**
             * Clear all state and shutdown...
             *
             * @return True, if successful. False otherwise.
             */
            CEngineResult<> deinitialize();

            /**
             * Return all current resources stored in the builder.
             *
             * @return See brief.
             *
             */
            SHIRABE_INLINE CRenderGraphResources const &getResources() const
            {
                return mResourceData;
            }

            /**
             * Set the graph mode used for resource management configuration.
             *
             * @param aMode The requested mode to use.
             */
            SHIRABE_INLINE
            void setGraphMode(framegraph::CGraph::EGraphMode const &aMode)
            {
                if(framegraph::CGraph::EGraphMode::Graphics != aMode && mRenderToBackBuffer)
                {
                    return; // render to backbuffer implies graphics mode.
                }

                mGraphMode = aMode;
            }

            SHIRABE_INLINE
            void setRenderToBackBuffer(bool const &aRenderToBackBuffer)
            {
                mRenderToBackBuffer = aRenderToBackBuffer;
                setGraphMode(framegraph::CGraph::EGraphMode::Graphics);
            }

            SHIRABE_INLINE
            void setOutputTextureResourceId(RenderGraphResourceId_t const &aOutputResourceId)
            {
                mOutputResourceId = aOutputResourceId;
            }

            /**
             * Spawn and register a new pass of type TPass to this graph builder using
             * the TPassCreationArgs as creation input.
             * This will implicitly invoke "setup" on the pass.
             *
             * @tparam TPass             The type of pass to spawn.
             * @tparam TPassCreationArgs The argument types required for pass creation.
             * @param aName              The name of the pass.
             * @param aArgs              The arguments required for pass creation.
             * @return                   Returns a pointer to the newly created pass or nullptr on error.
             */
            template <
                    typename    TPass,
                    typename... TPassCreationArgs
                    >
            CEngineResult<Shared<TPass>> addSubpass(std::string       const &aName
                                                  , TPassCreationArgs  &&... aArgs);

            /**
             * Begin a render pass, to which all subsequent calls of addSubpass will attach to.
             *
             * @param aId
             * @return
             */
            CEngineResult<Shared<CRenderPass>> beginRenderPass(std::string const &aName);

            /**
             * End and commit the currently render pass under construction.
             *
             * @return
             */
            CEngineResult<> endRenderPass();

            /**
             * Compile the graph, collecting all passes, resources and relationship,
             * performing various steps like a depth-first sorting to determine pass
             * execution order as well as culling to reduce and optimize resource
             * redundancy and usage.
             *
             * @return A pointer to a compiled and executable CGraph instance.
             */
            CEngineResult<Unique<CGraph>> compile();

        private_methods:
            friend class CRenderPass;

            /**
             * Create a ordered dependency from aPassTarget on aPassSource, to enforce execution of source before target.
             *
             * @param aPassSource Name of pass to depend on.
             * @param aPassTarget Dependent pass name.
             */
            CEngineResult<> createRenderPassDependency(std::string const &aPassSource
                                                     , std::string const &aPassTarget);

            /**
             * Generate a new PassUID.
             *
             * @return See brief.
             */
            RenderPassUID_t generateRenderPassUID();

            /**
             * Generate a new PassUID.
             *
             * @return See brief.
             */
            PassUID_t generateSubpassUID();

            /**
             * Generate a new PassUID.
             *
             * @return See brief.
             */
            RenderGraphResourceId_t generateResourceUID();

            /**
             * Return the current internal graph state.
             *
             * @return See brief.
             */
            Unique<CGraph> &graph();

            /**
             * Create a ordered dependency from aPassTarget on aPassSource, to enforce execution of source before target.
             *
             * @param aPassSource UID of the Pass to depend on.
             * @param aPassTarget Dependent pass UID.
             */
            CEngineResult<> createRenderPassDependencyByUID(
                     RenderPassUID_t const &aPassSource,
                     RenderPassUID_t const &aPassTarget);

            /**
             * Accepts an arbitrary resource and tries to traverse the resource-resourceview-path to find
             * the subjacent resource.
             *
             * @param aResourceMap          A map of resources containing all resources in the path from
             *                              aResourceToSearchFrom and the subjacent resource.
             * @param aResourceToSearchFrom Starting point of the path towards the subjacent resource.
             * @return                      The RenderGraphResourceId_t of the subjacent resource or 0 if not found.
EST_EXPORT CGraphBuilder
        {*/
            CEngineResult<RenderGraphResourceId_t> findSubjacentResource(
                    SRenderGraphResourceMap const &aResourceMap,
                    SRenderGraphResource    const &aResourceToSearchFrom);

            /**
             * Collect a fully set-up pass, performing quite a lot of validation and resource work.
             *
             * @param aPass The pass to collect.
             * @return      True, if successful. False, otherwise.
             */
            CEngineResult<> collectRenderPass(Shared<CRenderPass> aRenderPass);

            /**
             * Validate the pass order and data flow dependencies of a sorted
             * list of passes.
             *
             * @param aPassOrder The stack containing the pass order to be validated.
             * @return           True, if valid. False otherwise.
             */
            CEngineResult<> validate();

            /**
             * Validate, whether the textureview tries to access the subjacent texture in
             * correct bounds, i.e. format, array ranges and mip slices.
             *
             * @param aTexture     The subjacent texture to test against.
             * @param aTextureView The view to validate.
             * @return             True, if valid. False otherwise.
             */
            bool validateTextureView(
                SRenderGraphImage     const &aTexture,
                    SRenderGraphImageView const &aTextureView);

            /**
             * Validate, whether the texture's requested usages from all views based on the texture
             * are in the list of permitted usages of the texture.
             *
             * @param aTexture The texture to validate.
             * @return         True, if valid. False otherwise.
             */
            bool validateTextureUsage(SRenderGraphImage const &aTexture);

            /**
             * Validate the formats of a texture and a texture view, i.e. check, whether
             * the view's format is compatible or equal to the the texture's internal
             * format.
             *
             * @param aTexture     The texture to test against.
             * @param aTextureView The view to validate.
             * @return             True, if valid. False otherwise.
             */
            bool validateTextureFormat(
                SRenderGraphImage     const &aTexture,
                    SRenderGraphImageView const &aTextureView);

            /**
             * Validate the array range and mip slice access of a texture view, relative
             * to it's subjacent texture, so that no out of bound access is performed by
             * a texture view.
             *
             * @param aTexture     The texture to test against.
             * @param aTextureView The view to validate.
             * @return             True, if valid. False otherwise.
             */
            bool validateTextureSubresourceAccess(
                SRenderGraphImage     const &aTexture,
                    SRenderGraphImageView const &aTextureView);

            /**
             * To be implemented.
             *
             * @param aBuffer
             * @param aBufferView
             * @return
             */
            bool validateBufferView(
                    SRenderGraphBuffer     const &aBuffer,
                    SRenderGraphBufferView const &aBufferView);

        private_members:

            framegraph::CGraph::EGraphMode                  mGraphMode;
            bool                                            mRenderToBackBuffer;
            RenderGraphResourceId_t                         mOutputResourceId;

            Shared<os::SApplicationEnvironment>             mApplicationEnvironment;
            Shared<wsi::CWSIDisplay>                        mDisplay;

            CResourceManager const                         &mResourceManager;

            Shared<IUIDGenerator<RenderPassUID_t>>          mRenderPassUIDGenerator;
            Shared<IUIDGenerator<PassUID_t>>                mSubpassUIDGenerator;
            Shared<IUIDGenerator<RenderGraphResourceId_t>>  mResourceUIDGenerator;

            RenderGraphResourceIdList                        mResources;
            CRenderGraphMutableResources                     mResourceData;

            Shared<CRenderPass>                             mRenderPassUnderConstruction;
            RenderPassMap                                   mRenderPasses;
            datastructures::CAdjacencyTree<RenderPassUID_t> mRenderPassTree;
            PassMap                                         mSubpasses;

            Unique<CGraph>                                  mRenderGraph;

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            AdjacencyListMap_t<RenderGraphResourceId_t>             mResourceAdjacency;
            AdjacencyListMap_t<PassUID_t, RenderGraphResourceId_t>  mPassToResourceAdjacency;
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
        CEngineResult<Shared<TPass>> CGraphBuilder::addSubpass(
                std::string       const &aName,
                TPassCreationArgs &&...  aArgs)
        {
            if(nullptr == graph())
            {
                CLog::Error(logTag(), "No graph instance available.");
                return { EEngineStatus::Error };
            }

            if(nullptr == mRenderPassUnderConstruction)
            {
                CLog::Error(logTag(), "No render pass instance under construction to which the subpass could be added.");
                return { EEngineStatus::Error };
            }

            try
            {
                PassUID_t const uid = generateSubpassUID();

                CEngineResult<Shared<TPass>> passCreation = graph()->createPass<TPass, TPassCreationArgs...>(uid, aName, std::forward<TPassCreationArgs>(aArgs)...);
                if(not passCreation.successful())
                {
                    CLog::Error(logTag(), "No pass instance created.");
                    return { EEngineStatus::Error };
                }
                Shared<TPass> &pass = passCreation.data();

                // Link the pass providing the import and export resources for the passes from the variadic argument list.
                CPassBuilder passBuilder(uid, pass, mRenderPassUnderConstruction, mResourceManager, mResourceData);

                auto const &status = pass->setup(passBuilder);
                if(CheckEngineError(status))
                {
                    CLog::Error(logTag(), "Cannot setup pass instance.");
                    return { EEngineStatus::Error };
                }

                mRenderPassUnderConstruction->addSubpass(pass);
                mSubpasses.insert({ uid, pass });

                return { EEngineStatus::Ok, pass };
            }
            catch(std::exception const &e)
            {
                CLog::Error(logTag(), e.what());
                return { EEngineStatus::Error };
            }
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

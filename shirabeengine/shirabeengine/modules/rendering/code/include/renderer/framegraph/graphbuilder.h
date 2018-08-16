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
#include <core/random.h>
#include <core/uid.h>
#include <resources/core/resourcedomaintransfer.h>
#include "renderer/framegraph/framegraph.h"
#include "renderer/framegraph/pass.h"

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
            CGraphBuilder();

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
             * @return                        True, if successful. False otherwise.
             */
            bool initialize(CStdSharedPtr_t<SApplicationEnvironment> const &aApplicationEnvironment);

            /**
             * Clear all state and shutdown...
             *
             * @return True, if successful. False otherwise.
             */
            bool deinitialize();

            /**
             * Fetch the internal resource UID generator to request a new UID.
             *
             * @return Pointer to the internal resource UID generator.
             */
            CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> resourceUIDGenerator();

            /**
             * Return the application environment pointer used by the builder.
             *
             * @return A pionter to the registered application environment.
             */
            CStdSharedPtr_t<SApplicationEnvironment> &applicationEnvironment();

            /**
             * Return all current resources stored in the builder.
             *
             * @return See brief.
             *
             */
            SHIRABE_INLINE CFrameGraphResources const &getResources() const
            {
                return mResourceData;
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
            CStdSharedPtr_t<TPass> spawnPass(
                    std::string       const&aName,
                    TPassCreationArgs  &&...aArgs);

            /**
             * Register an external texture for read/write inside the render graph.
             *
             * @param aReadableName The name of the texture to register
             * @param aTexture      The texture descriptor to register.
             * @return              Returns a framegraph resource handle for the texture.
             */
            SFrameGraphResource registerTexture(
                    std::string        const &aReadableName,
                    SFrameGraphTexture const &aTexture);

            /**
             * Register a list of renderables for reading inside the render graph.
             *
             * @param readableIdentifier The name of the renderable collection.
             * @param renderables        The renderables to register.
             * @return                   Returns a framgraph resource hadnle for the list.
             */
            SFrameGraphResource registerRenderables(
                    std::string               const &aReadableIdentifier,
                    rendering::RenderableList const &aRenderables);

            /**
             * Compile the graph, collecting all passes, resources and relationship,
             * performing various steps like a depth-first sorting to determine pass
             * execution order as well as culling to reduce and optimize resource
             * redundancy and usage.
             *
             * @return A pointer to a compiled and executable CGraph instance.
             */
            CStdUniquePtr_t<CGraph> compile();

        private_methods:
            /**
             * Generate a new PassUID.
             *
             * @return See brief.
             */
            FrameGraphResourceId_t generatePassUID();

            /**
             * Return the current internal graph state.
             *
             * @return See brief.
             */
            CStdUniquePtr_t<CGraph> &graph();

            /**
             * Accepts an arbitrary resource and tries to traverse the resource-resourceview-path to find
             * the subjacent resource.
             *
             * @param aResourceMap          A map of resources containing all resources in the path from
             *                              aResourceToSearchFrom and the subjacent resource.
             * @param aResourceToSearchFrom Starting point of the path towards the subjacent resource.
             * @return                      The FrameGraphResourceId_t of the subjacent resource or 0 if not found.
             */
            FrameGraphResourceId_t findSubjacentResource(
                    SFrameGraphResourceMap const &aResourceMap,
                    SFrameGraphResource    const &aResourceToSearchFrom);

            /**
             * Collect a fully set-up pass, performing quite a lot of validation and resource work.
             *
             * @param aPass The pass to collect.
             * @return      True, if successful. False, otherwise.
             */
            bool collectPass(CStdSharedPtr_t<CPassBase> aPass);

            /**
             * Perform a topological depth first sort of the graph,
             * yielding it's pass execution order.
             *
             * @param aOutPassOrder Stack to push the pass execution order to.
             * @return              True, if successful. False otherwise.
             */
            template <typename TUID>
            bool topologicalSort(std::stack<TUID> &aOutPassOrder);

            /**
             * Validate the pass order and data flow dependencies of a sorted
             * list of passes.
             *
             * @param aPassOrder The stack containing the pass order to be validated.
             * @return           True, if valid. False otherwise.
             */
            bool validate(std::stack<PassUID_t> const &aPassOrder);

            /**
             * Validate, whether the textureview tries to access the subjacent texture in
             * correct bounds, i.e. format, array ranges and mip slices.
             *
             * @param aTexture     The subjacent texture to test against.
             * @param aTextureView The view to validate.
             * @return             True, if valid. False otherwise.
             */
            bool validateTextureView(
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aTextureView);

            /**
             * Validate, whether the texture's requested usages from all views based on the texture
             * are in the list of permitted usages of the texture.
             *
             * @param aTexture The texture to validate.
             * @return         True, if valid. False otherwise.
             */
            bool validateTextureUsage(SFrameGraphTexture const &aTexture);

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
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aTextureView);

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
                    SFrameGraphTexture     const &aTexture,
                    SFrameGraphTextureView const &aTextureView);

            /**
             * To be implemented.
             *
             * @param aBuffer
             * @param aBufferView
             * @return
             */
            bool validateBufferView(
                    SFrameGraphBuffer     const &aBuffer,
                    SFrameGraphBufferView const &aBufferView);

        private_members:
            CStdSharedPtr_t<SApplicationEnvironment>               mApplicationEnvironment;
            CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> mPassUIDGenerator;
            CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> mResourceUIDGenerator;
            Map<std::string, PublicResourceId_t>                   mImportedResources;
            PassMap                                                mPasses;
            FrameGraphResourceIdList                               mResources;
            CFrameGraphMutableResources                            mResourceData;
            AdjacencyListMap_t<FrameGraphResourceId_t>             mResourceAdjacency;
            AdjacencyListMap_t<PassUID_t>                          mPassAdjacency;
            AdjacencyListMap_t<PassUID_t, FrameGraphResourceId_t>  mPassToResourceAdjacency;
            CStdUniquePtr_t<CGraph>                                mFrameGraph;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <
                typename    TPass,
                typename... TPassCreationArgs
                >
        CStdSharedPtr_t<TPass> CGraphBuilder::spawnPass(
                std::string                 const&aName,
                TPassCreationArgs            &&...aArgs)
        {
            if(!graph())
            {
                return false;
            }

            try
            {
                CStdUniquePtr_t<CGraph::CMutableAccessor> accessor = graph()->getMutableAccessor(PassKey<CGraphBuilder>());

                PassUID_t const uid = generatePassUID();

                CStdSharedPtr_t<TPass> pass =
                        accessor->createPass<TPass, TPassCreationArgs...>(uid, aName, std::forward<TPassCreationArgs>(aArgs)...);
                if(!pass)
                {
                    return nullptr;
                }

                // Link the pass providing the import and export resources for the passes from the variadic argument list.
                // This will declare all required resources:
                //   - Create
                //   - Read
                //   - Write
                //   - Import
                PassBuilder passBuilder(uid, pass, mResourceData);

                bool const passSetupSuccessful = pass->setup(passBuilder);
                if(!passSetupSuccessful)
                {
                    CLog::Error(logTag(), "Cannot setup pass instance.");
                    pass = nullptr;
                    return nullptr;
                }

                mPasses[pass->passUID()] = pass;

                //
                // IMPORTANT: Perform implicit collection at this point in order to provide
                //            any subsequent pass spawn and setup to access already available
                //            resource descriptions!
                /*if(!collectPass(passBuilder))
                {
                    Log::Error(logTag(), "Cannot collect pass after setup.");
                    pass = nullptr;
                    return nullptr;
                }*/

                // Passes are added to the graph on compilation!!! Move there once the environment is setup.
                // if!(graph()->addPass(name, std::static_pointer_cast<PassBase>(pass))) {
                //  // TODO: Log
                //  pass = nullptr;
                //  return nullptr;
                // }

                // Read out the PassLinker state filled in by "setup(...)" and properly merge it with
                // the current graph builder state.

                return pass;
            }
            catch(std::exception e)
            {
                CLog::Error(logTag(), e.what());
                throw;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TUID>
        bool CGraphBuilder::topologicalSort(std::stack<TUID> &aOutPassOrder)
        {
            std::function<
                    void(
                        AdjacencyListMap_t<TUID> const&,
                        TUID const                    &,
                        std::map<TUID, bool>          &,
                        std::stack<TUID>              &)> DSFi_fn;

            // Define the recursive sort function
            DSFi_fn = [&](
                    AdjacencyListMap_t<TUID> const&aEdges,
                    TUID const                    &aVertex,
                    std::map<TUID, bool>          &aVisitedEdges,
                    std::stack<TUID>              &aPassOrder) -> void
            {
                bool const edgeVisited = aVisitedEdges[aVertex];
                if(edgeVisited)
                {
                    return;
                }

                aVisitedEdges[aVertex] = true;

                // For each outgoing edge...
                bool const doesNotContainEdge = (aEdges.find(aVertex) == aEdges.end());
                if(!doesNotContainEdge)
                {
                    for(TUID const &adjacent : aEdges.at(aVertex))
                    {
                        DSFi_fn(aEdges, adjacent, aVisitedEdges, aPassOrder);
                    }
                }

                aPassOrder.push(aVertex);
            };

            // Kick-off the sort algorithm
            try
            {
                std::map<TUID, bool> visitedEdges = {};
                for(typename AdjacencyListMap_t<TUID>::value_type &passAdjacency : mPassAdjacency)
                {
                    visitedEdges[passAdjacency.first] = false;
                }

                for(typename AdjacencyListMap_t<TUID>::value_type &passAdjacency : mPassAdjacency)
                {
                    DSFi_fn(mPassAdjacency, passAdjacency.first, visitedEdges, aOutPassOrder);
                }
            }
            catch(std::runtime_error const &aRTE)
            {
                CLog::Error(logTag(), CString::format("Failed to perform topological sort: %0 ", aRTE.what()));
                return false;
            }
            catch(...)
            {
                CLog::Error(logTag(), "Failed to perform topological sort. Unknown error.");
                return false;
            }

            return true;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

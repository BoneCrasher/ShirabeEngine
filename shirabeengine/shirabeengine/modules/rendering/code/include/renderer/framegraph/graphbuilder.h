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
         * @brief The CGraphBuilder class
         */
        class SHIRABE_TEST_EXPORT CGraphBuilder
        {
            SHIRABE_DECLARE_LOG_TAG(CGraphBuilder);

        public_constructors:
            CGraphBuilder();

        public_destructors:
            ~CGraphBuilder() = default;

        public_methods:
            /**
             * @brief initialize
             *
             * @param aApplicationEnvironment
             * @return
             */
            bool initialize(CStdSharedPtr_t<SApplicationEnvironment> const &aApplicationEnvironment);

            /**
             * @brief deinitialize
             *
             * @return
             */
            bool deinitialize();

            /**
             * @brief resourceUIDGenerator
             *
             * @return
             */
            CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> resourceUIDGenerator();

            /**
             * @brief spawnPass
             *
             * @tparam TPass
             * @tparam TPassCreationArgs
             * @param aName
             * @param aArgs
             * @return
             */
            template <
                    typename    TPass,
                    typename... TPassCreationArgs
                    >
            CStdSharedPtr_t<TPass>
            spawnPass(
                    std::string                 const&aName,
                    TPassCreationArgs            &&...aArgs);

            /**
             * @brief registerTexture
             * @param aReadableName
             * @param aTexture
             * @return
             */
            SFrameGraphResource registerTexture(
                    std::string        const &aReadableName,
                    SFrameGraphTexture const &aTexture);

            /**
             * @brief registerRenderables
             * @param readableIdentifier
             * @param renderables
             * @return
             */
            SFrameGraphResource registerRenderables(
                    std::string               const &aReadableIdentifier,
                    rendering::RenderableList const &aRenderables);

            /**
             * @brief compile
             * @return
             */
            CStdUniquePtr_t<CGraph> compile();

            /**
             * @brief applicationEnvironment
             * @return
             */
            CStdSharedPtr_t<SApplicationEnvironment> &applicationEnvironment();

            /**
             * @brief getResources
             * @return
             */
            SHIRABE_INLINE CFrameGraphResources const &getResources() const
            {
                return m_resourceData;
            }

        private_methods:
            /**
             * @brief generatePassUID
             * @return
             */
            FrameGraphResourceId_t generatePassUID();

            /**
             * @brief graph
             * @return
             */
            CStdUniquePtr_t<CGraph> &graph();

            /**
             * @brief findSubjacentResource
             * @param aResourceMap
             * @param aResourceToFind
             * @return
             */
            FrameGraphResourceId_t findSubjacentResource(
                    SFrameGraphResourceMap const &aResourceMap,
                    SFrameGraphResource    const &aResourceToFind);

            /**
             * @brief collectPass
             * @param aPass
             * @return
             */
            bool collectPass(CStdSharedPtr_t<CPassBase> aPass);

            /**
             * @brief topologicalSort
             * @param aOutPassOrder
             * @return
             */
            template <typename TUID>
            bool topologicalSort(std::stack<TUID> &aOutPassOrder);

            bool validate(std::stack<PassUID_t> const&passOrder);
            bool validateTextureView(FrameGraphTexture const&, FrameGraphTextureView const&);
            bool validateTextureUsage(FrameGraphTexture const&);
            bool validateTextureFormat(FrameGraphTexture const&, FrameGraphTextureView const&);
            bool validateTextureSubresourceAccess(FrameGraphTexture const&, FrameGraphTextureView const&);
            bool validateBufferView(FrameGraphBuffer const&, FrameGraphBufferView const&);

        private_members:
            CStdSharedPtr_t<ApplicationEnvironment> m_applicationEnvironment;

            CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> m_passUIDGenerator;
            CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> m_resourceUIDGenerator;

            Map<std::string, PublicResourceId_t> m_importedResources;

            PassMap                    m_passes;
            FrameGraphResourceIdList   m_resources;
            FrameGraphMutableResources m_resourceData;

            AdjacencyListMap<FrameGraphResourceId_t>            m_resourceAdjacency;
            AdjacencyListMap<PassUID_t>                         m_passAdjacency;
            AdjacencyListMap<PassUID_t, FrameGraphResourceId_t> m_passToResourceAdjacency;

            UniqueCStdSharedPtr_t<Graph> m_frameGraph;
        };

        /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation, typename... TPassCreationArgs> CStdSharedPtr_t<Pass<TPassImplementation>> GraphBuilder::spawnPass( std::string const&id, TPassCreationArgs&&... args)
     *
     * \brief Spawn pass
     *
     * \tparam  TPassImplementation Type of the pass implementation.
     * \tparam  TPassCreationArgs   Type of the pass creation arguments.
     * \param id    The identifier.
     * \param args  Variable arguments providing [in,out] The arguments.
     *
     * \return  A Ptr&lt;Pass&lt;TPassImplementation&gt;&gt;
     **************************************************************************************************/
        template <typename TPass, typename... TPassCreationArgs>
        CStdSharedPtr_t<TPass>
        CGraphBuilder::spawnPass(
                std::string                 const&name,
                TPassCreationArgs            &&...args)
        {
            if(!graph())
                return false;

            try {
                UniqueCStdSharedPtr_t<Graph::MutableAccessor> accessor = graph()->getMutableAccessor(PassKey<CGraphBuilder>());

                PassUID_t uid = generatePassUID();

                CStdSharedPtr_t<TPass> pass =
                        accessor->createPass<TPass, TPassCreationArgs...>(uid, name, std::forward<TPassCreationArgs>(args)...);
                if(!pass)
                    return nullptr;

                // Link the pass providing the import and export resources for the passes from the variadic argument list.
                // This will declare all required resources:
                //   - Create
                //   - Read
                //   - Write
                //   - Import
                PassBuilder passBuilder(uid, pass, m_resourceData);
                if(!pass->setup(passBuilder)) {
                    Log::Error(logTag(), "Cannot setup pass instance.");
                    pass = nullptr;
                    return nullptr;
                }

                m_passes[pass->passUID()] = pass;

                //
                // IMPORTANT: Perform implicit collection at this point in order to provide
                //            any subsequent pass spawn and setup to access already available
                //            resource descriptions!
                /*if(!collectPass(passBuilder)) {
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
            catch(std::exception e) {
                Log::Error(logTag(), e.what());
                throw;
            }
        }


        /**********************************************************************************************//**
     * \fn  bool GraphBuilder::topologicalSort(std::stack<PassUID_t>&outPassOrder)
     *
     * \brief Topological sort
     *
     * \param [in,out]  outPassOrder  The out pass order.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
        template <typename TUID>
        bool
        CGraphBuilder::topologicalSort(std::stack<TUID>&outPassOrder)
        {
            std::function<
                    void(
                        AdjacencyListMap<TUID> const&,
                        TUID const                  &,
                        std::map<TUID, bool>        &,
                        std::stack<TUID>            &)> DSFi;

            DSFi = [&](
                    AdjacencyListMap<TUID> const&edges,
                    TUID const                  &v,
                    std::map<TUID, bool>        &visitedEdges,
                    std::stack<TUID>            &passOrder) -> void
            {
                if(visitedEdges[v])
                    return;

                visitedEdges[v] = true;

                // For each outgoing edge...
                if(!(edges.find(v) == edges.end())) {
                    for(TUID const&adjacent : edges.at(v)) {
                        DSFi(edges, adjacent, visitedEdges, passOrder);
                    }
                }

                passOrder.push(v);
            };

            try {
                std::map<TUID, bool> visitedEdges {};
                for(typename AdjacencyListMap<TUID>::value_type &passAdjacency : m_passAdjacency) {
                    visitedEdges[passAdjacency.first] = false;
                }

                for(typename AdjacencyListMap<TUID>::value_type &passAdjacency : m_passAdjacency) {
                    DSFi(m_passAdjacency, passAdjacency.first, visitedEdges, outPassOrder);
                }
            }
            catch(std::runtime_error const&rte) {
                Log::Error(logTag(), String::format("Failed to perform topological sort: %0 ", rte.what()));
                return false;
            }
            catch(...) {
                Log::Error(logTag(), "Failed to perform topological sort. Unknown error.");
                return false;
            }

            return true;
        }


    }
}

#endif

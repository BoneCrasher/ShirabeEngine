#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <stack>
#include <utility>

#include "core/enginetypehelper.h"
#include "Core/Random.h"
#include "Core/UID.h"

#include "Log/Log.h"
#include "resources/core/resourcedto.h"

#include "Renderer/FrameGraph/FrameGraph.h"
#include "Renderer/FrameGraph/Pass.h"

namespace engine {
  namespace FrameGraph {

    using namespace engine::Core;
    using namespace engine::Resources;

    /**********************************************************************************************//**
     * \class GraphBuilder
     *
     * \brief A graph builder.
     **************************************************************************************************/
    class SHIRABE_TEST_EXPORT GraphBuilder {
      SHIRABE_DECLARE_LOG_TAG(GraphBuilder);

    public:
      GraphBuilder();
      ~GraphBuilder() = default;

      bool initialize(
        CStdSharedPtr_t<ApplicationEnvironment> const&environment);
      bool deinitialize();

      CStdSharedPtr_t<IUIDGenerator<FrameGraphResourceId_t>> resourceUIDGenerator();

      template <typename TPass, typename... TPassCreationArgs>
      CStdSharedPtr_t<TPass>
        spawnPass(
          std::string                 const&name,
          TPassCreationArgs            &&...args);
      
      FrameGraphResource
        registerTexture(
          std::string       const&readableName,
          FrameGraphTexture const&texture);

      FrameGraphResource
        registerRenderables(
          std::string               const&readableIdentifier,
          Rendering::RenderableList const&renderables);

      UniqueCStdSharedPtr_t<Graph>
        compile();
      
      CStdSharedPtr_t<ApplicationEnvironment>& env();

      inline FrameGraphResources const&getResources() const { return m_resourceData; }

    private:
      FrameGraphResourceId_t generatePassUID();

      UniqueCStdSharedPtr_t<Graph>&            graph();

      FrameGraphResourceId_t findSubjacentResource(FrameGraphResourceMap const&, FrameGraphResource const&);

      bool collectPass(CStdSharedPtr_t<PassBase> pass);
     
      template <typename TUID>
      bool topologicalSort(std::stack<TUID>&outPassOrder);
      bool validate(std::stack<PassUID_t> const&passOrder);
      bool validateTextureView(FrameGraphTexture const&, FrameGraphTextureView const&);
      bool validateTextureUsage(FrameGraphTexture const&);
      bool validateTextureFormat(FrameGraphTexture const&, FrameGraphTextureView const&);
      bool validateTextureSubresourceAccess(FrameGraphTexture const&, FrameGraphTextureView const&);
      bool validateBufferView(FrameGraphBuffer const&, FrameGraphBufferView const&);

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
      GraphBuilder::spawnPass(
        std::string                 const&name,
        TPassCreationArgs            &&...args)
    {
      if(!graph())
        return false;

      try {
        UniqueCStdSharedPtr_t<Graph::MutableAccessor> accessor = graph()->getMutableAccessor(PassKey<GraphBuilder>());

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
      GraphBuilder::topologicalSort(std::stack<TUID>&outPassOrder)
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
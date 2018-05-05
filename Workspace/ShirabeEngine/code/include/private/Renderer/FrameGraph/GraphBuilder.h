#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <stack>

#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"
#include "Core/UID.h"

#include "Log/Log.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/FrameGraph/FrameGraph.h"
#include "Renderer/FrameGraph/Pass.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Core;
    using namespace Engine::Resources;

    /**********************************************************************************************//**
     * \class GraphBuilder
     *
     * \brief A graph builder.
     **************************************************************************************************/
    class SHIRABE_TEST_EXPORT GraphBuilder {
      DeclareLogTag(GraphBuilder);
      
    public:
      GraphBuilder();
      ~GraphBuilder() = default;

      bool initialize(
        Ptr<ApplicationEnvironment> const&environment);
      bool deinitialize();

      Ptr<IUIDGenerator<FrameGraphResourceId_t>> resourceUIDGenerator();

      template <typename TPass, typename... TPassCreationArgs>
      Ptr<TPass>
        spawnPass(
          std::string                 const&name,
          TPassCreationArgs            &&...args);

      Ptr<ApplicationEnvironment> getApplicationEnvironment();

      FrameGraphResource
        registerTexture(
          std::string       const&readableName,
          FrameGraphTexture const&texture);

      UniquePtr<Graph>
        compile();

      FrameGraphTexture     const&getTextureData(FrameGraphResource const&resource) const;
      FrameGraphTextureView const&getTextureViewData(FrameGraphResource const&resource) const;

    private:
      FrameGraphResourceId_t generatePassUID();

      UniquePtr<Graph>& graph();

      FrameGraphResourceId_t findSubjacentResource(FrameGraphResourceMap const&, FrameGraphResource const&);

      bool collectPass(PassBuilder&passBuilder);
      bool topologicalSort(std::stack<PassUID_t>&outPassOrder);
      bool validate(std::stack<PassUID_t> const&passOrder);
      bool validateTextureView(FrameGraphTexture const&, FrameGraphTextureView const&);
      bool validateTextureUsage(FrameGraphTexture const&);
      bool validateTextureFormat(FrameGraphTexture const&, FrameGraphTextureView const&);
      bool validateTextureSubresourceAccess(FrameGraphTexture const&, FrameGraphTextureView const&);
      bool validateBufferView(FrameGraphBuffer const&, FrameGraphBufferView const&);
      
      Ptr<ApplicationEnvironment> m_applicationEnvironment;

      Ptr<IUIDGenerator<FrameGraphResourceId_t>> m_passUIDGenerator;
      Ptr<IUIDGenerator<FrameGraphResourceId_t>> m_resourceUIDGenerator;

      Map<std::string, PublicResourceId_t> m_importedResources;

      PassMap               m_passes;
      FrameGraphResourceMap m_resources;

      AdjacencyListMap<FrameGraphResourceId_t> m_resourceAdjacency;
      AdjacencyListMap<PassUID_t>              m_passAdjacency;

      UniquePtr<Graph> m_frameGraph;

    };

    /**********************************************************************************************//**
     * \fn  template <typename TPassImplementation, typename... TPassCreationArgs> Ptr<Pass<TPassImplementation>> GraphBuilder::spawnPass( std::string const&id, TPassCreationArgs&&... args)
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
    Ptr<TPass>
      GraphBuilder::spawnPass(
        std::string                 const&name,
        TPassCreationArgs            &&...args)
    {
      if(!graph())
        return false;

      try {
        PassUID_t uid = generatePassUID();

        Ptr<TPass>
          pass = MakeSharedPointerType<TPass>(uid, name, std::forward<TPassCreationArgs>(args)...);
        if(!pass)
          return nullptr;

        // Link the pass providing the import and export resources for the passes from the variadic argument list.
        // This will declare all required resources:
        //   - Create
        //   - Read
        //   - Write
        //   - Import
        PassBuilder passBuilder(uid, m_resourceUIDGenerator);
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
        if(!collectPass(passBuilder)) {
          Log::Error(logTag(), "Cannot collect pass after setup.");
          pass = nullptr;
          return nullptr;
        }

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
        return nullptr;
      }
      catch(...) {
        return nullptr;
      }
    }


  }
}

#endif
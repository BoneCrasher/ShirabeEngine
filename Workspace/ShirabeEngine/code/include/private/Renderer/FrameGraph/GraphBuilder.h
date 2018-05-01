#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>
#include <map>

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

      template <typename TUnderlyingID>
      using AdjacencyListMap = std::map<TUnderlyingID, std::vector<TUnderlyingID>>;


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

      bool
        importPersistentResource(
          std::string        const&id,
          PublicResourceId_t const&resourceId);

      UniquePtr<FrameGraph>
        compile();

      FrameGraphTexture     const&getTextureData(FrameGraphResource const&resource) const;
      FrameGraphTextureView const&getTextureViewData(FrameGraphResource const&resource) const;

    private:
      FrameGraphResourceId_t generatePassUID();

      UniquePtr<FrameGraph>&                graph();
      Map<std::string, PublicResourceId_t>& importedResources();

      bool collectPass(PassBuilder const&passBuilder);

      Ptr<ApplicationEnvironment> m_applicationEnvironment;

      Ptr<IUIDGenerator<FrameGraphResourceId_t>> m_passUIDGenerator;
      Ptr<IUIDGenerator<FrameGraphResourceId_t>> m_resourceUIDGenerator;

      Map<std::string, PublicResourceId_t> m_importedResources;
      FrameGraphResourceMap                m_resources;

      AdjacencyListMap<FrameGraphResourceId_t> m_resourceAdjacency;
      AdjacencyListMap<PassUID_t>              m_passAdjacency;

      UniquePtr<FrameGraph> m_frameGraph;

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
          pass = MakeSharedPointerType<TPass>(uid, std::forward<TPassCreationArgs>(args)...);
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
        //if!(graph()->addPass(name, std::static_pointer_cast<PassBase>(pass))) {
        //  // TODO: Log
        //  pass = nullptr;
        //  return nullptr;
        //}

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
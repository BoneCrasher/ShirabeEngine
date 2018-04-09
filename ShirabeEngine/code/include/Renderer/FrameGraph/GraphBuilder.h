#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"
#include "Resources/Core/ResourceDTO.h"

#include "Renderer/FrameGraph/FrameGraph.h"
#include "Renderer/FrameGraph/Pass.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Resources;

    /**********************************************************************************************//**
     * \class GraphBuilder
     *
     * \brief A graph builder.
     **************************************************************************************************/
    class SHIRABE_TEST_EXPORT GraphBuilder {
    public:
      GraphBuilder();
      ~GraphBuilder() = default;

      bool initialize(
        Ptr<ApplicationEnvironment> const&environment);
      bool deinitialize();

      Random::RandomState& resourceUIDGenerator();

      template <typename TPassImplementation, typename... TPassCreationArgs>
      Ptr<Pass<TPassImplementation>>
        spawnPass(
          std::string                 const&name,
          TPassCreationArgs            &&...args);

      bool
        importPersistentResource(
          std::string        const&id,
          PublicResourceId_t const&resourceId);

      UniquePtr<FrameGraph>
        compile();

    private:
      FrameGraphResourceId_t generatePassUID();

      UniquePtr<FrameGraph>&                graph();
      Map<std::string, PublicResourceId_t>& importedResources();

      Random::RandomState m_uidGenerator;

      UniquePtr<FrameGraph>                m_frameGraph;
      Map<std::string, PublicResourceId_t> m_importedResources;

      Ptr<ApplicationEnvironment>          m_applicationEnvironment;
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
    template <typename TPassImplementation, typename... TPassCreationArgs>
    Ptr<Pass<TPassImplementation>>
      GraphBuilder::spawnPass(
        std::string                 const&name,
        TPassCreationArgs            &&...args)
    {                               
      if (!graph())
        return false;

      try {
        FrameGraphResourceId_t uid = generatePassUID();

        UniquePtr<TPassImplementation>
          passImplementation = MakeUniquePointerType<TPassImplementation>();
        Ptr<Pass<TPassImplementation>>
          pass = MakeSharedPointerType<Pass<TPassImplementation>>(uid, std::move(passImplementation));
        if (!pass)
          return false;

        // Link the pass providing the input and output resources for the passes from the variadic argument list.
        // This will declare all required resources:
        //   - Create
        //   - Read
        //   - Write
        //   - Import
        if (!pass->setup<TPassCreationArgs...>(m_applicationEnvironment, *this, std::forward<TPassCreationArgs>(args)...)) {

        }

        graph()->passes().push_back(std::static_pointer_cast<PassBase>(pass));

        // Read out the PassLinker state filled in by "setup(...)" and properly merge it with 
        // the current graph builder state.
        

        return pass;
      }
      catch (std::exception e) {
        return nullptr;
      }
      catch (...) {
        return nullptr;
      }
    }

  }
}

#endif
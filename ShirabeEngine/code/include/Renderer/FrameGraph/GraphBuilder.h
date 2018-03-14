#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Core/Random.h"
#include "Resources/Core/ResourceDTO.h"

#include "FrameGraph.h"
#include "Pass.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Resources;

    /**********************************************************************************************//**
     * \class GraphBuilder
     *
     * \brief A graph builder.
     **************************************************************************************************/
    class GraphBuilder {
    public:
      GraphBuilder();

      bool initialize();
      bool deinitialize();

      template <typename TPassImplementation, typename... TPassCreationArgs>
      Ptr<Pass<TPassImplementation>>
        spawnPass(
          std::string      const&id,
          TPassCreationArgs&&... args);

      bool
        importPersistentResource(
          std::string        const&id,
          PublicResourceId_t const&resourceId);

      UniquePtr<FrameGraph>
        compile();

    private:
      FrameGraphResourceId_t generatePassUID();

      inline UniquePtr<FrameGraph>&                graph() { return m_frameGraph; }
      inline Map<std::string, PublicResourceId_t>& importedResources() { return m_importedResources; }

      Random::RandomState m_uidGenerator;

      UniquePtr<FrameGraph>                m_frameGraph;
      Map<std::string, PublicResourceId_t> m_importedResources;
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
        std::string       const&name,
        TPassCreationArgs  &&...args)
    {
      if (!graph())
        return false;

      try {
        FrameGraphResourceId_t uid = generatePassUID();

        UniquePtr<TPassImplementation>
          passImplementation = MakeUniquePointerType<TPassImplementation>(std::forward<TPassCreationArgs>(args)...);
        Ptr<Pass<TPassImplementation>>
          pass = MakeSharedPointerType<Pass<TPassImplementation>>(uid, std::move(passImplementation));
        if (!pass)
          return false;

        if (!pass->setup(*this)) {

        }

        graph()->passes().push_back(std::static_pointer_cast<PassBase>(pass));

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
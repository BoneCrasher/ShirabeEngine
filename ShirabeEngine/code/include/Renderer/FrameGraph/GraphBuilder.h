#ifndef __SHIRABE_RENDERER_GRAPHBUILDER_H__
#define __SHIRABE_RENDERER_GRAPHBUILDER_H__

#include <string>
#include <functional>

#include "Core/EngineTypeHelper.h"
#include "Resources/Core/ResourceDTO.h"

#include "Pass.h"
#include "FrameGraph.h"

namespace Engine {
  namespace FrameGraph {

    using namespace Engine::Resources;

    class GraphBuilder {
      bool initialize();
      bool deinitialize();

      template <typename TPassImplementation, typename... TPassCreationArgs>
      Ptr<PassBase>
        createPass(
          std::string      const&id,
          TPassCreationArgs&&... args);

      bool
        importPersistentResource(
          std::string        const&id,
          PublicResourceId_t const&resourceId);

      UniquePtr<FrameGraph>
        compile();

    private:
      inline UniquePtr<FrameGraph>&                graph()             { return m_frameGraph; }
      inline Map<std::string, PublicResourceId_t>& importedResources() { return m_importedResources; }

      UniquePtr<FrameGraph>                m_frameGraph;
      Map<std::string, PublicResourceId_t> m_importedResources;
    };

    template <typename TPassImplementation, typename... TPassCreationArgs>
    Ptr<PassBase>
      GraphBuilder::createPass(
        std::string      const&id,
        TPassCreationArgs&&... args)
    {
      if (!graph())
        return false;

      try {
        Ptr<Pass<TPassImplementation>>
          pass = MakeSharedPointerType<Pass<TPassImplementation>>(std::forward<TPassCreationArgs>(args)...);
        if (!pass)
          return false;

        graph()->passes().push_back(pass);

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
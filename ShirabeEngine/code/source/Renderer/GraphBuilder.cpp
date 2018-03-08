#include "Renderer/FrameGraph/GraphBuilder.h"

namespace Engine {
  namespace FrameGraph {

    bool
      GraphBuilder::initialize()
    {
      graph() = MakeUniquePointerType<FrameGraph>();

      return true;
    }

    bool
      GraphBuilder::deinitialize()
    {
      if (graph())
        graph() = nullptr;

      return true;
    }

    bool
      GraphBuilder::importPersistentResource(
        std::string        const&id,
        PublicResourceId_t const&resourceId)
    {
      try {
        bool alreadyAvailableForId = importedResources().find(id) == importedResources().end();

        if (!alreadyAvailableForId)
          importedResources()[id] = resourceId;

        return (!alreadyAvailableForId);
      }
      catch (std::exception) {
        return false;
      }
      catch (...) {
        return false;
      }
    }

    UniquePtr<FrameGraph>
      GraphBuilder::compile()
    {
      return nullptr; // static_assert(false, "Not implemented.");
    }
  }
}
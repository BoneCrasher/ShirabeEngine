#ifndef __SHIRABE_RENDERER_TYPES_H__
#define __SHIRABE_RENDERER_TYPES_H__

#include <stdint.h>
#include <string>

#include "Core/EngineTypeHelper.h"

namespace Engine {
  namespace Rendering {

    using MeshId_t     = uint64_t;
    using MaterialId_t = uint64_t;

    struct Renderable {
      std::string  name;
      MeshId_t     meshId;
      MaterialId_t materialId;
    };
    DeclareListType(Renderable, Renderable);

  }

  template <>
  inline std::string to_string<Rendering::Renderable>(Rendering::Renderable const&renderable) {

    std::string message =
      String::format(
        "  Renderable: %0\n"
        "    MeshId:     %1\n"
        "    MaterialId: %2\n",
        renderable.name,
        renderable.meshId,
        renderable.materialId);
    return message;
  }
}

#endif
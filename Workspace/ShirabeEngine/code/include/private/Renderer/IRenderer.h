#ifndef __SHIRABE_IRENDERER_H__
#define __SHIRABE_IRENDERER_H__

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"

#include "Platform/ApplicationEnvironment.h"
#include "Resources/Core/ResourceManager.h"
#include "Renderer/RendererConfiguration.h"

namespace Engine {
  namespace Renderer {
    using namespace Platform;
    using namespace Engine::Resources;

    using MeshId_t     = uint64_t;
    using MaterialId_t = uint64_t;

    struct Renderable {
      std::string  name;
      MeshId_t     meshId;
      MaterialId_t materialId;
    };
    DeclareListType(Renderable, Renderable);

    DeclareInterface(IRenderContext);
    virtual EEngineStatus bindResource(PublicResourceId_t const&)   = 0;
    virtual EEngineStatus unbindResource(PublicResourceId_t const&) = 0;

      virtual EEngineStatus render(Renderable const&) = 0;
    DeclareInterfaceEnd(IRenderContext);

    DeclareInterface(IRenderer);
      virtual EEngineStatus initialize(
        const ApplicationEnvironment&,
        const RendererConfiguration&,
        const Ptr<ResourceManager>&) = 0;

      virtual EEngineStatus deinitialize() = 0;
      virtual EEngineStatus reinitialize() = 0;
      
      virtual EEngineStatus pause()          = 0;
      virtual EEngineStatus resume()         = 0;
      virtual bool          isPaused() const = 0;
      
      virtual EEngineStatus render(Renderable const&) = 0;
    DeclareInterfaceEnd(IRenderer);
    DeclareSharedPointerType(IRenderer);
  }

  template <>
  inline std::string to_string<Renderer::Renderable>(Renderer::Renderable const&renderable) {

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
#ifndef __SHIRABE_IRENDERER_H__
#define __SHIRABE_IRENDERER_H__

#include "Core/BasicTypes.h"
#include "Core/EngineStatus.h"

#include "OS/ApplicationEnvironment.h"
#include "Resources/Core/ResourceManager.h"

#include "Renderer/RendererConfiguration.h"
#include "Renderer/RendererTypes.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"

namespace Engine {
  namespace Rendering {
    using namespace OS;
    using namespace Engine::Resources;

    using Engine::FrameGraph::IFrameGraphRenderContext;

    DeclareInterface(IRenderContext);
    virtual EEngineStatus bindResource(PublicResourceId_t const&)   = 0;
    virtual EEngineStatus unbindResource(PublicResourceId_t const&) = 0;

      virtual EEngineStatus render(Renderable const&) = 0;
    DeclareInterfaceEnd(IRenderContext);

    DeclareInterface(IRenderer);
      virtual EEngineStatus initialize(
        Ptr<ApplicationEnvironment>   const&,
        RendererConfiguration         const&,
        Ptr<IFrameGraphRenderContext>      &) = 0;

      virtual EEngineStatus deinitialize() = 0;
      virtual EEngineStatus reinitialize() = 0;
      
      virtual EEngineStatus pause()          = 0;
      virtual EEngineStatus resume()         = 0;
      virtual bool          isPaused() const = 0;
      
      virtual EEngineStatus renderScene() = 0;

    DeclareInterfaceEnd(IRenderer);
    DeclareSharedPointerType(IRenderer);
  }
}

#endif
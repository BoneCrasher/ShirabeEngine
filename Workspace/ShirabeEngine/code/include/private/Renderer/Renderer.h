#ifndef __SHIRABE_RENDERER_H__
#define __SHIRABE_RENDERER_H__

#include <atomic>

#include "Renderer/IRenderer.h"

namespace Engine {
  namespace Rendering {
    using Engine::FrameGraph::IFrameGraphRenderContext;

    class Renderer {
    public:
      Renderer();
      ~Renderer();

      EEngineStatus initialize(
        ApplicationEnvironment        const&,
        RendererConfiguration         const&,
        Ptr<IFrameGraphRenderContext>      &) ;

      EEngineStatus deinitialize();
      EEngineStatus reinitialize();

      EEngineStatus pause();
      EEngineStatus resume();
      bool          isPaused() const;

      EEngineStatus renderScene();

    private:
      RendererConfiguration m_configuration;

      Ptr<ApplicationEnvironment>   m_appEnvironment;
      Ptr<IFrameGraphRenderContext> m_frameGraphRenderContext;

      std::atomic<bool> m_paused;

    };

  }
}

#endif
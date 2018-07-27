#ifndef __SHIRABE_RENDERER_H__
#define __SHIRABE_RENDERER_H__

#include <atomic>

#include "Renderer/IRenderer.h"

namespace Engine {
  namespace Rendering {
    using Engine::FrameGraph::IFrameGraphRenderContext;

    class Renderer 
      : public IRenderer
    {
    public:
      Renderer();
      ~Renderer();

      EEngineStatus initialize(
        CStdSharedPtr_t<ApplicationEnvironment>   const&,
        RendererConfiguration         const&,
        CStdSharedPtr_t<IFrameGraphRenderContext>      &) ;

      EEngineStatus deinitialize();
      EEngineStatus reinitialize();

      EEngineStatus pause();
      EEngineStatus resume();
      bool          isPaused() const;

      EEngineStatus renderScene();

    private:
      RendererConfiguration m_configuration;

      CStdSharedPtr_t<ApplicationEnvironment>   m_appEnvironment;
      CStdSharedPtr_t<IFrameGraphRenderContext> m_frameGraphRenderContext;

      std::atomic<bool> m_paused;

    };

  }
}

#endif
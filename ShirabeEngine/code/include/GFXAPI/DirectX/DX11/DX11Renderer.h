#ifndef __SHIRABE_DX11_RENDERER_H__
#define __SHIRABE_DX11_RENDERER_H__

#include <atomic>

#include "DX11Common.h"
#include "DX11Linkage.h"
#include "DX11DeviceCapabilities.h"
#include "DX11Device.h"
#include "DX11Types.h"
#include "DX11Environment.h"

#include "GFXAPI/IRenderer.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Renderer;
      using namespace Engine::DX::_11;

      class DX11Renderer
        : public IRenderer
      {
      public:
        DX11Renderer();
        ~DX11Renderer();

        void setDX11Environment(Ptr<DX11Environment> const&pDx11Environment);

        EEngineStatus initialize(
          ApplicationEnvironment const &environment,
          RendererConfiguration  const &configuration,
          IResourceManagerPtr    const &resourceManager);


        EEngineStatus deinitialize();
        EEngineStatus reinitialize();

        EEngineStatus pause();
        EEngineStatus resume();
        bool          isPaused() const;

        EEngineStatus render(/* insert queue type*/);

      private:
        RendererConfiguration m_config;
        IResourceManagerPtr   m_resourceManager;

        Ptr<DX11Environment> m_dx11Environment;

        PublicResourceId_t
          m_swapChainId,
          m_defaultDepthStencilTextureId,
          m_defaultDepthStencilStateId;

        std::atomic_bool m_paused;
      };

    }
  }
}

#endif
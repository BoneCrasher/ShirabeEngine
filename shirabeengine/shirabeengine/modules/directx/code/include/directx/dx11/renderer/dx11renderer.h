#ifndef __SHIRABE_DX11_RENDERER_H__
#define __SHIRABE_DX11_RENDERER_H__

#include <atomic>

#include "GFXAPI/DirectX/DX11/Common.h"
#include "GFXAPI/DirectX/DX11/Linkage.h"
#include "GFXAPI/DirectX/DX11/DeviceCapabilities.h"
#include "GFXAPI/DirectX/DX11/Device.h"
#include "GFXAPI/DirectX/DX11/Types.h"
#include "GFXAPI/DirectX/DX11/Environment.h"

#include "Renderer/IRenderer.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Rendering;
      using namespace Engine::DX::_11;

      class DX11Renderer
        : public IRenderer
      {
      public:
        DX11Renderer();
        ~DX11Renderer();

        void setDX11Environment(Shared<DX11Environment> const&pDx11Environment);

        EEngineStatus initialize(
          ApplicationEnvironment const &environment,
          RendererConfiguration  const &configuration,
          Shared<ResourceManager>   const &resourceManager);


        EEngineStatus deinitialize();
        EEngineStatus reinitialize();

        EEngineStatus pause();
        EEngineStatus resume();
        bool          isPaused() const;

        EEngineStatus render(Renderable const&renderable);

      private:
        RendererConfiguration m_config;
        Shared<ResourceManager>  m_resourceManager;

        Shared<DX11Environment> m_dx11Environment;

        PublicResourceId_t
          mSwapChainId,
          m_defaultDepthStencilTextureId,
          m_defaultDepthStencilStateId;

        std::atomic_bool m_paused;
      };

    }
  }
}

#endif

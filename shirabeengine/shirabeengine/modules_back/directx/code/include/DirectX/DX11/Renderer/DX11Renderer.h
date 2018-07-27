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

        void setDX11Environment(CStdSharedPtr_t<DX11Environment> const&pDx11Environment);

        EEngineStatus initialize(
          ApplicationEnvironment const &environment,
          RendererConfiguration  const &configuration,
          CStdSharedPtr_t<ResourceManager>   const &resourceManager);


        EEngineStatus deinitialize();
        EEngineStatus reinitialize();

        EEngineStatus pause();
        EEngineStatus resume();
        bool          isPaused() const;

        EEngineStatus render(Renderable const&renderable);

      private:
        RendererConfiguration m_config;
        CStdSharedPtr_t<ResourceManager>  m_resourceManager;

        CStdSharedPtr_t<DX11Environment> m_dx11Environment;

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
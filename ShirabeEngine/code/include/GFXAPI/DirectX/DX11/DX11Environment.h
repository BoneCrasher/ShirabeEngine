#ifndef __SHIRABE_DIRECTX11_ENVIRONMENT_H__
#define __SHIRABE_DIRECTX11_ENVIRONMENT_H__

#include "Core/EngineStatus.h"
#include "Platform/ApplicationEnvironment.h"
#include "GFXAPI/RendererConfiguration.h"

#include "DX11Common.h"
#include "DX11Linkage.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      using Platform::ApplicationEnvironment;
      using Engine::Renderer::RendererConfiguration;

      class DX11Environment {
      public:
        DX11Environment();

        EEngineStatus initialize(
          ApplicationEnvironment const& applicationEnvironment,
          RendererConfiguration  const& configuration);
        EEngineStatus deinitialize();

        IDXGISwapChainPtr      getSwapChain();
        ID3D11DevicePtr        getDevice();
        ID3D11DeviceContextPtr getImmediateContext();

      private:
        Engine::GFXAPI::GAPIDeviceCapabilities m_caps;

        IDXGIFactoryPtr        m_dxgiFactory;
        IDXGIAdapterPtr        m_selectedAdapter;
        IDXGIOutputPtr         m_selectedOutput;

        IDXGISwapChainPtr      m_swapChain;
        ID3D11DevicePtr        m_device;
        ID3D11DeviceContextPtr m_immediateContext;
      };

    }
  }
}

#endif
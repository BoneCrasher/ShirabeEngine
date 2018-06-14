#ifndef __SHIRABE_DIRECTX11_ENVIRONMENT_H__
#define __SHIRABE_DIRECTX11_ENVIRONMENT_H__

#include "Core/EngineStatus.h"
#include "Platform/ApplicationEnvironment.h"
#include "Log/Log.h"

#include "GFXAPI/Capabilities.h"

#include "Renderer/RendererConfiguration.h"

#include "Common.h"
#include "Linkage.h"
#include "Types.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      using Platform::ApplicationEnvironment;
      using Engine::Rendering::RendererConfiguration;
      using Engine::GFXAPI::GAPIDeviceCapabilities;
      using Engine::GFXAPI::GAPIOutputMode;

      class DX11Environment {
        DeclareLogTag(DX11Environment);

      public:
        DX11Environment();

        EEngineStatus initialize(
          ApplicationEnvironment const& applicationEnvironment,
          RendererConfiguration  const& configuration);
        EEngineStatus deinitialize();

        IDXGIFactoryPtr getDxgiFactory() const;

        ID3D11DevicePtr        getDevice()           const;
        ID3D11DeviceContextPtr getImmediateContext() const;
        
        GAPIOutputMode const& getOutputMode() const;

      private:

        EEngineStatus getDeviceCapabilities(
          Format const&, 
          GAPIDeviceCapabilities*
        );

        GAPIDeviceCapabilities m_caps;

        IDXGIFactoryPtr        m_dxgiFactory;
        IDXGIAdapterPtr        m_selectedAdapter;
        IDXGIOutputPtr         m_selectedOutput;

        ID3D11DevicePtr        m_device;
        ID3D11DeviceContextPtr m_immediateContext;
      };

    }
  }
}

#endif
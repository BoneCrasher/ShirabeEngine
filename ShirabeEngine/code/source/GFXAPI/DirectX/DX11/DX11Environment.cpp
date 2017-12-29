#include <functional>

#include "Core/String.h"
#include "Platform/Windows/WindowsError.h"

#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11Environment.h"

#include "GFXAPI/GFXAPIDeviceCapabilities.h"
#include "GFXAPI/Types/SwapChain.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Platform::Windows;
      using namespace Engine::GFXAPI;

      DX11Environment::DX11Environment()
        : m_dxgiFactory(nullptr)
        , m_selectedAdapter(nullptr)
        , m_selectedOutput(nullptr)
        , m_device(nullptr)
        , m_immediateContext(nullptr)
      {}

      EEngineStatus
        DX11Environment::initialize(
          ApplicationEnvironment const& applicationEnvironment,
          RendererConfiguration  const& configuration)
      {
        std::function<void()> fnFetchCapabilities
          = [
            this,
              &applicationEnvironment,
              &configuration] () -> void
        {
          using Engine::GFXAPI::GAPIDeviceCapabilities;
          using Engine::GFXAPI::Format;

          EEngineStatus status = EEngineStatus::Ok;
          status = getDeviceCapabilities(Format::RGBA_8_UNORM, &m_caps);
          HandleEngineStatusError(status, "Failed to retrieve device capabilities.");
        };

            std::function<void()> fnVerifyCapabilities
              = [this, &configuration] () -> void
            {
              EEngineStatus status = EEngineStatus::Ok;

              uint32_t
                adapterIndex = -1,
                outputIndex  = -1,
                modeIndex    = -1;

              // No adapters, or adapterindex > 0? --> No primary adapter. 
              // TODO: Is the effective adapter index of any importance?
              if(m_caps.adapters.empty()) {
                status = EEngineStatus::GAPI_NoPrimaryAdapter;
                HandleEngineStatusError(status, "No adapter found.");
              }

              for(uint32_t a=0; a < m_caps.adapters.size(); ++a) {
                GAPIAdapter const&adapter = m_caps.adapters[a];

                // If this is no "Output"-Adapter, return.
                if(adapter.outputs.empty()) {
                  status = EEngineStatus::GAPI_NoPrimaryAdapterOutputs;
                }

                for(uint32_t o=0; o < adapter.outputs.size(); ++o) {
                  // Loop all outputs and find an output with a valid output mode configuration!
                  GAPIOutput const&output = adapter.outputs[o];

                  if(output.outputModes.empty()) {
                    status = EEngineStatus::GAPI_NoPrimaryAdapterOutputModes;
                    // No failure createria, if there's at least another output for our desired modes.
                  }

                  for(uint32_t m=0; m < output.outputModes.size(); ++m) {
                    GAPIOutputMode const&mode = output.outputModes[m];

                    if(mode.format != Format::RGBA_8_UNORM) // Redundant test?
                      // The format is mandatory!!! 
                      continue;

                    if(mode.size.xy() == configuration.preferredBackBufferSize) {
                      modeIndex = m;
                      break;
                    }
                  }

                  outputIndex = o;
                }

                adapterIndex = a;
              }

              // Verify selection
              if(!((adapterIndex + outputIndex + modeIndex) >= 0)) {
                if(adapterIndex < 0)
                  HandleEngineStatusError(EEngineStatus::GAPI_NoPrimaryAdapter, "No valid adapters available.");
                if(outputIndex < 0)
                  HandleEngineStatusError(EEngineStatus::GAPI_NoPrimaryAdapterOutputs, String::format("No valid outputs for adapter @index %0 available.", adapterIndex));
                if(modeIndex < 0)
                  HandleEngineStatusError(EEngineStatus::GAPI_NoPrimaryAdapterOutputModes, String::format("No valid output modes for output @index %0 for adapter @index %1 available.", outputIndex, adapterIndex));
              }

              GAPIAdapterOutputConfig config={};
              config.adapterIndex = adapterIndex;
              config.outputIndex  = outputIndex;
              config.modeIndex    = modeIndex;
              m_caps.selectedConfig = config;

              // Finally fetch the selected handles so that we can easily access them.
              IDXGIAdapter *pDxgiAdapterUnmanaged = nullptr;
              IDXGIOutput  *pDxgiOutputUnmanaged  = nullptr;
              
              m_dxgiFactory->EnumAdapters(config.adapterIndex, &pDxgiAdapterUnmanaged);
              pDxgiAdapterUnmanaged->EnumOutputs(config.outputIndex, &pDxgiOutputUnmanaged);

              m_selectedAdapter = MakeDxSharedPointer(pDxgiAdapterUnmanaged);
              m_selectedOutput  = MakeDxSharedPointer(pDxgiOutputUnmanaged);         
            };

            std::function<void()> fnCreateDeviceAndImmediateContext
              = [this,
              &applicationEnvironment,
              &configuration] () -> void
            {
              HRESULT       hres   = S_OK;
              EEngineStatus status = EEngineStatus::Ok;

              // Define the desired DX11-Device feature level
              D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

              // CREATE. This will also create an ID3D11Texture2D-backbuffer for the swap-chain, which
              //         is NOT maintained by the resource-manager currently! 			
              ID3D11Device        *pDx11DeviceUnmanaged        = nullptr;
              ID3D11DeviceContext *pDx11DeviceContextUnmanaged = nullptr;

              hres = D3D11CreateDevice(
                m_selectedAdapter.get(),       // Adapter
                D3D_DRIVER_TYPE_HARDWARE,      // What kind of device to create?
                NULL,                          // Software module handle
#ifdef CONFIGURATION_DEBUG                     
                D3D11_CREATE_DEVICE_DEBUG,     // Creation flags for Debug
#else                                          
                0,                             // Creation flags for Release
#endif                                         
                &featureLevel,                 // Array of desired d3d feature levels
                1,                             // Number of feature level array
                D3D11_SDK_VERSION,             // Specific SDK version to use
                &pDx11DeviceUnmanaged,         // Device output
                NULL,                          // Feature Level output array of feature levels supported
                &pDx11DeviceContextUnmanaged); // The primary, immediate context of the DX11 device created.
              HandleWindowsError(hres, EEngineStatus::DXGI_DeviceAndSwapChainCreationFailed, "Failed to create DirectX11 device and/or immediate context.");

              m_device           = MakeDxSharedPointer<ID3D11Device>(pDx11DeviceUnmanaged);
              m_immediateContext = MakeDxSharedPointer<ID3D11DeviceContext>(pDx11DeviceContextUnmanaged);

              //// Get the swap chain.
              //static const std::size_t BACK_BUFFER_COUNT = 1;

              //SwapChain::Descriptor swapChainDesc ={};
              //swapChainDesc.name                   = "DefaultSwapChain";
              //swapChainDesc.backBufferCount        = BACK_BUFFER_COUNT;
              //swapChainDesc.texture.name           = "DefaultSwapChainTexture2D";
              //swapChainDesc.texture.textureFormat  = getSelectedMode().format;
              //swapChainDesc.texture.dimensions[0]  = getSelectedMode().size.x();
              //swapChainDesc.texture.dimensions[1]  = getSelectedMode().size.y();
              //swapChainDesc.vsyncEnabled           = configuration.enableVSync;
              //swapChainDesc.refreshRateNumerator   = getSelectedMode().refreshRate.x();
              //swapChainDesc.refreshRateDenominator = getSelectedMode().refreshRate.y();
              //swapChainDesc.fullscreen             = !configuration.requestFullscreen;
              //swapChainDesc.windowHandle           = reinterpret_cast<unsigned int>(static_cast<void *>(environment.primaryWindowHandle));

              //SwapChain::CreationRequest swapChainCreationRequest(swapChainDesc);

              //status = resourceManager->createSwapChain(swapChainDesc, swapChain);
              //if(CheckEngineError(status)) {
              //  goto _return_failed;
              //}
            };

            try {
              EEngineStatus status = EEngineStatus::Ok;

              fnFetchCapabilities();
              fnVerifyCapabilities();
              fnCreateDeviceAndImmediateContext();

              return status;
            } catch(WindowsException const we) {
              Log::Error(logTag(), we.message());
              return we.engineStatus();
            } catch(EngineException const e) {
              Log::Error(logTag(), e.message());
              return e.status();
            } catch(std::exception const stde) {
              Log::Error(logTag(), stde.what());
              return EEngineStatus::Error;
            } catch(...) {
              Log::Error(logTag(), "Unknown error occurred.");
              return EEngineStatus::Error;
            }
      }

      EEngineStatus
        DX11Environment::deinitialize()
      {
        m_immediateContext->ClearState();
        m_immediateContext = nullptr;
        m_device           = nullptr;
        m_selectedOutput   = nullptr;
        m_selectedAdapter  = nullptr;
        m_dxgiFactory      = nullptr;

        return EEngineStatus::Ok;
      }


      GAPIOutputMode const&
        DX11Environment::getOutputMode() const
      {
        return
          m_caps
          .adapters[m_caps.selectedConfig.adapterIndex]
          .outputs[m_caps.selectedConfig.outputIndex]
          .outputModes[m_caps.selectedConfig.outputIndex];
      }

      IDXGIFactoryPtr 
        DX11Environment::getDxgiFactory() const
      {
        return m_dxgiFactory;
      }

      ID3D11DevicePtr
        DX11Environment::getDevice() const
      {
        return m_device;
      }

      ID3D11DeviceContextPtr
        DX11Environment::getImmediateContext() const
      {
        return m_immediateContext;
      }


      EEngineStatus DX11Environment::getDeviceCapabilities(
        const Format       &inRequiredBackBufferFormat,
        GAPIDeviceCapabilities *pOutCapabilities) {
        if(!pOutCapabilities)
          return EEngineStatus::NullPointer;

        DXGI_FORMAT inDXRequiredBackBufferFormat = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(inRequiredBackBufferFormat);

        HRESULT res = S_OK;

        IDXGIFactoryPtr pFactory = nullptr;
        { // Try to create a DXGIFactory for device caps determination.
          IDXGIFactory *pDxgiFactoryUnmanaged  = nullptr;
          res = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pDxgiFactoryUnmanaged);
          if(FAILED(res)) {
            Log::Error(logTag(), "Unable to create DXGIFactory.");
            return EEngineStatus::DXGI_FactoryCreationFailed;
          }

          pFactory = MakeDxSharedPointer(pDxgiFactoryUnmanaged);
          // Reset to avoid side effects.
          res = S_OK;
        }

        { // Try to retrieve all adapters (graphics cards) and their output (monitors) and modes.
          GAPIAdapterList outAdapters;
          UINT k=0;
          IDXGIAdapter *pDxgiAdapterUnmanaged = nullptr;
          while((res = pFactory->EnumAdapters(k, &pDxgiAdapterUnmanaged)) != DXGI_ERROR_NOT_FOUND) {
            GAPIAdapter     outAdapter ={};
            IDXGIAdapterPtr pAdapter   = MakeDxSharedPointer(pDxgiAdapterUnmanaged);

            // Get adapter information
            {
              DXGI_ADAPTER_DESC adpaterDescription ={};
              res = pAdapter->GetDesc(&adpaterDescription);
              if(FAILED(res)) {
                Log::Error(logTag(), String::format("Unable to fetch adapter description data for adapter %0", k));
                return EEngineStatus::DXGI_AdapterGetDescFailed;
              }

              GAPIAdapterDescription outAdapterDesc ={};
              outAdapterDesc.name               = String::toNarrowString(adpaterDescription.Description);
              outAdapterDesc.dedicatedGPUMemory = adpaterDescription.DedicatedVideoMemory;
              outAdapterDesc.dedicatedSysMemory = adpaterDescription.DedicatedSystemMemory;

              outAdapter.description = outAdapterDesc;
            }

            // Enumerate outputs (monitors)
            GAPIOutputList outAdapterOutputs;
            UINT j=0;
            IDXGIOutput *pDxgiOutputUnmanaged = nullptr;
            while((res = pAdapter->EnumOutputs(j, &pDxgiOutputUnmanaged)) != DXGI_ERROR_NOT_FOUND) {
              GAPIOutput outAdapterOutput ={};

              IDXGIOutputPtr pOutput = MakeDxSharedPointer(pDxgiOutputUnmanaged);

              // Get the number of output modes that match the required backbuffer format
              do {
                UINT nOutputModes = 0;
                DXGI_MODE_DESC *outputModes = nullptr;
                res = pOutput->GetDisplayModeList(inDXRequiredBackBufferFormat, DXGI_ENUM_MODES_INTERLACED, &nOutputModes, nullptr);
                if(!FAILED(res)) {
                  // Now get 'em modes...
                  outputModes = new DXGI_MODE_DESC[nOutputModes];
                  res = pOutput->GetDisplayModeList(inDXRequiredBackBufferFormat, DXGI_ENUM_MODES_INTERLACED, &nOutputModes, outputModes);
                  if(FAILED(res)) { // Did the data change again? This is too unstable... Abort gracefully.
                    Log::Error(logTag(), String::format("Failed to determine the output modes of 'device %0|adapter %1'", k, j));

                    if(outAdapters.size() == 1)
                      // Primary adapter, which should be the main graphics card with at least one output and mode.
                      // Bail out...
                      return EEngineStatus::DXGI_PrimaryAdapterGetOutputModesFailed;
                  }

                  outAdapterOutput.outputIndex = j;

                  // Now process all modes and store them
                  for(unsigned int i=0; i < nOutputModes; ++i) {
                    GAPIOutputMode outAdapterOutputMode ={};

                    DXGI_MODE_DESC desc = outputModes[i];
                    outAdapterOutputMode.size        = Vec2Dui(desc.Width, desc.Height);
                    outAdapterOutputMode.format      = DX11DeviceCapsHelper::convertFormatDXGI2GAPI(desc.Format);
                    outAdapterOutputMode.refreshRate = Vec2Dui(desc.RefreshRate.Numerator, desc.RefreshRate.Denominator);

                    outAdapterOutput.outputModes.push_back(outAdapterOutputMode);
                  };

                  // DON'T FORGET TO CLEAN UP UNMANAGED DATA.
                  delete[] outputModes; outputModes = nullptr;
                }
              } while(res == DXGI_ERROR_MORE_DATA); // Repeat, if the mode data has changed immediately after querying the platform.

              outAdapterOutputs.push_back(outAdapterOutput);

              ++j;
            }
            // Check and report, if no output was found for the specific adapter. 
            // This however is not reason to drop out, UNLESS this is the primary adapter.
            if(FAILED(res) && outAdapterOutputs.empty()) {
              if(outAdapters.size() < 1) {
                Log::Error(logTag(), String::format("Failed to get a valid output for adapter %0.", k));
                return EEngineStatus::DXGI_PrimaryAdapterGetOutputFailed;
              }
            }

            outAdapter.outputs = outAdapterOutputs;
            outAdapters.push_back(outAdapter);

            ++k;
          }

          // Check, if we were able to fetch at least one adapter successfully.
          if(FAILED(res) && outAdapters.empty()) {
            Log::Error(logTag(), "Failed to determine a valid adapter.");
            return EEngineStatus::DXGI_FactoryGetAdapterFailed;
          }

          pOutCapabilities->adapters = outAdapters;

          // Save out the factory!
          m_dxgiFactory = pFactory;
        }

        return EEngineStatus::Ok;
      }
    }
  }
}
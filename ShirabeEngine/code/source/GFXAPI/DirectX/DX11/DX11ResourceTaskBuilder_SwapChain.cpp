#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Linkage.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

#include "Platform/Windows/WindowsError.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      EEngineStatus DX11ResourceTaskBuilder
        ::creationTask(
          SwapChain::CreationRequest   const&request,
          ResolvedDependencyCollection const&resolvedDependencies,
          ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        SwapChain::Descriptor const&desc = request.resourceDescriptor();

        DXGI_SWAP_CHAIN_DESC swapChainDescription ={};
        // Setup backbuffer
        swapChainDescription.BufferCount                 = desc.backBufferCount;
        swapChainDescription.BufferDesc.Width            = desc.texture.dimensions[0];
        swapChainDescription.BufferDesc.Height           = desc.texture.dimensions[1];
        swapChainDescription.BufferDesc.Format           = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.texture.textureFormat);
        swapChainDescription.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        if(desc.vsyncEnabled) {
          swapChainDescription.BufferDesc.RefreshRate.Numerator   = desc.refreshRateNumerator;
          swapChainDescription.BufferDesc.RefreshRate.Denominator = desc.refreshRateDenominator;
        }
        else {
          swapChainDescription.BufferDesc.RefreshRate.Numerator   = 0;
          swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
        }
        swapChainDescription.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;

        // Multisampling: For now deactivate.
        swapChainDescription.SampleDesc.Count   = 1;
        swapChainDescription.SampleDesc.Quality = 0;
        // Fullscreen?
        swapChainDescription.Windowed = true; // Do not set immediately! desc.fullscreen;
                                    // Attach to window
        swapChainDescription.OutputWindow = static_cast<HWND>(reinterpret_cast<void *>(desc.windowHandle));
        swapChainDescription.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;

        outTask = [this, &swapChainDescription] () -> GFXAPIResourceHandleAssignment
        {
          IDXGISwapChain *pSwapChainUnmanaged = nullptr;

          HRESULT hres = m_dx11Environment->getDxgiFactory()->CreateSwapChain(m_dx11Environment->getDevice().get(), &swapChainDescription, &pSwapChainUnmanaged);
          Platform::Windows::HandleWindowsError(hres, EEngineStatus::DXGI_SwapChainCreationFailed, "Failed to create swap chain resource.");

          GFXAPIResourceHandleAssignment assignment ={};
          assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(pSwapChainUnmanaged); // Just abuse the pointer target address of the handle...
          assignment.internalHandle = MakeDxSharedPointer(pSwapChainUnmanaged);

          return assignment;
        };

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::updateTask(
        SwapChain::UpdateRequest     const&request,
        ResolvedDependencyCollection const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::destructionTask(
        SwapChain::DestructionRequest const&request,
        ResolvedDependencyCollection  const&resolvedDependencies,
        ResourceTaskFn_t                   &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::queryTask(
        SwapChain::Query const&request,
        ResourceTaskFn_t      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder
        ::creationTask(
          SwapChainBuffer::CreationRequest const&request,
          ResolvedDependencyCollection     const&resolvedDependencies,
          ResourceTaskFn_t                      &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        // Based on the provided descriptor, we created "n" backbuffers. 
        // Create resource handles for them.

        SwapChainBuffer::Descriptor const&desc = request.resourceDescriptor();

        GFXAPIResourceHandle_t publicDependencyHandle  = resolvedDependencies.at(request.swapChainHandle());
        Ptr<void>              privateDependencyHandle = m_storage[publicDependencyHandle];
        if(!privateDependencyHandle) {
          m_storage.erase(publicDependencyHandle); // Remove default or inconsistent dependency.
          HandleEngineStatusError(EEngineStatus::DXDevice_CreateSwapChainBuffer_Failed, "Failed to create SRV due to missing dependency.");
        }
        
        Ptr<IDXGISwapChain> swapChain = std::static_pointer_cast<IDXGISwapChain>(privateDependencyHandle);

        DXGI_SWAP_CHAIN_DESC  swapChainDescriptor ={};
        ID3D11Texture2D      *backBufferUnmanaged = nullptr;

        outTask = [&, this] () -> GFXAPIResourceHandleAssignment
        {
          HRESULT hres = swapChain->GetBuffer(desc.backBufferIndex, __uuidof(ID3D11Texture2D), (void **)&backBufferUnmanaged);
          Platform::Windows::HandleWindowsError(hres, EEngineStatus::DXDevice_CreateSwapChainBuffer_Failed, "Failed to acquire and wrap back buffer pointer.");

          GFXAPIResourceHandleAssignment assignment ={};
          assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(backBufferUnmanaged); // Just abuse the pointer target address of the handle...
          assignment.internalHandle = MakeDxSharedPointer(backBufferUnmanaged);

          return assignment;
        };

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::updateTask(
        SwapChainBuffer::UpdateRequest     const&request,
        ResolvedDependencyCollection       const&resolvedDependencies,
        ResourceTaskFn_t                        &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::destructionTask(
        SwapChainBuffer::DestructionRequest const&request,
        ResolvedDependencyCollection        const&resolvedDependencies,
        ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::queryTask(
        SwapChainBuffer::Query const&request,
        ResourceTaskFn_t            &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}

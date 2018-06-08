#include "GFXAPI/DirectX/DX11/Common.h"
#include "GFXAPI/DirectX/DX11/Linkage.h"
#include "GFXAPI/DirectX/DX11/Types.h"
#include "GFXAPI/DirectX/DX11/DeviceCapabilities.h"
#include "GFXAPI/DirectX/DX11/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::DX::_11;

      EEngineStatus
        DX11ResourceTaskBackend::
        creationTask(
          RenderTargetView::CreationRequest   const&request,
          ResolvedDependencyCollection        const&resolvedDependencies,
          ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        RenderTargetView::Descriptor const&desc = request.resourceDescriptor();

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc ={};
        rtvDesc.Format = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.textureFormat);

        uint8_t dimensionCount = 1;
        dimensionCount += (desc.subjacentTexture.height > 1) ? 1 : 0;
        dimensionCount += (desc.subjacentTexture.depth  > 1) ? 1 : 0;

        switch(dimensionCount) {
        case 1:
          if(desc.subjacentTexture.arraySize > 1) {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.FirstArraySlice = desc.arraySlices.offset;
            rtvDesc.Texture1DArray.ArraySize       = desc.arraySlices.length;
            rtvDesc.Texture1DArray.MipSlice        = desc.mipMapSlices.offset;
          }
          else {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice = desc.mipMapSlices.offset;
          }
          break;
        case 2:
          if(desc.subjacentTexture.arraySize > 1) {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.FirstArraySlice = desc.arraySlices.offset;
            rtvDesc.Texture2DArray.ArraySize       = desc.arraySlices.length;
            rtvDesc.Texture2DArray.MipSlice        = desc.mipMapSlices.offset;
          }
          else {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = desc.mipMapSlices.offset;
          }
          break;
        case 3:
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
          rtvDesc.Texture3D.FirstWSlice    = 0;
          rtvDesc.Texture3D.WSize          = desc.subjacentTexture.depth;
          rtvDesc.Texture3D.MipSlice       = desc.mipMapSlices.offset;
          break;
        }

        outTask = [&, this] () -> GFXAPIResourceHandleAssignment
        {
          Ptr<void> privateDependencyHandle = resolvedDependencies.at(request.underlyingTextureHandle());
          if(!privateDependencyHandle) {
            HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create RTV due to missing dependency.");
          }

          Ptr<ID3D11Texture2D> underlyingTexture = std::static_pointer_cast<ID3D11Texture2D>(privateDependencyHandle);

          GFXAPIResourceHandleAssignment assignment ={};

          ID3D11RenderTargetView *pResourceUnmanaged = nullptr;
          HRESULT hres = m_dx11Environment->getDevice()->CreateRenderTargetView(underlyingTexture.get(), &rtvDesc, &pResourceUnmanaged);
          if(FAILED(hres)) {
            HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create RTV using dx11 device.");
          }
          else {
            assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(pResourceUnmanaged); // Just abuse the pointer target address of the handle...
            assignment.internalHandle = MakeDxSharedPointer(pResourceUnmanaged);
          }

          return assignment;
        };

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        updateTask(
          RenderTargetView::UpdateRequest const&request,
          ResolvedDependencyCollection    const&resolvedDependencies,
          ResourceTaskFn_t                     &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        destructionTask(
          RenderTargetView::DestructionRequest const&request,
          ResolvedDependencyCollection         const&resolvedDependencies,
          ResourceTaskFn_t                          &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus
        DX11ResourceTaskBackend::
        queryTask(
          RenderTargetView::Query const&request,
          ResourceTaskFn_t             &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}
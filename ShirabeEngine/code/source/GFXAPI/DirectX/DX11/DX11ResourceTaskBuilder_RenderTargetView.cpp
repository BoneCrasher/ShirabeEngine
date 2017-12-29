#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Linkage.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11DeviceCapabilities.h"

namespace Engine {
  namespace DX {
    namespace _11 {

      EEngineStatus DX11ResourceTaskBuilder
        ::creationTask(
          RenderTargetView::CreationRequest   const&request,
          ResolvedDependencyCollection        const&resolvedDependencies,
          ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        RenderTargetView::Descriptor const&desc = request.resourceDescriptor();

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc ={};
        rtvDesc.Format = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.textureFormat);

        switch (desc.dimensionNb) {
        case 1:
          if (desc.array.isTextureArray) {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvDesc.Texture1DArray.FirstArraySlice = desc.array.firstArraySlice;
            rtvDesc.Texture1DArray.ArraySize       = desc.array.size;
            rtvDesc.Texture1DArray.MipSlice        = 0;
          }
          else {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
            rtvDesc.Texture1D.MipSlice = 0;
          }
          break;
        case 2:
          if (desc.array.isTextureArray) {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvDesc.Texture2DArray.FirstArraySlice = 0;
            rtvDesc.Texture2DArray.ArraySize       = desc.array.size;
            rtvDesc.Texture2DArray.MipSlice        = 0;
          }
          else {
            rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvDesc.Texture2D.MipSlice = 0;
          }
          break;
        case 3:
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
          rtvDesc.Texture3D.FirstWSlice    = 0;
          rtvDesc.Texture3D.WSize          = desc.array.size;
          rtvDesc.Texture3D.MipSlice       = 0;					
          break;
        }

        outTask = [&, this] () -> GFXAPIResourceHandleAssignment
        {
          Ptr<ID3D11Texture2D> underlyingTexture = std::static_pointer_cast<ID3D11Texture2D>(resolvedDependencies.at(request.underlyingTextureHandle()));
          
          GFXAPIResourceHandleAssignment assignment ={};
          
          ID3D11RenderTargetView *pResourceUnmanaged = nullptr;
          HRESULT hres = m_dx11Environment->getDevice()->CreateRenderTargetView(underlyingTexture.get(), &rtvDesc, &pResourceUnmanaged);
          if (FAILED(hres)) {
             EEngineStatus::DXDevice_AttachSwapChainToBackBuffer_CreateRTV_Failed;
          }
          else {
            assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(pResourceUnmanaged); // Just abuse the pointer target address of the handle...
            assignment.internalHandle = MakeDxSharedPointer(pResourceUnmanaged);
          }

          return assignment;
        };

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::updateTask(
        RenderTargetView::UpdateRequest const&request,
        ResolvedDependencyCollection    const&resolvedDependencies,
        ResourceTaskFn_t                     &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::destructionTask(
        RenderTargetView::DestructionRequest const&request,
        ResolvedDependencyCollection         const&resolvedDependencies,
        ResourceTaskFn_t                          &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::queryTask(
        RenderTargetView::Query const&request,
        ResourceTaskFn_t             &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}

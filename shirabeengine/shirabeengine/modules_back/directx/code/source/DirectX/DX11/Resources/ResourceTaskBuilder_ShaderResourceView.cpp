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

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  creationTask(
      //    ShaderResourceView::CreationRequest const&request,
      //    ResolvedDependencyCollection        const&resolvedDependencies,
      //    ResourceTaskFn_t                         &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  ShaderResourceView::Descriptor const&desc = request.resourceDescriptor();

      //  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc ={};
      //  srvDesc.Format = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.format);

      //  // Map configuration to DX11 creation struct
      //  if(desc.srvType == ShaderResourceView::Descriptor::EShaderResourceDimension::Texture) {
      //    ShaderResourceViewDeclaration::Texture texture = std::get<ShaderResourceViewDeclaration::Texture>(desc.shaderResourceDimension);

      //    uint8_t dimensionCount = 1;
      //    dimensionCount += (desc.subjacentTexture.height > 1) ? 1 : 0;
      //    dimensionCount += (desc.subjacentTexture.depth  > 1) ? 1 : 0;

      //    switch(dimensionCount) {
      //    case 1:
      //      if(desc.subjacentTexture.arraySize > 1) {
      //        srvDesc.Texture1DArray.ArraySize       = texture.arraySlice.length;
      //        srvDesc.Texture1DArray.FirstArraySlice = texture.arraySlice.offset;
      //        srvDesc.Texture1DArray.MipLevels       = texture.mipSlice.length;
      //        srvDesc.Texture1DArray.MostDetailedMip = texture.mipSlice.offset;
      //      }
      //      else {
      //        srvDesc.Texture1D.MipLevels       = texture.mipSlice.length;
      //        srvDesc.Texture1D.MostDetailedMip = texture.mipSlice.offset;
      //      }

      //      break;
      //    case 2:
      //      if(desc.subjacentTexture.arraySize > 1) {
      //        srvDesc.Texture2DArray.ArraySize       = texture.arraySlice.length;
      //        srvDesc.Texture2DArray.FirstArraySlice = texture.arraySlice.offset;
      //        srvDesc.Texture2DArray.MipLevels       = texture.mipSlice.length;
      //        srvDesc.Texture2DArray.MostDetailedMip = texture.mipSlice.offset;
      //      }
      //      else {
      //        srvDesc.Texture2D.MipLevels       = texture.mipSlice.length;
      //        srvDesc.Texture2D.MostDetailedMip = texture.mipSlice.offset;
      //      }

      //      break;
      //    case 3:
      //      if(desc.subjacentTexture.arraySize % 6 == 0) {
      //        if(desc.subjacentTexture.arraySize > 1) {
      //          srvDesc.TextureCube.MipLevels       = texture.mipSlice.length;
      //          srvDesc.TextureCube.MostDetailedMip = texture.mipSlice.offset;
      //        }
      //        else {
      //          srvDesc.TextureCubeArray.NumCubes        = desc.subjacentTexture.arraySize / 6;
      //          srvDesc.TextureCubeArray.MipLevels       = texture.mipSlice.length;
      //          srvDesc.TextureCubeArray.MostDetailedMip = texture.mipSlice.offset;
      //        }
      //      }
      //      else {
      //        if(desc.subjacentTexture.arraySize > 1) {
      //          // ERROR: NO 3D Texture Arrays supported!
      //        }
      //        else {
      //          srvDesc.Texture3D.MipLevels       = texture.mipSlice.length;
      //          srvDesc.Texture3D.MostDetailedMip = texture.mipSlice.offset;
      //        }
      //      }
      //      break;
      //    default:
      //      break;
      //    }
      //  }
      //  else { // Structured Buffer
      //    ShaderResourceViewDeclaration::StructuredBuffer structuredBuffer = std::get<ShaderResourceViewDeclaration::StructuredBuffer>(desc.shaderResourceDimension);
      //    srvDesc.Buffer.ElementOffset = structuredBuffer.firstElementOffset;
      //    srvDesc.Buffer.ElementWidth  = structuredBuffer.elementWidthInBytes;
      //  }

      //  outTask = [&, this] () -> GFXAPIResourceHandleAssignment
      //  {
      //    Ptr<void> privateDependencyHandle = resolvedDependencies.at(request.underlyingBufferHandle());
      //    if(!privateDependencyHandle) {
      //      HandleEngineStatusError(EEngineStatus::DXDevice_CreateSRV_Failed, "Failed to create SRV due to missing dependency.");
      //    }

      //    Ptr<ID3D11Resource> underlyingResource = std::static_pointer_cast<ID3D11Resource>(privateDependencyHandle);

      //    GFXAPIResourceHandleAssignment assignment ={};

      //    ID3D11ShaderResourceView *pResourceUnmanaged = nullptr;
      //    HRESULT hres = m_dx11Environment->getDevice()->CreateShaderResourceView(underlyingResource.get(), &srvDesc, &pResourceUnmanaged);
      //    if(FAILED(hres)) {
      //      HandleEngineStatusError(EEngineStatus::DXDevice_CreateSRV_Failed, "Failed to create SRV in dx11 device.");
      //    }
      //    else {
      //      assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(pResourceUnmanaged); // Just abuse the pointer target address of the handle...
      //      assignment.internalHandle = MakeDxSharedPointer(pResourceUnmanaged);
      //    }

      //    return assignment;
      //  };

      //  return status;
      //}

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  updateTask(
      //    ShaderResourceView::UpdateRequest const&request,
      //    ResolvedDependencyCollection      const&resolvedDependencies,
      //    ResourceTaskFn_t                       &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  return status;
      //}

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  destructionTask(
      //    ShaderResourceView::DestructionRequest const&request,
      //    ResolvedDependencyCollection           const&resolvedDependencies,
      //    ResourceTaskFn_t                            &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  return status;
      //}

      //EEngineStatus
      //  DX11ResourceTaskBackend::
      //  queryTask(
      //    ShaderResourceView::Query const&request,
      //    ResourceTaskFn_t               &outTask)
      //{
      //  EEngineStatus status = EEngineStatus::Ok;

      //  return status;
      //}

    }
  }
}
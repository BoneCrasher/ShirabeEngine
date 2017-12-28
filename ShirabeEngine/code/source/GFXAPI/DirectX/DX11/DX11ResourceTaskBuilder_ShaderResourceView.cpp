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
          ShaderResourceView::CreationRequest const&request,
          ResolvedDependencyCollection        const&resolvedDependencies,
          ResourceTaskFn_t                         &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;
        
        ShaderResourceView::Descriptor const&desc = request.resourceDescriptor();

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc ={};
        srvDesc.Format = DX11DeviceCapsHelper::convertFormatGAPI2DXGI(desc.format);

        // Map configuration to DX11 creation struct
        if( desc.srvType == ShaderResourceView::Descriptor::EShaderResourceDimension::Texture ) {
          switch( desc.shaderResourceDimension.texture.dimensionNb ) {
          case 1:
            if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
              srvDesc.Texture1DArray.ArraySize       = desc.shaderResourceDimension.texture.array.size;
              srvDesc.Texture1DArray.FirstArraySlice = desc.shaderResourceDimension.texture.array.firstArraySlice;
              srvDesc.Texture1DArray.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
              srvDesc.Texture1DArray.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
            }
            else {
              srvDesc.Texture1D.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
              srvDesc.Texture1D.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
            }

            break;
          case 2:
            if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
              srvDesc.Texture2DArray.ArraySize       = desc.shaderResourceDimension.texture.array.size;
              srvDesc.Texture2DArray.FirstArraySlice = desc.shaderResourceDimension.texture.array.firstArraySlice;
              srvDesc.Texture2DArray.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
              srvDesc.Texture2DArray.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
            }
            else {
              srvDesc.Texture2D.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
              srvDesc.Texture2D.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
            }

            break;
          case 3:
            if( desc.shaderResourceDimension.texture.isCube ) {
              if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
                srvDesc.TextureCube.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
                srvDesc.TextureCube.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
              }
              else {
                srvDesc.TextureCubeArray.NumCubes        = desc.shaderResourceDimension.texture.array.size;
                srvDesc.TextureCubeArray.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
                srvDesc.TextureCubeArray.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
              }
            }
            else {
              if( desc.shaderResourceDimension.texture.array.isTextureArray ) {
                // ERROR: NO 3D Texture Arrays supported!
              }
              else {
                srvDesc.Texture3D.MipLevels       = desc.shaderResourceDimension.texture.mipMap.mipLevels;
                srvDesc.Texture3D.MostDetailedMip = desc.shaderResourceDimension.texture.mipMap.firstMipMapLevel;
              }
            }
            break;
          default:
            break;
          }
        }
        else { // Structured Buffer
          srvDesc.Buffer.ElementOffset = desc.shaderResourceDimension.structuredBuffer.firstElementOffset;
          srvDesc.Buffer.ElementWidth  = desc.shaderResourceDimension.structuredBuffer.elementWidthInBytes;
        }

        outTask = [=] () -> GFXAPIResourceHandleAssignment
        {
          Ptr<ID3D11Resource> underlyingResource = std::static_pointer_cast<ID3D11Resource>(resolvedDependencies.at(request.underlyingBufferHandle()));

          GFXAPIResourceHandleAssignment assignment ={};

          ID3D11ShaderResourceView *pResourceUnmanaged = nullptr;
          HRESULT hres = m_device->CreateShaderResourceView(underlyingResource.get(), &srvDesc, &pResourceUnmanaged);
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
        ShaderResourceView::UpdateRequest const&request,
        ResolvedDependencyCollection      const&resolvedDependencies,
        ResourceTaskFn_t                       &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::destructionTask(
        ShaderResourceView::DestructionRequest const&request,
        ResolvedDependencyCollection           const&resolvedDependencies,
        ResourceTaskFn_t                            &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

      EEngineStatus DX11ResourceTaskBuilder::queryTask(
        ShaderResourceView::Query const&request,
        ResourceTaskFn_t               &outTask)
      {
        EEngineStatus status = EEngineStatus::Ok;

        return status;
      }

    }
  }
}

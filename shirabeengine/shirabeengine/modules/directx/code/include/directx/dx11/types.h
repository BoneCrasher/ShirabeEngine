#ifndef __SHIRABE_DX11TYPES_H__
#define __SHIRABE_DX11TYPES_H__

#include "GFXAPI/DirectX/DX11/Common.h"
#include "GFXAPI/DirectX/DX11/Linkage.h"

#include "core/enginetypehelper.h"

#include "resources/core/eresourcetype.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;

			template <EResourceType type, EResourceSubType subtype>
			struct DetermineDXResourceType {};

			DeclareSharedPointerType(IUnknown);
      DeclareSharedPointerType(IDXGIFactory);
      DeclareSharedPointerType(IDXGIAdapter);
      DeclareSharedPointerType(IDXGIOutput);
			DeclareSharedPointerType(ID3D11Resource);
			DeclareSharedPointerType(ID3D11Device);
			DeclareSharedPointerType(ID3D11DeviceContext);
			DeclareSharedPointerType(IDXGISwapChain);
			DeclareSharedPointerType(ID3D11Texture1D);
			DeclareSharedPointerType(ID3D11Texture2D);
			DeclareSharedPointerType(ID3D11Texture3D);
			DeclareSharedPointerType(ID3D11VertexShader);
			DeclareSharedPointerType(ID3D11HullShader);
			DeclareSharedPointerType(ID3D11DomainShader);
			DeclareSharedPointerType(ID3D11GeometryShader);
			DeclareSharedPointerType(ID3D11PixelShader);
			DeclareSharedPointerType(ID3D11ComputeShader);
			DeclareSharedPointerType(ID3D11Buffer);
			DeclareSharedPointerType(ID3D11RenderTargetView);
			DeclareSharedPointerType(ID3D11ShaderResourceView);
			DeclareSharedPointerType(ID3D11DepthStencilView);
			DeclareSharedPointerType(ID3D11UnorderedAccessView);
			DeclareSharedPointerType(ID3D11DepthStencilState);
			DeclareSharedPointerType(ID3D11RasterizerState);
			DeclareSharedPointerType(ID3D11BlendState);

			DeclareListType(IUnknownPtr,                  IUnknown);
			DeclareListType(ID3D11DevicePtr,              ID3D11Device);
			DeclareListType(ID3D11DeviceContextPtr,       ID3D11DeviceContext);
			DeclareListType(IDXGISwapChainPtr,            IDXGISwapChain);
			DeclareListType(ID3D11Texture1DPtr,           ID3D11Texture1D);
			DeclareListType(ID3D11Texture2DPtr,           ID3D11Texture2D);
			DeclareListType(ID3D11Texture3DPtr,           ID3D11Texture3D);
			DeclareListType(ID3D11VertexShaderPtr,        ID3D11VertexShader);
			DeclareListType(ID3D11HullShaderPtr,          ID3D11HullShader);
			DeclareListType(ID3D11DomainShaderPtr,        ID3D11DomainShader);
			DeclareListType(ID3D11GeometryShaderPtr,      ID3D11GeometryShader);
			DeclareListType(ID3D11PixelShaderPtr,         ID3D11PixelShader);
			DeclareListType(ID3D11ComputeShaderPtr,       ID3D11ComputeShader);
			DeclareListType(ID3D11BufferPtr,              ID3D11Buffer);
			DeclareListType(ID3D11RenderTargetViewPtr,    ID3D11RenderTargetView);
			DeclareListType(ID3D11ShaderResourceViewPtr,  ID3D11ShaderResourceView);
			DeclareListType(ID3D11DepthStencilViewPtr,    ID3D11DepthStencilView);
			DeclareListType(ID3D11UnorderedAccessViewPtr, ID3D11UnorderedAccessView);
			DeclareListType(ID3D11DepthStencilStatePtr,   ID3D11DepthStencilState);
			DeclareListType(ID3D11RasterizerStatePtr,     ID3D11RasterizerState);
			DeclareListType(ID3D11BlendStatePtr,          ID3D11BlendState);

		#define DeclareDxType(_type, _subtype, _dxresource)      \
			template <>											 \
			struct DetermineDXResourceType < _type, _subtype > { \
				typedef _dxresource##Ptr type;					 \
			};

			DeclareDxType(EResourceType::UNKNOWN,        EResourceSubType::UNKNOWN,               IUnknown);
			DeclareDxType(EResourceType::GAPI_COMPONENT, EResourceSubType::DEVICE,                ID3D11Device);
			DeclareDxType(EResourceType::GAPI_COMPONENT, EResourceSubType::DEVICE_CONTEXT,        ID3D11DeviceContext);
			DeclareDxType(EResourceType::GAPI_COMPONENT, EResourceSubType::SWAP_CHAIN,            IDXGISwapChain);
			DeclareDxType(EResourceType::TEXTURE,        EResourceSubType::TEXTURE_1D,            ID3D11Texture1D);
			DeclareDxType(EResourceType::TEXTURE,        EResourceSubType::TEXTURE_2D,            ID3D11Texture2D);
			DeclareDxType(EResourceType::TEXTURE,        EResourceSubType::TEXTURE_3D,            ID3D11Texture3D);
			DeclareDxType(EResourceType::SHADER,         EResourceSubType::SHADER_VS,             ID3D11VertexShader);
			DeclareDxType(EResourceType::SHADER,         EResourceSubType::SHADER_HS,             ID3D11HullShader);
			DeclareDxType(EResourceType::SHADER,         EResourceSubType::SHADER_DS,             ID3D11DomainShader);
			DeclareDxType(EResourceType::SHADER,         EResourceSubType::SHADER_GS,             ID3D11GeometryShader);
			DeclareDxType(EResourceType::SHADER,         EResourceSubType::SHADER_PS,             ID3D11PixelShader);
			DeclareDxType(EResourceType::SHADER,         EResourceSubType::SHADER_CS,             ID3D11ComputeShader);
			DeclareDxType(EResourceType::BUFFER,         EResourceSubType::BUFFER_CONSTANT,       ID3D11Buffer);
			DeclareDxType(EResourceType::BUFFER,         EResourceSubType::BUFFER_TEXTURE,        ID3D11Buffer);
			DeclareDxType(EResourceType::BUFFER,         EResourceSubType::BUFFER_STRUCTURED,     ID3D11Buffer);
			DeclareDxType(EResourceType::BUFFER,         EResourceSubType::BUFFER_VERTEX,         ID3D11Buffer);
			DeclareDxType(EResourceType::BUFFER,         EResourceSubType::BUFFER_INDEX,          ID3D11Buffer);
			DeclareDxType(EResourceType::GAPI_VIEW,      EResourceSubType::RENDER_TARGET_VIEW,    ID3D11RenderTargetView);
			DeclareDxType(EResourceType::GAPI_VIEW,      EResourceSubType::SHADER_RESOURCE_VIEW,  ID3D11ShaderResourceView);
			DeclareDxType(EResourceType::GAPI_VIEW,      EResourceSubType::DEPTH_STENCIL_VIEW,    ID3D11DepthStencilView);
			DeclareDxType(EResourceType::GAPI_VIEW,      EResourceSubType::UNORDERED_ACCESS_VIEW, ID3D11UnorderedAccessView);
			DeclareDxType(EResourceType::GAPI_STATE,     EResourceSubType::DEPTH_STENCIL_STATE,   ID3D11DepthStencilState);
			DeclareDxType(EResourceType::GAPI_STATE,     EResourceSubType::RASTERIZER_STATE,      ID3D11RasterizerState);
			DeclareDxType(EResourceType::GAPI_STATE,     EResourceSubType::BLEND_STATE,           ID3D11BlendState);

      template <typename T>
      static Shared<T> MakeDxSharedPointer(T *pInRaw) {
        if(!pInRaw)
          return nullptr;

        return Shared<T>(pInRaw, DxResourceDeleter<T>());
      }

		}
	}
}

#endif

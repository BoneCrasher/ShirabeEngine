#ifndef __SHIRABE_DX11TYPES_H__
#define __SHIRABE_DX11TYPES_H__

#include "GAPI/DirectX/DX11/DX11Common.h"
#include "GAPI/DirectX/DX11/DX11Linkage.h"
#include "Core/EngineTypeHelper.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			DeclareSharedPointerType(ID3D11Device);
			DeclareSharedPointerType(ID3D11DeviceContext);
			DeclareSharedPointerType(IDXGISwapChain);
			DeclareSharedPointerType(ID3D11Resource);
			DeclareSharedPointerType(ID3D11Texture1D);
			DeclareSharedPointerType(ID3D11Texture2D);
			DeclareSharedPointerType(ID3D11Texture3D);
			DeclareSharedPointerType(ID3D11RenderTargetView);
			DeclareSharedPointerType(ID3D11ShaderResourceView);
			DeclareSharedPointerType(ID3D11DepthStencilState);
			DeclareSharedPointerType(ID3D11DepthStencilView);
			DeclareSharedPointerType(ID3D11RasterizerState);

			DeclareListType(ID3D11Texture2DPtr, ID3D11Texture2D);
		}
	}
}

#endif
#ifndef __SHIRABE_RESOURCES_DX11TASKBUILDER_H__
#define __SHIRABE_RESOURCES_DX11TASKBUILDER_H__

#include <map>

#include "GraphicsAPI/Resources/GFXAPIResourceTaskBackend.h"
#include "GraphicsAPI/Resources/Types/Texture.h"

#include "GFXAPI/DirectX/DX11/Common.h"
#include "GFXAPI/DirectX/DX11/Types.h"
#include "GFXAPI/DirectX/DX11/Environment.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::GFXAPI;

      /**********************************************************************************************//**
       * \def	DxTypes__
       *
       * \brief	A macro that defines all supported DirectX11 types being used!
       **************************************************************************************************/
      #define DxTypes__ 																			    		                                          /*                    */\
			  IDXGISwapChain, 																					                                        /* Components         */\
     	  ID3D11Texture1D, ID3D11Texture2D, ID3D11Texture3D, 											                          /* Texture-Types      */\
        ID3D11Buffer,                                                                                     /* Generic-Buffers    */\
			  ID3D11ShaderResourceView, ID3D11RenderTargetView, ID3D11DepthStencilView,					             		/* View-Types         */\
        ID3D11RasterizerState, ID3D11DepthStencilState, ID3D11BlendState,                                 /* State-Types        */\
			  ID3D11VertexShader, ID3D11HullShader, ID3D11DomainShader, ID3D11GeometryShader, ID3D11PixelShader /* Shader-Types       */
    }
  }

  #define DeclareTaskBuilderModule(Type)                                                                                                                  \
          EEngineStatus creationTask   (Type::CreationRequest    const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus updateTask     (Type::CUpdateRequest      const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus destructionTask(Type::CDestructionRequest const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus queryTask      (Type::Query              const&request, ResourceTaskFn_t &outTask);                                               

  namespace DX {
    namespace _11 {

      /**********************************************************************************************//**
       * \class	DX11ResourceTaskBuilder
       *
       * \brief	Glue...
       **************************************************************************************************/
      class DX11ResourceTaskBackend
        : public GFXAPIResourceTaskBackend
      {
      public:
        DX11ResourceTaskBackend(Shared<DX11Environment> const& device);

      private:
        Shared<DX11Environment> m_dx11Environment;
      };

    }
  }
}

#endif

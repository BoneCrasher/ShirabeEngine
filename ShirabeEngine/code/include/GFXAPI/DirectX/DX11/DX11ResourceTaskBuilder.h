#ifndef __SHIRABE_RESOURCES_DX11TASKBUILDER_H__
#define __SHIRABE_RESOURCES_DX11TASKBUILDER_H__

#include <map>

#include "Resources/System/Core/ResourceTask.h"
#include "Resources/System/Core/ResourceTaskBuilder.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceTaskBackend.h"

#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"
#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/DX11Environment.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;
      using namespace Engine::GFXAPI;


      /**********************************************************************************************//**
       * \struct	ResourceMapping
       *
       * \brief	A resource mapping used to map GFXAPI-model to real platform type.
       *
       * \tparam	TResource	Type of the resource.
       **************************************************************************************************/
      template <typename TResource>
      struct ResourceMapping {
      };

      template <>
      struct ResourceMapping<Texture1D> {
        typedef ID3D11Texture1D mapped_resource_type;
      };
      template <>
      struct ResourceMapping<Texture2D> {
        typedef ID3D11Texture2D mapped_resource_type;
      };
      template <>
      struct ResourceMapping<Texture3D> {
        typedef ID3D11Texture3D mapped_resource_type;
      };

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

  namespace GFXAPI {
    #define DeclareTaskBuilderImplementation(Type)                                                                                                      \
      template <>                                                                                                                                         \
      class GFXAPIResourceTaskBackendImpl<Type> {                                                                                                         \
      public:                                                                                                                                             \
          EEngineStatus creationTask   (Type::CreationRequest    const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus updateTask     (Type::UpdateRequest      const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus destructionTask(Type::DestructionRequest const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus queryTask      (Type::Query              const&request, ResourceTaskFn_t &outTask);                                               \
      };

    DeclareTaskBuilderImplementation(Texture1D);
    DeclareTaskBuilderImplementation(Texture2D);
    DeclareTaskBuilderImplementation(Texture3D);
    DeclareTaskBuilderImplementation(ShaderResourceView);
    DeclareTaskBuilderImplementation(RenderTargetView);
    DeclareTaskBuilderImplementation(DepthStencilView);
    DeclareTaskBuilderImplementation(DepthStencilState);
    DeclareTaskBuilderImplementation(RasterizerState);
    DeclareTaskBuilderImplementation(SwapChain);
    DeclareTaskBuilderImplementation(SwapChainBuffer);
  }

  namespace DX {
    namespace _11 {
      using namespace Engine::GFXAPI;

      /**********************************************************************************************//**
       * \class	DX11ResourceTaskBuilder
       *
       * \brief	Glue...
       **************************************************************************************************/
      class DX11ResourceTaskBuilder
        : public GFXAPIResourceTaskBackend<EngineTypes>
      {
      public:
        DX11ResourceTaskBuilder(Ptr<DX11Environment> const& device);

      private:
        Ptr<DX11Environment> m_dx11Environment;

        PublicToPrivateBackendResourceMap m_storage;
      };

    }
  }
}

#endif
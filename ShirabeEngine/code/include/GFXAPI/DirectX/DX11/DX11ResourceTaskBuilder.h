#ifndef __SHIRABE_RESOURCES_DX11TASKBUILDER_H__
#define __SHIRABE_RESOURCES_DX11TASKBUILDER_H__

#include "Resources/System/Core/ResourceTask.h"

#include "GFXAPI/Types/TextureND.h"
#include "GFXAPI/DirectX/DX11/Builders/BuilderBase.h"
#include "GFXAPI/DirectX/DX11/DX11Common.h"

namespace Engine {
  namespace DX {
    namespace _11 {
      using namespace Engine::Resources;

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

       /**********************************************************************************************//**
        * \class	DX11ResourceTaskBuilderImplementation
        *
        * \brief	Default task builder implementation used by the DX11ResourceTaskBuilder!!!
        *
        * \tparam	TResource	Type of the resource.
        **************************************************************************************************/
      template <typename TResource>
      class DX11ResourceTaskBuilderImplementation
        : public ITaskBuilderImplementationBase<TResource>
      {
        inline Ptr<Task> build(
          const ResourceDescriptor<GfxApiType> &descriptor,
          const EResourceTaskType              &taskType)
        {
          using BuilderType = DX11ResourceBuilderBase<TResource, typename ResourceMapping<TResource>::mapped_resource_type>;
          // using UpdaterType    = ...;
          // using DestructorType = ...;
          // using QueryType      = ...;

          Ptr<Task> task = nullptr;

          switch(taskType) {
          case EResourceTaskType::Creation:
            break;
          case EResourceTaskType::Update:
            break;
          case EResourceTaskType::Destruction:
            break;
          case EResourceTaskType::Query:
            break;
          default:
            break;
          }

          return task;
        }
      };

      /**********************************************************************************************//**
       * \class	DX11ResourceTaskBuilder
       *
       * \brief	Glue...
       **************************************************************************************************/
      class DX11ResourceTaskBuilder
        : public GenericTaskBuilder<DX11ResourceTaskBuilderImplementation, DxTypes__>
      { };

    }
  }
}

#endif
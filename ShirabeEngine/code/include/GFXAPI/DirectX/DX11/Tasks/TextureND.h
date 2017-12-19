#ifndef __SHIRABE_DX11TEXTURENDTask_H__
#define __SHIRABE_DX11TEXTURENDTask_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "GFXAPI/Types/TextureND.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/Tasks/TaskBase.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GFXAPI;

			EEngineStatus createTexture1D (
				const ID3D11DevicePtr               &device,
				const ResourceDescriptor<Texture1D> &desc,
				ID3D11Texture1DPtr                  &outTexture
			);

			EEngineStatus createTexture2D (
				const ID3D11DevicePtr               &device,
				const ResourceDescriptor<Texture2D> &desc,
				ID3D11Texture2DPtr                  &outTexture
			);

			EEngineStatus createTexture3D (
				const ID3D11DevicePtr               &device,
				const ResourceDescriptor<Texture3D> &desc,
				ID3D11Texture3DPtr                  &outTexture
			);

			/**********************************************************************************************//**
			 * \class	DX11Texture1DResourceTask
			 *
			 * \brief	A dx 11 texture 1 d resource Task.
			 **************************************************************************************************/
			class DX11Texture1DResourceTask
				: public DX11ResourceTaskBase<Texture1D, IUnknownPtr>
			{
				DeclareLogTag(Texture1DResourceTask_ID3D11Device);

			public:
				static EEngineStatus build(
					ResourceDescriptor<Texture1D> const &descriptor,
					gfxapi_parameter_struct_type         &gfxapiParams,
					built_resource_map                   &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture1DPtr pTexture = nullptr;

					status = DX::_11::createTexture1D(gfxapiParams.device, descriptor, pTexture);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create Texture1D internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						// Perform any further processing if necessary...
						ResourceHandle p(descriptor.name, resource_type, resource_subtype);

						outResources[p] = (IUnknownPtr) pTexture;
					}

					return status;
				}
			};

			/**********************************************************************************************//**
			 * \class	DX11Texture2DResourceTask
			 *
			 * \brief	A dx 11 texture 2D resource Task.
			 **************************************************************************************************/
			class DX11Texture2DResourceTask
				: public DX11ResourceTaskBase<Texture2D, IUnknownPtr>
			{
				DeclareLogTag(Texture2DResourceTask_ID3D11Device);

			public:
				static EEngineStatus build(
					ResourceDescriptor<Texture2D> const &descriptor,
					gfxapi_parameter_struct_type         &gfxapiParams,
					built_resource_map                   &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture2DPtr pTexture = nullptr;

					status = DX::_11::createTexture2D(gfxapiParams.device, descriptor, pTexture);
					if( CheckEngineError(status) )
					{
						Log::Error(logTag(), String::format("Cannot create Texture2D internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else
					{
						// Perform any further processing if necessary...
						ResourceHandle p(descriptor.name, resource_type, resource_subtype);

						outResources[p] = (IUnknownPtr)pTexture;
					}

					return status;
				}
			};

			class DX11Texture3DResourceTask
				: public DX11ResourceTaskBase<Texture3D, IUnknownPtr>
			{
				DeclareLogTag(Texture3DResourceTask_ID3D11Device);

			public:
				static EEngineStatus build(
					ResourceDescriptor<Texture3D> const &descriptor,
					gfxapi_parameter_struct_type         &gfxapiParams,
					built_resource_map                   &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture3DPtr pTexture = nullptr;

					status = DX::_11::createTexture3D(gfxapiParams.device, descriptor, pTexture);
					if( CheckEngineError(status) )
					{
						Log::Error(logTag(), String::format("Cannot create Texture2D internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else
					{
						// Perform any further processing if necessary...
						ResourceHandle p(descriptor.name, resource_type, resource_subtype);

						outResources[p] = (IUnknownPtr)pTexture;
					}

					return status;
				}
			};

			DeclareTemplatedSharedPointerType(DX11Texture1DResourceTask, DX11Texture1DResourceTask);
			DeclareTemplatedSharedPointerType(DX11Texture2DResourceTask, DX11Texture2DResourceTask);
			DeclareTemplatedSharedPointerType(DX11Texture3DResourceTask, DX11Texture3DResourceTask);

		}
	}

  namespace Resources {
    template <>
    class ResourceTask<Texture1D> {
      typedef DX11Texture1DResourceTask type;
    };

    template <>
    class ResourceTask<Texture2D> {
      typedef DX11Texture2DResourceTask type;
    };

    template <>
    class ResourceTask<Texture3D> {
      typedef DX11Texture3DResourceTask type;
    };
  }
}

#endif
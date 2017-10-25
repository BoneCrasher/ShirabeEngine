#ifndef __SHIRABE_DX11TEXTURENDBUILDER_H__
#define __SHIRABE_DX11TEXTURENDBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "GFXAPI/Types/TextureND.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/Builders/BuilderBase.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GFXAPI;

			EEngineStatus createTexture1D (
				const ID3D11DevicePtr     &device,
				const Texture1DDescriptor &desc,
				ID3D11Texture1DPtr        &outTexture
			);

			EEngineStatus createTexture2D (
				const ID3D11DevicePtr     &device,
				const Texture2DDescriptor &desc,
				ID3D11Texture2DPtr        &outTexture
			);

			EEngineStatus createTexture3D (
				const ID3D11DevicePtr     &device,
				const Texture3DDescriptor &desc,
				ID3D11Texture3DPtr        &outTexture
			);

			/**********************************************************************************************//**
			 * \class	DX11Texture1DResourceBuilder
			 *
			 * \brief	A dx 11 texture 1 d resource builder.
			 **************************************************************************************************/
			class DX11Texture1DResourceBuilder
				: public DX11ResourceBuilderBase<Texture1D, IUnknownPtr>
			{
				DeclareLogTag(Texture1DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					descriptor_type const        &descriptor,
					gfxapi_parameter_struct_type &gfxapiParams,
					built_resource_map           &outResources)
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
			 * \class	DX11Texture2DResourceBuilder
			 *
			 * \brief	A dx 11 texture 2D resource builder.
			 **************************************************************************************************/
			class DX11Texture2DResourceBuilder
				: public DX11ResourceBuilderBase<Texture2D, IUnknownPtr>
			{
				DeclareLogTag(Texture2DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					descriptor_type const        &descriptor,
					gfxapi_parameter_struct_type &gfxapiParams,
					built_resource_map           &outResources)
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

			class DX11Texture3DResourceBuilder
				: public DX11ResourceBuilderBase<Texture3D, IUnknownPtr>
			{
				DeclareLogTag(Texture3DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					descriptor_type const        &descriptor,
					gfxapi_parameter_struct_type &gfxapiParams,
					built_resource_map           &outResources)
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

			DeclareTemplatedSharedPointerType(DX11Texture1DResourceBuilder, DX11Texture1DResourceBuilder);
			DeclareTemplatedSharedPointerType(DX11Texture2DResourceBuilder, DX11Texture2DResourceBuilder);
			DeclareTemplatedSharedPointerType(DX11Texture3DResourceBuilder, DX11Texture3DResourceBuilder);
		}
	}
}

#endif
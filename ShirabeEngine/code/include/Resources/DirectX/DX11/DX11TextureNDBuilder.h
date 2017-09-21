#ifndef __SHIRABE_DX11TEXTURENDBUILDER_H__
#define __SHIRABE_DX11TEXTURENDBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceBuilder.h"
#include "Resources/Types/TextureND.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"

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
				: public ResourceBuilderBase<ID3D11DevicePtr, EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D, IUnknownPtr>
			{
				DeclareLogTag(Texture1DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					gapi_device_type      &gapiDevice,
					descriptor_type const &descriptor,
					built_resource_map    &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture1DPtr pTexture = nullptr;

					status = DX::_11::createTexture1D(gapiDevice, descriptor, pTexture);
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
				: public ResourceBuilderBase<ID3D11DevicePtr, EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D, IUnknownPtr>
			{
				DeclareLogTag(Texture2DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					IResourceManagerPtr   &resourceManager,
					gapi_device_type      &gapiDevice,
					descriptor_type const &descriptor,
					built_resource_map    &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture2DPtr pTexture = nullptr;

					status = DX::_11::createTexture2D(gapiDevice, descriptor, pTexture);
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
				: public ResourceBuilderBase<ID3D11DevicePtr, EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D, IUnknownPtr>
			{
				DeclareLogTag(Texture3DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					IResourceManagerPtr   &resourceManager,
					gapi_device_type      &gapiDevice,
					descriptor_type const &descriptor,
					built_resource_map    &outResources)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture3DPtr pTexture = nullptr;

					status = DX::_11::createTexture3D(gapiDevice, descriptor, pTexture);
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
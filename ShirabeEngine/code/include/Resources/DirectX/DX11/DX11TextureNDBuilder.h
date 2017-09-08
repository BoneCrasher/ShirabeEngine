#ifndef __SHIRABE_DX11TEXTURENDBUILDER_H__
#define __SHIRABE_DX11TEXTURENDBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/Handle.h"
#include "Resources/ResourceBuilder.h"
#include "Resources/TextureND.h"

#include "GAPI/DirectX/DX11/DX11Types.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GAPI;

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
				: public ResourceBuilderBase<ID3D11DevicePtr, EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D, ID3D11Texture1DPtr>
			{
				DeclareLogTag(Texture1DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					gapi_device_type      &gapiDevice,
					descriptor_type const &descriptor,
					resource_type_ptr     &outResource)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11Texture1DPtr pTexture = nullptr;

					status = DX::_11::createTexture1D(gapiDevice, descriptor, pTexture);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create Texture1D internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						// Perform any further processing if necessary...
						outResource = std::move(pTexture);
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
				: public ResourceBuilderBase<ID3D11Device, EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D, ID3D11Texture2DPtr>
			{
				DeclareLogTag(Texture2DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename traits_type::TGAPIDevicePtr        &gapiDevice,
					typename const traits_type::descriptor_type &descriptor,
					typename traits_type::resource_type_ptr     &outResource)
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
						outResource = std::move(pTexture);
					}

					return status;
				}
			};

			class DX11Texture3DResourceBuilder
				: public ResourceBuilderBase<ID3D11Device, EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D, ID3D11Texture3DPtr>
			{
				DeclareLogTag(Texture3DResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename traits_type::TGAPIDevicePtr        &gapiDevice,
					typename const traits_type::descriptor_type &descriptor,
					typename traits_type::resource_type_ptr     &outResource)
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
						outResource = std::move(pTexture);
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
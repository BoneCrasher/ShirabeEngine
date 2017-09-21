#ifndef __SHIRABE_DX11_SHADERRESOURCEBUILDER_H__
#define __SHIRABE_DX11_SHADERRESOURCEBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceBuilder.h"

#include "Resources/Types/ShaderResource.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;

			class DX11ShaderResourceBuilderImpl {
				friend class DX11ShaderResourceBuilder;

				static EEngineStatus createShaderResource(
					const ID3D11DevicePtr          &device,
					const ShaderResourceDescriptor &desc,
					ID3D11ResourcePtr              &inUnderlyingResource,
					ID3D11ShaderResourceViewPtr    &outRes
				);

			};


			class DX11ShaderResourceBuilder
				: public ResourceBuilderBase<ID3D11Device, EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW, ID3D11ShaderResourceViewPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename traits_type::TGAPIDevicePtr        &gapiDevice,
					typename const traits_type::descriptor_type &descriptor,
					ID3D11ResourcePtr                           &inUnderlyingResource,
					typename traits_type::resource_type_ptr     &outResource)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11ShaderResourceViewPtr   pRes = nullptr;

					status = DX::_11::DX11ShaderResourceBuilderImpl::createShaderResource(gapiDevice, descriptor, inUnderlyingResource, pRes);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						outResource = std::move(pRes);
					}

					return status;
				}
			};

			DeclareTemplatedSharedPointerType(DX11ShaderResourceBuilderImpl, DX11ShaderResourceBuilderImpl);
		}

	}
}

#endif
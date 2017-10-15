#ifndef __SHIRABE_DX11_SHADERRESOURCEBUILDER_H__
#define __SHIRABE_DX11_SHADERRESOURCEBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Types/ShaderResourceView.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/Builders/BuilderBase.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;

			class DX11ShaderResourceBuilderImpl {
				friend class DX11ShaderResourceBuilder;

				static EEngineStatus createShaderResource(
					const ID3D11DevicePtr              &device,
					const ShaderResourceViewDescriptor &desc,
					ID3D11ResourcePtr                  &inUnderlyingResource,
					ID3D11ShaderResourceViewPtr        &outRes);

			};


			class DX11ShaderResourceBuilder
				: public DX11ResourceBuilderBase<ShaderResourceView, IUnknownPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename const traits_type::descriptor_type &descriptor,
					gfxapi_parameter_struct_type                &gfxapiParams,
					built_resource_map                          &outResources,
					ID3D11ResourcePtr                           &inUnderlyingResource)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11ShaderResourceViewPtr pRes = nullptr;

					status = DX::_11::DX11ShaderResourceBuilderImpl::createShaderResource(gfxapiParams.device, descriptor, inUnderlyingResource, pRes);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						ResourceHandle p(descriptor.name, resource_type, resource_subtype);
						outResources[p] = pRes;
					}

					return status;
				}
			};

			DeclareTemplatedSharedPointerType(DX11ShaderResourceBuilderImpl, DX11ShaderResourceBuilderImpl);
		}

	}
}

#endif
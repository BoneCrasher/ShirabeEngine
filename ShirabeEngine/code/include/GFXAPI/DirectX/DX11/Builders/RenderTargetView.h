#ifndef __SHIRABE_RenderTargetViewBuilder_H__
#define __SHIRABE_RenderTargetViewBuilder_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/System/Core/Handle.h"

#include "GFXAPI/DirectX/DX11/DX11Types.h"
#include "GFXAPI/DirectX/DX11/Builders/BuilderBase.h"

#include "GFXAPI/Types/RenderTargetView.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GFXAPI;

			EEngineStatus createRenderTargetView(
				const ID3D11DevicePtr            &device,
				const RenderTargetViewDescriptor &desc,
				IUnknownPtr                      &sourceResource,
				ID3D11RenderTargetViewPtr        &outRTV
			);


			class DX11RenderTargetResourceBuilder
				: public DX11ResourceBuilderBase<RenderTargetView, IUnknownPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename const traits_type::descriptor_type &descriptor,
					gfxapi_parameter_struct_type                &gfxapiParams,
					built_resource_map                          &outResources,
					IUnknownPtr                                 &inSourceResource)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11RenderTargetViewPtr pRTV = nullptr;

					status = DX::_11::createRenderTargetView(gfxapiParams.device, descriptor, inSourceResource, pRTV);
					if( CheckEngineError(status) ) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					} else {
						// What to pass to the texND to encapsulate the internal handle and resource? How to recreated it?
						ResourceHandle p(descriptor.name, resource_type, resource_subtype);
						outResources[p] = pRTV;
					}

					return status;
				}
			};

			DeclareTemplatedSharedPointerType(DX11RenderTargetResourceBuilder, DX11RenderTargetResourceBuilder);
		}

	}
}

#endif
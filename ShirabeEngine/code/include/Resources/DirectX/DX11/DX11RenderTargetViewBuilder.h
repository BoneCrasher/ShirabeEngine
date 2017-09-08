#ifndef __SHIRABE_DX11RENDERTARGETVIEWBUILDER_H__
#define __SHIRABE_DX11RENDERTARGETVIEWBUILDER_H__

#include "Core/EngineStatus.h"
#include "Log/Log.h"

#include "Resources/Handle.h"
#include "Resources/ResourceBuilder.h"
#include "Resources/RenderTarget.h"

#include "GAPI/DirectX/DX11/DX11Types.h"

namespace Engine {
	namespace DX {
		namespace _11 {
			using namespace Engine::Resources;
			using namespace GAPI;

			EEngineStatus createRenderTargetView (
				const ID3D11DevicePtr        &device,
				const RenderTargetDescriptor &desc,
				ID3D11ResourcePtr            &sourceResource,
				ID3D11RenderTargetViewPtr    &outRTV
			);


			class DX11RenderTargetResourceBuilder
				: public ResourceBuilderBase<ID3D11Device, EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW, ID3D11RenderTargetViewPtr>
			{
				DeclareLogTag(DX11RenderTargetResourceBuilder_ID3D11Device);

			public:
				static EEngineStatus build(
					typename traits_type::TGAPIDevicePtr        &gapiDevice,
					typename const traits_type::descriptor_type &descriptor,
					typename traits_type::resource_type_ptr     &outResource,
					ID3D11ResourcePtr                           &inSourceResource)
				{
					EEngineStatus status = EEngineStatus::Ok;

					ID3D11RenderTargetViewPtr pRTV           = nullptr;

					status = DX::_11::createRenderTargetView(gapiDevice, descriptor, inSourceResource, pRTV);
					if (CheckEngineError(status)) {
						Log::Error(logTag(), String::format("Cannot create RenderTargetView internal resource for descriptor: %s", descriptor.toString().c_str()));
					}
					else {
						// What to pass to the texND to encapsulate the internal handle and resource? How to recreated it?
						outResource = std::move(pRTV);
					}

					return status;
				}
			};

			DeclareTemplatedSharedPointerType(DX11RenderTargetResourceBuilder, DX11RenderTargetResourceBuilder);
		}

	}
}

#endif
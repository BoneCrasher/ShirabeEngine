#ifndef __SHIRABE_RESOURCETYPES_RENDERTARGET_H__
#define __SHIRABE_RESOURCETYPES_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"


namespace Engine {
	namespace GFXAPI {

		/**********************************************************************************************//**
		 * \struct	RenderTargetDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		struct RenderTargetViewDescriptorImpl {
			std::string             name;
			Format                  textureFormat;
			unsigned int            dimensionNb;
			TextureArrayDescriptor  array;
			TextureMipMapDescriptor mipMap;

			RenderTargetViewDescriptorImpl()
				: name("")
				, textureFormat(Format::UNKNOWN)
				, dimensionNb(0)
			{}

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "RenderTargetDescriptor ('" << name << "'): "
					<< " Format: " << (uint8_t)textureFormat << ";";

				return ss.str();
			}
		};		

		struct RenderTargetViewResourceBinding {
			ResourceHandle           handle;
			TextureNDResourceBinding textureBinding;
			// BufferResourceBinding    bufferBinding;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class RenderTargetView
			: public ResourceDescriptorAdapter<RenderTargetView>
			, public ResourceBindingAdapter<RenderTargetView>
		{
		public:
			static const constexpr EResourceType    resource_type    = EResourceType::GAPI_COMPONENT;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::RENDER_TARGET_VIEW;

			using descriptor_impl_type = RenderTargetViewDescriptorImpl;
			using binding_type         = RenderTargetViewResourceBinding;

			using my_type = RenderTargetView;

			RenderTargetView(
				const descriptor_type &descriptor,
				const binding_type    &binding)
				: ResourceDescriptorAdapter<RenderTargetView>(descriptor)
				, ResourceBindingAdapter<RenderTargetView>(binding)
			{}
		};

		DeclareSharedPointerType(RenderTargetView);

		using RenderTargetViewDescriptor = ResourceDescriptor<RenderTargetView>;
	}
}

#endif
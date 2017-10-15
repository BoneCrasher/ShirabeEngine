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

		struct RenderTargetViewTraits {
			static const constexpr EResourceType    resource_type    = EResourceType::GAPI_COMPONENT;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::RENDER_TARGET_VIEW;

			using descriptor_impl_type = RenderTargetViewDescriptorImpl;
			using binding_type         = RenderTargetViewResourceBinding;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class RenderTargetView
			: public ResourceDescriptorAdapter<RenderTargetViewTraits>
			, public ResourceBindingAdapter<RenderTargetViewTraits>
		{
		public:
			using my_type = RenderTargetView;

			static const constexpr EResourceType    resource_type    = RenderTargetViewTraits::resource_type;
			static const constexpr EResourceSubType resource_subtype = RenderTargetViewTraits::resource_subtype;

			using descriptor_impl_type = RenderTargetViewTraits::descriptor_impl_type;

			RenderTargetView(
				const descriptor_type &descriptor,
				const binding_type    &binding)
				: ResourceDescriptorAdapter<RenderTargetViewTraits>(descriptor)
				, ResourceBindingAdapter<RenderTargetViewTraits>(binding)
			{}
		};

		DeclareSharedPointerType(RenderTargetView);

		typedef ResourceDescriptor<RenderTargetView> RenderTargetViewDescriptor;
	}
}

#endif
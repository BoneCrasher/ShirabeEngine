#ifndef __SHIRABE_RESOURCETYPES_RENDERTARGET_H__
#define __SHIRABE_RESOURCETYPES_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Definitions.h"
#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "Resources/Types/TextureND.h"

namespace Engine {
	namespace Resources {	
		using namespace GFXAPI;

		/**********************************************************************************************//**
		 * \struct	RenderTargetDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		struct RenderTargetDescriptorImpl {
			std::string             name;
			Format                  textureFormat;
			unsigned int            dimensionNb;
			TextureArrayDescriptor  array;
			TextureMipMapDescriptor mipMap;

			RenderTargetDescriptorImpl()
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

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::GAPI_VIEW,EResourceSubType::RENDER_TARGET_VIEW>
		 *
		 * \brief	A render target view>.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>
			: public RenderTargetDescriptorImpl
		{
			typedef RenderTargetDescriptorImpl type;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW> RenderTargetDescriptor;

		/**********************************************************************************************//**
		 * \class	RenderTargetDescriptorAdapterBase
		 *
		 * \brief	A render target descriptor adapter base.
		 **************************************************************************************************/
		class RenderTargetDescriptorAdapterBase {		
		public:
			typedef
				typename ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::RENDER_TARGET_VIEW>::type
				descriptor_type;

			inline RenderTargetDescriptorAdapterBase(
				const descriptor_type& descriptor
			) : _descriptor(descriptor)
			{}

			inline const descriptor_type&
				descriptor() const { return _descriptor; }

			inline const Format&
				format() const { return _descriptor.textureFormat; }

		private:
			descriptor_type _descriptor;
		};

		struct RenderTargetResourceBinding {
			ResourceHandle handle;
			unsigned int   index;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class RenderTarget
			: public RenderTargetDescriptorAdapterBase
		{
		public:
			using my_type = RenderTarget;

			RenderTarget(
				const descriptor_type        &descriptor)
				: RenderTargetDescriptorAdapterBase(descriptor)
			{}
			
		private:
			ResourceHandle _underlyingTexture; // Reference to the underlying texture resource used for the rendertarget.
		};

		DeclareSharedPointerType(RenderTarget);
	}
}

#endif
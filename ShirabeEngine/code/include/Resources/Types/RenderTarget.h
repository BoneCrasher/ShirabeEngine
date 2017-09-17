#ifndef __SHIRABE_RESOURCETYPES_RENDERTARGET_H__
#define __SHIRABE_RESOURCETYPES_RENDERTARGET_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"

#include "Resources/System/GFXAPI/Definitions.h"
#include "Resources/System/GFXAPI/GFXAPI.h"

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
			std::string             _name;
			Format                  _textureFormat;
			unsigned int            _dimensions;
			TextureArrayDescriptor  _array;
			TextureMipMapDescriptor _mipMap;

			RenderTargetDescriptorImpl()
				: _name("")
				, _textureFormat(Format::UNKNOWN)
				, _dimensions(0)
			{}

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "RenderTargetDescriptor ('" << _name << "'): "
					<< " Format: " << (uint8_t)_textureFormat << ";";

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
				format() const { return _descriptor._textureFormat; }

		private:
			descriptor_type _descriptor;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class GFXAPIRenderTarget
			: public RenderTargetDescriptorAdapterBase
			, public GFXAPIResourceAdapter
		{
		public:
			using my_type = GFXAPIRenderTarget;

			GFXAPIRenderTarget(
				const descriptor_type        &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: RenderTargetDescriptorAdapterBase(descriptor)
				, GFXAPIResourceAdapter(platformResourceHandle)
			{}
			
		private:
			ResourceHandle _underlyingTexture; // Reference to the underlying texture resource used for the rendertarget.
		};

		DeclareSharedPointerType(GFXAPIRenderTarget);
	}
}

#endif
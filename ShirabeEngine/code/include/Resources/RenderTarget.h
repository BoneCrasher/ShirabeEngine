#ifndef __SHIRABE_RENDERTARGET_H__
#define __SHIRABE_RENDERTARGET_H__

#include "Resources/EResourceType.h"
#include "Resources/ResourceDescriptors.h"
#include "Resources/IResource.h"
#include "Resources/GFXAPI.h"

namespace Engine {
	namespace Resources {	
		using namespace GFXAPI;

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

		class GAPIRenderTarget
			: public RenderTargetDescriptorAdapterBase
			, public GFXAPIResourceAdapter
		{
		public:
			using my_type = GAPIRenderTarget;

			GAPIRenderTarget(
				const descriptor_type          &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: RenderTargetDescriptorAdapterBase(descriptor)
				, GFXAPIResourceAdapter(platformResourceHandle)
			{}
			
		private:
			ResourceHandle _sourceTexture;
		};

		DeclareSharedPointerType(GAPIRenderTarget);
	}
}

#endif
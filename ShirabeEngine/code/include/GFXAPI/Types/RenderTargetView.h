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

    class RenderTargetView;

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
    
    struct RenderTargetViewCreationRequestImpl {
    public:
      inline const RenderTargetViewDescriptorImpl& resourceDescriptor() const { return _resourceDescriptor; }

      std::string toString() const {
        std::stringstream ss;

        ss
          << "RenderTargetViewCreationRequest: \n"
          << "[\n"
          << _resourceDescriptor.toString() << "\n"
          << "]"
          << std::endl;

        return ss.str();
      }
    private:
      RenderTargetViewDescriptorImpl _resourceDescriptor;
    };


    struct RenderTargetViewDestructionRequestImpl {

    };

    struct RenderTargetViewQueryRequestImpl {

    };

    struct RenderTargetViewResourceBinding {
      ResourceHandle           handle;
      TextureNDResourceBinding textureBinding;
      // BufferResourceBinding    bufferBinding;
    };

    DeclareResourceTraits(RenderTargetView,
                          RenderTargetView,
                          EResourceType::GAPI_VIEW,
                          EResourceSubType::RENDER_TARGET_VIEW,
                          RenderTargetViewResourceBinding,
                          RenderTargetViewDescriptorImpl,
                          RenderTargetViewCreationRequestImpl,
                          void,
                          RenderTargetViewQueryRequestImpl,
                          RenderTargetViewDestructionRequestImpl);

    DefineTraitsPublicTypes(RenderTargetView, RenderTargetViewTraits);

		/**********************************************************************************************//**
		 * \class	GFXAPIRenderTarget
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class RenderTargetView
			: public RenderTargetViewTraits
      , public ResourceDescriptorAdapter<RenderTargetViewTraits>
			, public ResourceBindingAdapter<RenderTargetViewTraits>
		{
		public:
			using my_type = RenderTargetView;

			RenderTargetView(
				const RenderTargetViewDescriptor &descriptor,
				const RenderTargetViewBinding    &binding)
				: RenderTargetViewTraits()
        , ResourceDescriptorAdapter<RenderTargetViewTraits>(descriptor)
				, ResourceBindingAdapter<RenderTargetViewTraits>(binding)
			{}
		};

		DeclareSharedPointerType(RenderTargetView);

		typedef ResourceDescriptor<RenderTargetView> RenderTargetViewDescriptor;
	}
}

#endif
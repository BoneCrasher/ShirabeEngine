#ifndef __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__
#define __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace Resources {
		using namespace GFXAPI;

    class ShaderResourceView;

		/**********************************************************************************************//**
		 * \struct	ShaderResourceDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		struct ShaderResourceViewDescriptorImpl {
			enum class EShaderResourceDimension {
				Texture,
				StructuredBuffer
			};

			std::string              name;
			Format                   format;
			EShaderResourceDimension srvType;

			union ShaderResourceDimension {
				struct Texture {
					unsigned int            dimensionNb; // 1..3
				    VecND<uint32_t, 3>      dimensions;
					bool                    isCube; // Implies the dimensions[2] to be 6
					TextureArrayDescriptor  array;
					TextureMipMapDescriptor mipMap;

					inline Texture()
						: dimensionNb(0)
						, dimensions({ 0, 0, 0 })
						, isCube(false)
						, array()
						, mipMap()
					{}
				} texture;

				struct StructuredBuffer {
					unsigned int firstElementOffset;
					unsigned int elementWidthInBytes;

					inline StructuredBuffer()
						: firstElementOffset(0)
						, elementWidthInBytes(0)
					{}
				} structuredBuffer;

				inline ShaderResourceDimension()
					: texture()
					, structuredBuffer()
				{}
			} shaderResourceDimension;

			ShaderResourceViewDescriptorImpl()
				: name("")
				, shaderResourceDimension()
			{}

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "RenderTargetDescriptor ('" << name << "'): \n"
					<< " Format:     " << (uint8_t)format << "\n,";

				if( srvType == EShaderResourceDimension::Texture ) {
					ss 
						<< " Dimensions:        " << (uint8_t)shaderResourceDimension.texture.dimensionNb              << "\n,"
						<< " Array:             " << (uint8_t)shaderResourceDimension.texture.array.size               << "\n,"
						<< " First array index: " << (uint8_t)shaderResourceDimension.texture.array.firstArraySlice    << "\n,"
						<< " MipMap:            " << (uint8_t)shaderResourceDimension.texture.mipMap.mipLevels         << "\n,"
						<< " Most Detailed MIP: " << (uint8_t)shaderResourceDimension.texture.mipMap.firstMipMapLevel  << ";";
				}
				else { // StructuredBuffer
					ss
						<< " First elem. off.: " << (uint8_t)shaderResourceDimension.structuredBuffer.firstElementOffset << ","
						<< " Elem. byte-size:  " << (uint8_t)shaderResourceDimension.structuredBuffer.elementWidthInBytes << ";";
				}

				return ss.str();
			}
		};

    struct ShaderResourceViewCreationRequestImpl {
    public:
      const struct ResourceDescriptor<ShaderResourceView>& resourceDescriptor() const; 

      std::string toString() const {
        std::stringstream ss;

        ss
          << "ShaderResourceViewCreationRequest: \n"
          << "[\n"
          << _resourceDescriptor.toString() << "\n"
          << "]"
          << std::endl;

        return ss.str();
      }
    private:
      ShaderResourceViewDescriptorImpl _resourceDescriptor;
    };


    struct ShaderResourceViewDestructionRequestImpl {

    };

    struct ShaderResourceViewQueryRequestImpl {

    };

		struct ShaderResourceViewResourceBinding {
			ResourceHandle handle;
		};

    DeclareResourceTraits(ShaderResourceView,
                          ShaderResourceView,
                          EResourceType::GAPI_VIEW,
                          EResourceSubType::SHADER_RESOURCE_VIEW,
                          ShaderResourceViewResourceBinding,
                          ShaderResourceViewDescriptorImpl,
                          ShaderResourceViewCreationRequestImpl,
                          void,
                          ShaderResourceViewQueryRequestImpl,
                          ShaderResourceViewDestructionRequestImpl);

    DefineTraitsPublicTypes(ShaderResourceView, ShaderResourceViewTraits);

		/**********************************************************************************************//**
		 * \class	GFXAPIShaderResource
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class ShaderResourceView
			: public ShaderResourceViewTraits
      , public ResourceDescriptorAdapter<ShaderResourceViewTraits>
			, public ResourceBindingAdapter<ShaderResourceViewTraits>
		{
		public:
			using my_type = ShaderResourceView;

			ShaderResourceView(
				const ShaderResourceViewDescriptor &descriptor,
				const ShaderResourceViewBinding    &binding)
				: ShaderResourceViewTraits()
        , ResourceDescriptorAdapter<ShaderResourceViewTraits>(descriptor)
				, ResourceBindingAdapter<ShaderResourceViewTraits>(binding)
			{}
		};

		DeclareSharedPointerType(ShaderResourceView);

		using ShaderResourceViewDescriptor = ResourceDescriptor<ShaderResourceView>;

    const ResourceDescriptor<ShaderResourceView>& ShaderResourceViewCreationRequestImpl
      ::resourceDescriptor() const
    {
      return ResourceDescriptor<ShaderResourceView>(_resourceDescriptor);
    }
	}
}

#endif


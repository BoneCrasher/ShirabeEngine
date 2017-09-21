#ifndef __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__
#define __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"

#include "GFXAPI/Definitions.h"
#include "Resources/System/GFXAPI/GFXAPI.h"

#include "Resources/Types/TextureND.h"

namespace Engine {
	namespace Resources {
		using namespace GFXAPI;

		/**********************************************************************************************//**
		 * \struct	ShaderResourceDescriptorImpl
		 *
		 * \brief	A render target descriptor implementation.
		 **************************************************************************************************/
		struct ShaderResourceDescriptorImpl {
			enum class ShaderResourceDimension {
				Texture,
				StructuredBuffer
			};

			std::string             name;
			Format                  format;
			ShaderResourceDimension srvType;

			union {
				struct {
					unsigned int            dimensionNb; // 1..3
					unsigned int            dimensions[3];
					bool                    isCube; // Implies the dimensions[2] to be 6
					TextureArrayDescriptor  array;
					TextureMipMapDescriptor mipMap;
				} texture;

				struct {
					unsigned int firstElementOffset;
					unsigned int elementWidthInBytes;
				} structuredBuffer;

			} shaderResourceDimension;

			ShaderResourceDescriptorImpl()
				: name("")
				, shaderResourceDimension()
			{}

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "RenderTargetDescriptor ('" << name << "'): \n"
					<< " Format:     " << (uint8_t)format << "\n,";

				if( srvType == ShaderResourceDimension::Texture ) {
					ss 
						<< " Dimensions:        " << (uint8_t)shaderResourceDimension.texture.dimensionNb               << "\n,"
						<< " Array:             " << (uint8_t)shaderResourceDimension.texture.array.size       << "\n,"
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

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::GAPI_VIEW,EResourceSubType::RENDER_TARGET_VIEW>
		 *
		 * \brief	A render target view>.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>
			: public ShaderResourceDescriptorImpl
		{
			typedef ShaderResourceDescriptorImpl type;
		};
		typedef ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW> ShaderResourceDescriptor;

		/**********************************************************************************************//**
		 * \class	ShaderResourceDescriptorAdapterBase
		 *
		 * \brief	A render target descriptor adapter base.
		 **************************************************************************************************/
		class ShaderResourceDescriptorAdapterBase {
		public:
			typedef
				typename ResourceDescriptor<EResourceType::GAPI_VIEW, EResourceSubType::SHADER_RESOURCE_VIEW>::type
				descriptor_type;

			inline ShaderResourceDescriptorAdapterBase(
				const descriptor_type& descriptor
			) : _descriptor(descriptor)
			{}

			inline const descriptor_type&
				descriptor() const { return _descriptor; }

			inline const Format&
				format() const { return _descriptor.shaderResourceDimension.texture.textureFormat; }

		private:
			descriptor_type _descriptor;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIShaderResource
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class GFXAPIShaderResource
			: public ShaderResourceDescriptorAdapterBase
			, public GFXAPIResourceAdapter
		{
		public:
			using my_type = GFXAPIShaderResource;

			GFXAPIShaderResource(
				const descriptor_type        &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: ShaderResourceDescriptorAdapterBase(descriptor)
				, GFXAPIResourceAdapter(platformResourceHandle)
			{}

		private:
			ResourceHandle _underlyingTexture; // Reference to the underlying texture resource used for the rendertarget.
		};

		DeclareSharedPointerType(GFXAPIShaderResource);


	}
}

#endif
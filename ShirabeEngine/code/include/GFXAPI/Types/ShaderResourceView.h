#ifndef __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__
#define __SHIRABE_RESOURCETYPES_SHADERRESOURCE_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
#include "Resources/System/Core/IResourceDescriptor.h"
#include "Resources/System/Core/ResourceBinding.h"

#include "GFXAPI/Definitions.h"
#include "GFXAPI/Types/TextureND.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace Resources {
		using namespace GFXAPI;

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

		

		struct ShaderResourceViewResourceBinding {
			ResourceHandle handle;
		};

		/**********************************************************************************************//**
		 * \class	GFXAPIShaderResource
		 *
		 * \brief	A gfxapi render target.
		 **************************************************************************************************/
		class ShaderResourceView
			: public ResourceDescriptorAdapter<ShaderResourceView>
			, public ResourceBindingAdapter<ShaderResourceView>
		{
		public:
			static const constexpr EResourceType    resource_type    = EResourceType::GAPI_COMPONENT;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::SHADER_RESOURCE_VIEW;

			using descriptor_impl_type = ShaderResourceViewDescriptorImpl;
			using binding_type         = ShaderResourceViewResourceBinding;

			using my_type = ShaderResourceView;

			ShaderResourceView(
				const descriptor_type &descriptor,
				const binding_type    &binding)
				: ResourceDescriptorAdapter<ShaderResourceView>(descriptor)
				, ResourceBindingAdapter<ShaderResourceView>(binding)
			{}
		};

		DeclareSharedPointerType(ShaderResourceView);

		using ShaderResourceViewDescriptor = ResourceDescriptor<ShaderResourceView>;
	}
}

#endif


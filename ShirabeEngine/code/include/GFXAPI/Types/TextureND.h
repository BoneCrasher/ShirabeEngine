#ifndef __SHIRABE_TEXTUREND_H__
#define __SHIRABE_TEXTUREND_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/IResourceDescriptor.h"
#include "Resources/System/Core/ResourceBinding.h"

#include "GFXAPI/Definitions.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
	namespace Resources {
		using namespace GFXAPI;

		/**********************************************************************************************//**
		 * \struct	TextureMipMapDescriptor
		 *
		 * \brief	A texture mip map descriptor.
		 **************************************************************************************************/
		struct TextureMipMapDescriptor {
			bool    useMipMaps;
			uint8_t mipLevels;
			uint8_t firstMipMapLevel;
		};

		/**********************************************************************************************//**
		 * \struct	TextureArrayDescriptor
		 *
		 * \brief	A texture array descriptor.
		 **************************************************************************************************/
		struct TextureArrayDescriptor {
			bool    isTextureArray;
			uint8_t size;
			uint8_t firstArraySlice;
		};

		/**********************************************************************************************//**
		 * \struct	TextureMultisapmlingDescriptor
		 *
		 * \brief	A texture multisapmling descriptor.
		 **************************************************************************************************/
		struct TextureMultisapmlingDescriptor {
			bool    useMultisampling;
			uint8_t size;
			uint8_t quality;
		};

		/**********************************************************************************************//**
		 * \struct	TextureDescriptorImpl
		 *
		 * \brief	A texture descriptor implementation.
		 *
		 * \tparam	N	Type of the n.
		 **************************************************************************************************/
		template <uint8_t N>
		struct TextureDescriptorImpl {
			static const uint8_t dimensionNb = N;

			std::string                    name;
			Format                         textureFormat;
			VecND<uint32_t, N>             dimensions;
			TextureMipMapDescriptor        mipMap;
			TextureArrayDescriptor         array;
			TextureMultisapmlingDescriptor multisampling;
			ResourceUsage                  cpuGpuUsage;
			BufferBindingFlags_t           gpuBinding;

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "TextureDescriptor<" << N << ">"
					<< " ('" << name << "'): "
					<< " Dimensions: "      << N << ", "
					<< " Format: "          << (uint8_t)textureFormat << ", "
					<< " MipMaps: "         << (mipMap.useMipMaps ? "true" : "false") << "; Levels: " << mipMap.mipLevels << ", "
					<< " Array: "           << (array.isTextureArray ? "true" : "false") << "; Layers: " << array.size << ", "
					<< " Multisampling: "   << (multisampling.useMultisampling ? "true" : "false")
					<< "; Count/Quality:  " << multisampling.size << "/" << multisampling.quality << ", "
					<< " CPU-GPU-Usage: "   << (uint8_t)cpuGpuUsage << ", "
					<< " GPU-Binding:  "    << (uint8_t)gpuBinding << ";";

				return ss.str();
			}
		};

		template <uint8_t N>
		struct TextureNDCreationRequestImpl {
		public:
			inline const TextureDescriptorImpl<N>& resourceDescriptor() const { return _resourceDescriptor; }

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "TextureNDCreationRequest<" << N << ">: \n"
					<< "[\n"
					<< _resourceDescriptor.toString() << "\n"
					<< "]"
					<< std::endl;

				return ss.str();
			}
		private:
			TextureDescriptorImpl<N> _resourceDescriptor;
		};

		template <uint8_t N>
		struct TextureNDDestructionRequestImpl {

		};

		template <uint8_t N>
		struct TextureNDUpdateRequestImpl {

		};

		template <uint8_t N>
		struct TextureNDQueryRequestImpl {

		};

		/**********************************************************************************************//**
		 * \fn	template <uint8_t N> std::ostream& operator<<(std::ostream& s, const TextureDescriptorImpl<N>& d)
		 *
		 * \brief	Cast that converts the given std::ostream&amp; to a &lt;&lt;&lt;uint8_t N&gt;
		 *
		 * \param [in,out]	s	A std::ostream to process.
		 * \param 		  	d	A TextureDescriptorImpl&lt;N&gt; to process.
		 *
		 * \return	The result of the operation.
		 **************************************************************************************************/
		template <uint8_t N>
		std::ostream& operator<<(std::ostream& s, const TextureDescriptorImpl<N>& d) {
			return (s << d.toString());
		}
		
		/**********************************************************************************************//**
		 * \struct	TextureResourceSubtypeMapper
		 *
		 * \brief	A texture resource subtype mapper.
		 *
		 * \tparam	N	Type of the n.
		 **************************************************************************************************/
		template <uint8_t N>
		struct TextureResourceSubtypeMapper {
		};

		/**********************************************************************************************//**
		 * \struct	TextureResourceSubtypeMapper<1>
		 *
		 * \brief	A texture resource subtype mapper.
		 **************************************************************************************************/
		template <>
		struct TextureResourceSubtypeMapper<1> {
			static constexpr const EResourceSubType value = EResourceSubType::TEXTURE_1D;
		};

		/**********************************************************************************************//**
		 * \struct	TextureResourceSubtypeMapper<2>
		 *
		 * \brief	A texture resource subtype mapper.
		 **************************************************************************************************/
		template <>
		struct TextureResourceSubtypeMapper<2> {
			static constexpr const EResourceSubType value = EResourceSubType::TEXTURE_2D;
		};

		/**********************************************************************************************//**
		 * \struct	TextureResourceSubtypeMapper<3>
		 *
		 * \brief	A texture resource subtype mapper.
		 **************************************************************************************************/
		template <>
		struct TextureResourceSubtypeMapper<3> {
			static constexpr const EResourceSubType value = EResourceSubType::TEXTURE_3D;
		};


		struct TextureNDResourceBinding {
			ResourceHandle handle;
			ResourceHandle srvBinding;
			ResourceHandle rtvBinding;
			ResourceHandle dsvBinding;
		};

		template <uint8_t N>
		struct TextureNDTraits {
			static const constexpr EResourceType    resource_type    = EResourceType::TEXTURE;
			static const constexpr EResourceSubType resource_subtype = EResourceSubType::TEXTURE_1D;

			using binding_type                  = TextureNDResourceBinding;
			using descriptor_impl_type          = TextureDescriptorImpl<N>;
			using creation_request_impl_type    = TextureNDCreationRequestImpl<N>;
			// using update_request_impl_type   = TextureNDUpdateRequestImpl<N>;
			using query_request_impl_type       = TextureNDQueryRequestImpl<N>;
			using destruction_request_impl_type = TextureNDDestructionRequestImpl<N>;
		};

		/**********************************************************************************************//**
		 * \class	TextureNDBase
		 *
		 * \brief	A texture nd base.
		 *
		 * \tparam	N	Type of the n.
		 **************************************************************************************************/
		template <uint8_t N>
		class TextureNDBase
			: public ResourceDescriptorAdapter<TextureNDTraits<N>>
			, public ResourceBindingAdapter<TextureNDTraits<N>>
		{
		public:
			using my_type = TextureNDBase<N>;
			static const constexpr EResourceType    resource_type    = TextureNDTraits<N>::resource_type;
			static const constexpr EResourceSubType resource_subtype = TextureNDTraits<N>::resource_subtype;
			
			using descriptor_impl_type          = typename TextureNDTraits<N>::descriptor_impl_type;
			using creation_request_impl_type    = typename TextureNDTraits<N>::creation_request_impl_type;
			using update_request_impl_type	    = typename TextureNDTraits<N>::update_request_impl_type;
			using query_request_impl_type	    = typename TextureNDTraits<N>::query_request_impl_type;
			using destruction_request_impl_type = typename TextureNDTraits<N>::destruction_request_impl_type;
			
				TextureNDBase(
				const descriptor_type &descriptor,
				const binding_type    &binding) 
				: ResourceDescriptorAdapter<TextureNDTraits<N>>(descriptor)
				, ResourceBindingAdapter<TextureNDTraits<N>>(binding)
			{}

		private:
		};

		/**********************************************************************************************//**
		 * \class	GAPITexture1D
		 *
		 * \brief	A gapi texture 1 d.
		 **************************************************************************************************/
		class Texture1D
			: public TextureNDBase<1>
		{
		public:
			inline Texture1D(
				const descriptor_type &descriptor,
				const binding_type    &binding)
				: TextureNDBase<1>(descriptor, binding) 
			{}
		};

		/**********************************************************************************************//**
		 * \class	GAPITexture2D
		 *
		 * \brief	A gapi texture 2d.
		 **************************************************************************************************/
		class Texture2D
			: public TextureNDBase<2>
		{
		public:
			inline Texture2D(
				const descriptor_type &descriptor,
				const binding_type    &binding)
				: TextureNDBase<2>(descriptor, binding)
			{}
		};

		/**********************************************************************************************//**
		 * \class	GAPITexture3D
		 *
		 * \brief	A gapi texture 3d.
		 **************************************************************************************************/
		class Texture3D
			: public TextureNDBase<3>
		{
		public:
			inline Texture3D(
				const descriptor_type &descriptor,
				const binding_type    &binding)
				: TextureNDBase<3>(descriptor, binding)
			{}
		};

		DeclareSharedPointerType(Texture1D);
		DeclareSharedPointerType(Texture2D);
		DeclareSharedPointerType(Texture3D);

		typedef ResourceDescriptor<Texture1D> Texture1DDescriptor;
		typedef ResourceDescriptor<Texture2D> Texture2DDescriptor;
		typedef ResourceDescriptor<Texture3D> Texture3DDescriptor;
	}
}

#endif
#ifndef __SHIRABE_TEXTUREND_H__
#define __SHIRABE_TEXTUREND_H__

#include "Resources/ResourceDescriptors.h"
#include "Resources/IResource.h"
#include "Resources/GFXAPI.h"

namespace Engine {
	namespace Resources {
		using namespace GAPI;
		using namespace GFXAPI;

		template <uint8_t N>
		struct TextureResourceSubtypeMapper {
		};

		template <>
		struct TextureResourceSubtypeMapper<1> {
			static constexpr const EResourceSubType value = EResourceSubType::TEXTURE_1D;
		};

		template <>
		struct TextureResourceSubtypeMapper<2> {
			static constexpr const EResourceSubType value = EResourceSubType::TEXTURE_2D;
		};

		template <>
		struct TextureResourceSubtypeMapper<3> {
			static constexpr const EResourceSubType value = EResourceSubType::TEXTURE_3D;
		};

		/**********************************************************************************************//**
		 * \class	TextureDescriptorAdapterBase
		 *
		 * \brief	A texture descriptor adapter base.
		 *
		 * \tparam	N	Type of the n.
		 **************************************************************************************************/
		template <uint8_t N>
		class TextureDescriptorAdapterBase {		

			template <uint8_t N, uint8_t Condition>
			using has_dimension_t = typename std::enable_if<N >= Condition, uint32_t>::type;

		public:
			typedef
				typename ResourceDescriptor<EResourceType::TEXTURE, TextureResourceSubtypeMapper<N>::value>::type
				descriptor_type;

			inline TextureDescriptorAdapterBase(
				const descriptor_type& descriptor
			) : _descriptor(descriptor)
			{}

			inline const descriptor_type&
				descriptor() const { return _descriptor; }

			inline const std::string& 
				name() const { return _descriptor._name; }

			inline const Format&
				format() const { return _descriptor._textureFormat; }

			inline const VecND<uint32_t, descriptor_type::Dimensions>&
				dimensions() const { return _descriptor._dimensions; }

			inline const TextureMipMapDescriptor&
				mipMap() const { return _descriptor._mipMap; }

			inline const TextureArrayDescriptor&
				texArray() const { return _descriptor._array; }

			inline const TextureMultisapmlingDescriptor&
				multiSampling() const { return _descriptor._multisampling; }

			inline const ResourceUsage&
				usage() const { return _descriptor._cpGpuUsage; }

			inline const BufferBindingFlags_t&
				binding() const { return _descriptor._gpuBinding; }

			inline const uint32_t width()  const { return (N > 0 ? _descriptor._dimensions[0] : 0); }
			inline const uint32_t height() const { return (N > 1 ? _descriptor._dimensions[1] : 0); }
			inline const uint32_t depth()  const { return (N > 2 ? _descriptor._dimensions[2] : 0); }

		private:
			descriptor_type _descriptor;
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
			: public TextureDescriptorAdapterBase<N>
			, public GFXAPIResourceAdapter
		{
		public:
			using my_type = TextureNDBase<N>;

			TextureNDBase(
				const descriptor_type          &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle
			) 
				: TextureDescriptorAdapterBase<N>(descriptor)
				, GFXAPIResourceAdapter(platformResourceHandle)
			{}

		private:
		};

		/**********************************************************************************************//**
		 * \class	GAPITexture1D
		 *
		 * \brief	A gapi texture 1 d.
		 **************************************************************************************************/
		class GAPITexture1D
			: public TextureNDBase<1>
		{
		public:
			inline GAPITexture1D(
				const descriptor_type        &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: TextureNDBase<1>(descriptor, platformResourceHandle) 
			{}
		};

		/**********************************************************************************************//**
		 * \class	GAPITexture2D
		 *
		 * \brief	A gapi texture 2d.
		 **************************************************************************************************/
		class GAPITexture2D
			: public TextureNDBase<2>
		{
		public:
			inline GAPITexture2D(
				const descriptor_type          &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: TextureNDBase<2>(descriptor, platformResourceHandle)
			{}
		};

		/**********************************************************************************************//**
		 * \class	GAPITexture3D
		 *
		 * \brief	A gapi texture 3d.
		 **************************************************************************************************/
		class GAPITexture3D
			: public TextureNDBase<3>
		{
		public:
			inline GAPITexture3D(
				const descriptor_type          &descriptor,
				const GFXAPIResourceHandle_t &platformResourceHandle)
				: TextureNDBase<3>(descriptor, platformResourceHandle)
			{}
		};

		DeclareSharedPointerType(GAPITexture1D);
		DeclareSharedPointerType(GAPITexture2D);
		DeclareSharedPointerType(GAPITexture3D);
	}
}

#endif
#ifndef __SHIRABE_TEXTUREND_H__
#define __SHIRABE_TEXTUREND_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/GFXAPI/GFXAPI.h"
#include "Resources/System/GFXAPI/Definitions.h"

namespace Engine {
	namespace Resources {
		using namespace GAPI;
		using namespace GFXAPI;

		/**********************************************************************************************//**
		 * \struct	TextureMipMapDescriptor
		 *
		 * \brief	A texture mip map descriptor.
		 **************************************************************************************************/
		struct TextureMipMapDescriptor {
			bool    _useMipMaps;
			uint8_t _mipLevels;
		};

		/**********************************************************************************************//**
		 * \struct	TextureArrayDescriptor
		 *
		 * \brief	A texture array descriptor.
		 **************************************************************************************************/
		struct TextureArrayDescriptor {
			bool    _isTextureArray;
			uint8_t _textureCount;
		};

		/**********************************************************************************************//**
		 * \struct	TextureMultisapmlingDescriptor
		 *
		 * \brief	A texture multisapmling descriptor.
		 **************************************************************************************************/
		struct TextureMultisapmlingDescriptor {
			bool    _useMultisampling;
			uint8_t _count;
			uint8_t _quality;
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
			static const uint8_t Dimensions = N;

			std::string                    _name;
			Format                         _textureFormat;
			VecND<uint32_t, N>             _dimensions;
			TextureMipMapDescriptor        _mipMap;
			TextureArrayDescriptor         _array;
			TextureMultisapmlingDescriptor _multisampling;
			ResourceUsage                  _cpuGpuUsage;
			BufferBindingFlags_t           _gpuBinding;

			std::string toString() const {
				std::stringstream ss;

				ss
					<< "TextureDescriptor<" << N << ">"
					<< " ('" << _name << "'): "
					<< " Dimensions: "      << N << ", "
					<< " Format: "          << (uint8_t)_textureFormat << ", "
					<< " MipMaps: "         << (_mipMap._useMipMaps ? "true" : "false") << "; Levels: " << _mipMap._mipLevels << ", "
					<< " Array: "           << (_array._isTextureArray ? "true" : "false") << "; Layers: " << _array._textureCount << ", "
					<< " Multisampling: "   << (_multisampling._useMultisampling ? "true" : "false")
					<< "; Count/Quality:  " << _multisampling._count << "/" << _multisampling._quality << ", "
					<< " CPU-GPU-Usage: "   << (uint8_t)_cpuGpuUsage << ", "
					<< " GPU-Binding:  "    << (uint8_t)_gpuBinding << ";";

				return ss.str();
			}
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
		 * \struct	ResourceDescriptor<EResourceType::TEXTURE,EResourceSubType::TEXTURE_1D>
		 *
		 * \brief	A texture 1 d>.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D>
			: public TextureDescriptorImpl<1>
		{
			typedef TextureDescriptorImpl<1> type;
		};
		typedef ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_1D> Texture1DDescriptor;

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::TEXTURE,EResourceSubType::TEXTURE_2D>
		 *
		 * \brief	A texture 2 d>.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D>
			: public TextureDescriptorImpl<2>
		{
			typedef TextureDescriptorImpl<2> type;
		};
		typedef ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_2D> Texture2DDescriptor;

		/**********************************************************************************************//**
		 * \struct	ResourceDescriptor<EResourceType::TEXTURE,EResourceSubType::TEXTURE_3D>
		 *
		 * \brief	A texture 3 d>.
		 **************************************************************************************************/
		template <>
		struct ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D>
			: public TextureDescriptorImpl<3>
		{
			typedef TextureDescriptorImpl<3> type;
		};
		typedef ResourceDescriptor<EResourceType::TEXTURE, EResourceSubType::TEXTURE_3D> Texture3DDescriptor;

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
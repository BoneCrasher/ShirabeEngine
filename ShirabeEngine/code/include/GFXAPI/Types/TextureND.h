#ifndef __SHIRABE_TEXTUREND_H__
#define __SHIRABE_TEXTUREND_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/ResourceDomainTransfer.h"
#include "Resources/System/Core/ResourceTraits.h"

#include "GFXAPI/Definitions.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

namespace Engine {
  namespace Resources {
    using namespace GFXAPI;

    class Texture1D;
    class Texture2D;
    class Texture3D;
    
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
     * \struct	TextureResourceSubtypeMapper
     *
     * \brief	A texture resource subtype mapper.
     *
     * \tparam	N	Type of the n.
     **************************************************************************************************/
    template <uint8_t N>
    struct TextureResourceMapper {
    };

    /**********************************************************************************************//**
     * \struct	TextureResourceSubtypeMapper<1>
     *
     * \brief	A texture resource subtype mapper.
     **************************************************************************************************/
    template <>
    struct TextureResourceMapper<1> {
      typedef Texture1D type;
    };

    /**********************************************************************************************//**
     * \struct	TextureResourceSubtypeMapper<2>
     *
     * \brief	A texture resource subtype mapper.
     **************************************************************************************************/
    template <>
    struct TextureResourceMapper<2> {
      typedef Texture2D type;
    };

    /**********************************************************************************************//**
     * \struct	TextureResourceSubtypeMapper<3>
     *
     * \brief	A texture resource subtype mapper.
     **************************************************************************************************/
    template <>
    struct TextureResourceMapper<3> {
      typedef Texture3D type;
    };

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
    struct TextureNDDescriptorImpl
      : public DescriptorImplBase<EResourceType::TEXTURE, TextureResourceSubtypeMapper<N>::value>
    {
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
          << " Dimensions: " << N << ", "
          << " Format: " << (uint8_t)textureFormat << ", "
          << " MipMaps: " << (mipMap.useMipMaps ? "true" : "false") << "; Levels: " << mipMap.mipLevels << ", "
          << " Array: " << (array.isTextureArray ? "true" : "false") << "; Layers: " << array.size << ", "
          << " Multisampling: " << (multisampling.useMultisampling ? "true" : "false")
          << "; Count/Quality:  " << multisampling.size << "/" << multisampling.quality << ", "
          << " CPU-GPU-Usage: " << (uint8_t)cpuGpuUsage << ", "
          << " GPU-Binding:  " << (uint8_t)gpuBinding << ";";

        return ss.str();
      }
    };

    /**********************************************************************************************//**
     * \struct  TextureNDCreationRequestImpl
     *
     * \brief A texture nd creation request implementation.
     *
     * \tparam  N Type of the n.
     **************************************************************************************************/
    template <uint8_t N>
    struct TextureNDCreationRequestImpl {

    public:
      const struct ResourceDescriptor<typename TextureResourceMapper<N>::type>& resourceDescriptor() const;

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
      TextureNDDescriptorImpl<N> _resourceDescriptor;
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

    struct TextureNDResourceBinding {
      ResourceHandle handle;
      ResourceHandle srvBinding;
      ResourceHandle rtvBinding;
      ResourceHandle dsvBinding;
    };

    template <uint8_t N>
    class TextureNDBase;

    DeclareTemplatedResourceTraits(
      template <uint8_t N>,
      TextureND,
      typename TextureResourceMapper<N>::type,
      EResourceType::TEXTURE,
      TextureResourceSubtypeMapper<N>::value,
      TextureNDResourceBinding,
      TextureNDDescriptorImpl<N>,
      TextureNDCreationRequestImpl<N>,
      void,
      TextureNDQueryRequestImpl<N>,
      TextureNDDestructionRequestImpl<N>);

    DefineTraitsPublicTemplateTypes(template <uint8_t N>, TextureND, TextureNDTraits<N>);

    DefineTraitsPublicTypes(Texture1D, TextureNDTraits<1>);
    DefineTraitsPublicTypes(Texture2D, TextureNDTraits<2>);
    DefineTraitsPublicTypes(Texture3D, TextureNDTraits<3>);

    /**********************************************************************************************//**
     * \class	TextureNDBase
     *
     * \brief	Common templated base class for any texture used in the engine.
     *
     * \tparam	N	Type of the n.
     **************************************************************************************************/
    template <uint8_t N>
    class TextureNDBase
      : public TextureNDTraits<N>
      , public ResourceDescriptorAdapter<TextureNDTraits<N>>
      , public ResourceBindingAdapter<TextureNDTraits<N>>
    {
    public:
      using my_type = TextureNDBase<N>;

      TextureNDBase(
        const TextureNDDescriptor<N> &descriptor,
        const TextureNDBinding<N>    &binding)
        : TextureNDTraits<N>()
        , ResourceDescriptorAdapter<TextureNDTraits<N>>(descriptor)
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

    void foo() {
      ResourceDescriptor<Texture1D> desc;
      ResourceBinding<Texture1D>    binding;
      Texture1D t1 = Texture1D(desc, binding);
    }

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
    std::ostream& operator<<(std::ostream& s, const TextureNDDescriptor<N>& d) {
      return (s << d.toString());
    }

    template <uint8_t N>
    const ResourceDescriptor<typename TextureResourceMapper<N>::type>& 
      TextureNDCreationRequestImpl<N>
      ::resourceDescriptor() const 
    { 
      return ResourceDescriptor<typename TextureResourceMapper<N>::type>(_resourceDescriptor); 
    }
  }
}

#endif
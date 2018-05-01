#ifndef __SHIRABE_TEXTUREND_H__
#define __SHIRABE_TEXTUREND_H__

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/IResource.h"
#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Core/ResourceTraits.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"

#include "GFXAPI/Definitions.h"

#include "TextureNDDefinition.h"
#include "RequestDefaultImplementation.h"
#include "GFXAPI/Types/RenderTargetView.h"
#include "GFXAPI/Types/ShaderResourceView.h"
#include "GFXAPI/Types/DepthStencilView.h"

namespace Engine {
  namespace Resources {
    using namespace GFXAPI;

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

    template <uint8_t N>
    class TextureNDDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::TEXTURE;
      static const constexpr EResourceSubType resource_subtype = TextureResourceSubtypeMapper<N>::value;


      /**********************************************************************************************//**
       * \struct	TextureDescriptorImpl
       *
       * \brief	A texture descriptor implementation.
       *
       * \tparam	N	Type of the n.
       **************************************************************************************************/
      struct Descriptor
        : public DescriptorImplBase<EResourceType::TEXTURE, TextureResourceSubtypeMapper<N>::value>
      {
        static const uint8_t dimensionNb = N;

        std::string                    name;
        Format                         textureFormat;
        TVector<uint32_t, N>           dimensions;
        TextureMipMapDescriptor        mipMap;
        TextureArrayDescriptor         array;
        TextureMultisapmlingDescriptor multisampling;
        ResourceUsage                  cpuGpuUsage;
        BitField<BufferBinding>        gpuBinding;

        std::string toString() const;
      };

      /**********************************************************************************************//**
       * \struct  TextureNDCreationRequestImpl
       *
       * \brief A texture nd creation request implementation.
       *
       * \tparam  N Type of the n.
       **************************************************************************************************/
      class CreationRequest 
        : public BaseDeclaration::CreationRequestBase<Descriptor>
      {
      public:
        CreationRequest(const Descriptor &desc);

        std::string toString() const;
      };

      class UpdateRequest
        : public BaseDeclaration::UpdateRequestBase
      {
      public:
        UpdateRequest(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId);
      };

      class DestructionRequest
        : public BaseDeclaration::DestructionRequestBase
      {
      public:
        DestructionRequest(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId);
      };

      class Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId);
      };
    };

    /**********************************************************************************************//**
     * \class	TextureNDBase
     *
     * \brief	Common templated base class for any texture used in the engine.
     *
     * \tparam	N	Type of the n.
     **************************************************************************************************/
    template <uint8_t N>
    class Texture
      : public TextureNDDeclaration<N>
      , public ResourceDescriptorAdapter<TextureNDDeclaration<N>::Descriptor>
    {
    public:
      Texture(
        const typename Texture<N>::Descriptor &descriptor);

      inline Ptr<RenderTargetView>   renderTargetView()   const { return m_renderTargetView;   }
      inline Ptr<ShaderResourceView> shaderResourceView() const { return m_shaderResourceView; }
      inline Ptr<DepthStencilView>   depthStencilView()   const { return m_depthStencilView;   }

    private:
      Ptr<RenderTargetView>   m_renderTargetView;
      Ptr<ShaderResourceView> m_shaderResourceView;
      Ptr<DepthStencilView>   m_depthStencilView;
    };

    template <uint8_t N>
    DeclareTemplatedSharedPointerType(TextureND, Template(Texture<N>));

    using Texture1D = Texture<1>;
    using Texture2D = Texture<2>;
    using Texture3D = Texture<3>;

    DeclareSharedPointerType(Texture1D);
    DeclareSharedPointerType(Texture2D);
    DeclareSharedPointerType(Texture3D);

    DefineTraitsPublicTemplateTypes(template <uint8_t N>, Texture, Template(Texture<N>));


    template <uint8_t N>
    std::string
      TextureNDDeclaration<N>::Descriptor
      ::toString() const
    {
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
        << " GPU-Binding:  "  << (uint8_t)gpuBinding.value() << ";";

      return ss.str();
    }

    template <uint8_t N>
    TextureNDDeclaration<N>::CreationRequest
      ::CreationRequest(
        const Descriptor &desc)
      : BaseDeclaration::CreationRequestBase<Descriptor>(desc)
    {}
    
    template <uint8_t N>
    std::string
      TextureNDDeclaration<N>::CreationRequest
      ::toString() const
    {
      std::stringstream ss;

      ss
        << "TextureNDCreationRequest<" << N << ">: \n"
        << "[\n"
        << resourceDescriptor().toString() << "\n"
        << "]"
        << std::endl;

      return ss.str();
    }

    template <uint8_t N>
    TextureNDDeclaration<N>::UpdateRequest
      ::UpdateRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::UpdateRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    template <uint8_t N>
    TextureNDDeclaration<N>::DestructionRequest
      ::DestructionRequest(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::DestructionRequestBase(inPublicResourceId, inSubjacentResourceId)
    {}

    template <uint8_t N>
    TextureNDDeclaration<N>::Query
      ::Query(
        PublicResourceId_t    const& inPublicResourceId,
        SubjacentResourceId_t const& inSubjacentResourceId)
      : BaseDeclaration::QueryBase(inPublicResourceId, inSubjacentResourceId)
    {}

    template <uint8_t N>
    Texture<N>
      ::Texture(
        typename Texture<N>::Descriptor const&descriptor)
      : TextureNDDeclaration<N>()
      , ResourceDescriptorAdapter<typename TextureNDDeclaration<N>::Descriptor>(descriptor)
    {}


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
    std::ostream& operator<<(std::ostream& s, typename TextureNDDeclaration<N>::Descriptor const& d) {
      return (s << d.toString());
    }

  }
}

#endif
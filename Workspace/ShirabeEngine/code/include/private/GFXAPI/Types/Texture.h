#ifndef __SHIRABE_GFXAPI_TEXTURE_H__
#define __SHIRABE_GFXAPI_TEXTURE_H__

#include "Platform/Platform.h"

#include "Resources/Core/EResourceType.h"
#include "Resources/Core/IResource.h"
#include "Resources/Core/ResourceDomainTransfer.h"
#include "Resources/Core/ResourceTraits.h"

#include "Resources/Subsystems/GFXAPI/GFXAPI.h"
#include "Resources/Types/Texture.h"

#include "GFXAPI/Definitions.h"

#include "RequestDefaultImplementation.h"
#include "GFXAPI/Types/RenderTargetView.h"
#include "GFXAPI/Types/ShaderResourceView.h"
#include "GFXAPI/Types/DepthStencilView.h"

namespace Engine {
  namespace Resources {
    using namespace GFXAPI;

    class SHIRABE_TEST_EXPORT TextureDeclaration {
    public:
      static const constexpr EResourceType    resource_type    = EResourceType::TEXTURE;
      static const constexpr EResourceSubType resource_subtype = EResourceSubType::UNKNOWN;


      /**********************************************************************************************//**
       * \struct	TextureDescriptorImpl
       *
       * \brief	A texture descriptor implementation.
       *
       * \tparam	N	Type of the n.
       **************************************************************************************************/
      struct Descriptor
        : public DescriptorImplBase<EResourceType::TEXTURE, EResourceSubType::UNKNOWN>
      {
        std::string                    name;
        TextureInfo                    textureInfo;
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
    class SHIRABE_TEST_EXPORT Texture
      : public TextureDeclaration
      , public ResourceDescriptorAdapter<TextureDeclaration::Descriptor>
    {
    public:
      Texture(
        const TextureDeclaration::Descriptor &descriptor);
    };

    DeclareSharedPointerType(Texture);
    DefineTraitsPublicTypes(Texture);
    

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
    std::ostream& operator<<(std::ostream& s, TextureDeclaration::Descriptor const& d) {
      return (s << d.toString());
    }

  }
}

#endif
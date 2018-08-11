#ifndef __SHIRABE_GFXAPI_TEXTURE_H__
#define __SHIRABE_GFXAPI_TEXTURE_H__

#include "Platform/Platform.h"

#include "resources/core/eresourcetype.h"
#include "resources/core/iresource.h"
#include "resources/core/resourcedomaintransfer.h"
#include "resources/core/resourcetraits.h"
#include "resources/core/requestdefaultimplementation.h"

#include "GraphicsAPI/Definitions.h"
#include "GraphicsAPI/Resources/GFXAPI.h"
#include "GraphicsAPI/Resources/Types/Definition.h"


namespace Engine {
  namespace GFXAPI {

    struct Multisapmling {
      uint8_t size;
      uint8_t quality;
    };

    struct SHIRABE_TEST_EXPORT TextureInfo {
      uint32_t
        width,  // 0 - Undefined
        height, // At least 1
        depth;  // At least 1
      Format
        format;
      uint16_t
        arraySize; // At least 1 (basically everything is a vector...)
      uint16_t
        mipLevels;
      Multisapmling
        multisampling;

      TextureInfo();

      void assignTextureInfoParameters(TextureInfo const&other);
    };

    using ArraySlices = Range;
    using MipSlices   = Range;

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
      struct SHIRABE_TEST_EXPORT Descriptor
        : public DescriptorImplBase<EResourceType::TEXTURE, EResourceSubType::UNKNOWN>
      {
        std::string                name;
        TextureInfo                textureInfo;
        ResourceUsage              cpuGpuUsage;
        BitField<BufferBinding>    gpuBinding;
        Vector<#include "resources/core/resourcedatasource.h"> initialData;

        std::string toString() const;
      };

      /**********************************************************************************************//**
       * \struct  TextureNDCreationRequestImpl
       *
       * \brief A texture nd creation request implementation.
       *
       * \tparam  N Type of the n.
       **************************************************************************************************/
      class SHIRABE_TEST_EXPORT CreationRequest
        : public BaseDeclaration::CreationRequestBase<Descriptor>
      {
      public:
        CreationRequest(const Descriptor &desc);

        std::string toString() const;
      };

      class SHIRABE_TEST_EXPORT CUpdateRequest
        : public BaseDeclaration::CUpdateRequestBase
      {
      public:
        CUpdateRequest(
          PublicResourceId_t    const& inPublicResourceId);
      };

      class SHIRABE_TEST_EXPORT CDestructionRequest
        : public BaseDeclaration::CDestructionRequestBase
      {
      public:
        CDestructionRequest(
          PublicResourceId_t    const& inPublicResourceId);
      };

      class SHIRABE_TEST_EXPORT Query
        : public BaseDeclaration::QueryBase
      {
      public:
        Query(
          PublicResourceId_t    const& inPublicResourceId);
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
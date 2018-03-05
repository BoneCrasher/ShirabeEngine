#ifndef __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__
#define __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__

#include "Resources/System/Core/ResourceDTO.h"
#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"

#include "Resources/System/Core/ResourceDomainTransfer.h"

namespace Engine {
  namespace GFXAPI {

    using namespace Engine::Resources;

    class BaseDeclaration {
    public:

      template <typename TDescriptor>
      class CreationRequestBase {
      public:
        CreationRequestBase(
          TDescriptor const&desc)
          : m_resourceDescriptor(desc)
        {}

        TDescriptor const& resourceDescriptor() const { return m_resourceDescriptor; }

        bool serializeOnDestruct() const { return m_serializeOnDestruct; }

        virtual std::string toString() const = 0;

      private:
        TDescriptor m_resourceDescriptor;

        bool m_serializeOnDestruct;
      };
      
      class ExistingResourceRequestBase { 
      public:
        inline 
          ExistingResourceRequestBase(
            PublicResourceId_t    const& inPublicResourceId,
            SubjacentResourceId_t const& inSubjacentResourceId)
          : m_publicResourceId(inPublicResourceId)
          , m_subjacentResouceId(inSubjacentResourceId)
        {}

        inline PublicResourceId_t    const& publicResourceId()    const { return m_publicResourceId;   }
        inline SubjacentResourceId_t const& subjacentResourceId() const { return m_subjacentResouceId; }

      private:
        PublicResourceId_t    m_publicResourceId;
        SubjacentResourceId_t m_subjacentResouceId;
      };


      class UpdateRequestBase
        : public ExistingResourceRequestBase
      {
      public:
        inline UpdateRequestBase(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId)
          : ExistingResourceRequestBase(inPublicResourceId, inSubjacentResourceId)
        {}
      };

      class DestructionRequestBase
        : public ExistingResourceRequestBase
      {
      public:
        inline DestructionRequestBase(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId)
          : ExistingResourceRequestBase(inPublicResourceId, inSubjacentResourceId)
        {}
      };

      class QueryBase
        : public ExistingResourceRequestBase
      {
      public:
        inline QueryBase(
          PublicResourceId_t    const& inPublicResourceId,
          SubjacentResourceId_t const& inSubjacentResourceId)
          : ExistingResourceRequestBase(inPublicResourceId, inSubjacentResourceId)
        {}
      };
    };

  }
}

#endif
#ifndef __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__
#define __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__

#include "resources/core/resourcedto.h"
#include "resources/core/resourcedomaintransfer.h"
#include "resources/core/eresourcetype.h"
#include "resources/core/iresource.h"
#include "Resources/Core/#include "resources/core/resourcedatasource.h".h"

namespace Engine {
  namespace Resources {

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
            PublicResourceId_t    const& inPublicResourceId)
          : m_publicResourceId(inPublicResourceId)
        {}

        inline PublicResourceId_t    const& publicResourceId()    const { return m_publicResourceId;   }

      private:
        PublicResourceId_t    m_publicResourceId;
      };


      class UpdateRequestBase
        : public ExistingResourceRequestBase
      {
      public:
        inline UpdateRequestBase(
          PublicResourceId_t    const& inPublicResourceId)
          : ExistingResourceRequestBase(inPublicResourceId)
        {}
      };

      class DestructionRequestBase
        : public ExistingResourceRequestBase
      {
      public:
        inline DestructionRequestBase(
          PublicResourceId_t    const& inPublicResourceId)
          : ExistingResourceRequestBase(inPublicResourceId)
        {}
      };

      class QueryBase
        : public ExistingResourceRequestBase
      {
      public:
        inline QueryBase(
          PublicResourceId_t    const& inPublicResourceId)
          : ExistingResourceRequestBase(inPublicResourceId)
        {}
      };
    };

  }
}

#endif
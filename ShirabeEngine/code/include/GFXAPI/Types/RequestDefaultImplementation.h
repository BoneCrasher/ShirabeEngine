#ifndef __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__
#define __SHIRABE_RESOURCES_REQUEST_DEFAULT_IMPL_H__

#include "Resources/System/Core/EResourceType.h"
#include "Resources/System/Core/IResource.h"
#include "Resources/System/Core/Handle.h"
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
          TDescriptor    const&desc)
          : _resourceDescriptor(desc)
        {}

        TDescriptor const& resourceDescriptor() const { return _resourceDescriptor; }

        virtual std::string toString() const = 0;

      private:
        TDescriptor _resourceDescriptor;
      };
      
      class ExistingResourceRequestBase { 
      public:
        inline 
          ExistingResourceRequestBase(ResourceHandle const& inHandle)
          : m_handle(inHandle)
        {}

        inline ResourceHandle const& handle() const { return m_handle; }

      private:
        ResourceHandle m_handle;
      };


      class UpdateRequestBase
        : public ExistingResourceRequestBase
      {
      public:
        inline UpdateRequestBase(ResourceHandle const& handle = ResourceHandle::Invalid())
          : ExistingResourceRequestBase(handle)
        {}
      };

      class DestructionRequestBase
        : public ExistingResourceRequestBase
      {
      public:
        inline DestructionRequestBase(ResourceHandle const& handle = ResourceHandle::Invalid())
          : ExistingResourceRequestBase(handle)
        {}
      };

      class QueryBase
        : public ExistingResourceRequestBase
      {
      public:
        inline QueryBase(ResourceHandle const& handle = ResourceHandle::Invalid())
          : ExistingResourceRequestBase(handle)
        {}
      };

      struct BindingBase {
        ResourceHandle handle;

        inline BindingBase() 
          : handle(ResourceHandle::Invalid()) 
        {}
      };
    };

  }
}

#endif
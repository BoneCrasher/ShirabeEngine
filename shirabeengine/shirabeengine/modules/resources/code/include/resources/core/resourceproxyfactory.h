#ifndef __SHIRABE_IRESOURCEPROXYFACTORY_H__
#define __SHIRABE_IRESOURCEPROXYFACTORY_H__

#include <variant>

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"

#include "Resources/Core/IResourceProxy.h"

namespace engine {
  namespace resources {

    class ResourceProxyFactory 
    {
      using CreatorMap_t = Map<EResourceType, Any>;

    public:
      template <typename TResource>
      using CreatorFn_t  = std::function<CStdSharedPtr_t<IResourceProxy<TResource>>(EProxyType const&, typename TResource::CreationRequest const&)>;

      template <typename TResource>
      CStdSharedPtr_t<IResourceProxy<TResource>> create(
        EProxyType                          const&proxyType,
        typename TResource::CreationRequest const&creationRequest);

      template <typename TResource>
      bool addCreator(
        EResourceType          const&type,
        CreatorFn_t<TResource> const&creatorFn);
      
    private:      
      CreatorMap_t m_creators;
    };

    template <typename TResource>
    CStdSharedPtr_t<IResourceProxy<TResource>>
      ResourceProxyFactory::create(
        EProxyType                          const&proxyType,
        typename TResource::CreationRequest const&creationRequest)
    {
      EResourceType type = creationRequest.resourceDescriptor().type();

      if(m_creators.find(type) == m_creators.end())
        return nullptr;

      Any                    anyCreator = m_creators.at(type);
      CreatorFn_t<TResource> creator    = std::any_cast<CreatorFn_t<TResource>>(anyCreator);

      CStdSharedPtr_t<IResourceProxy<TResource>> proxy = creator(proxyType, creationRequest);
      if(!proxy)
        throw std::runtime_error("Cannot create proxy.");

      return proxy;
    }
    
    template <typename TResource>
    bool 
      ResourceProxyFactory::addCreator(
        EResourceType          const&type, 
        CreatorFn_t<TResource> const&creatorFn)
    {
      if(m_creators.find(type) != m_creators.end())
        return false;

      m_creators[type] = creatorFn;

      return true;
    }
  }
}

#endif
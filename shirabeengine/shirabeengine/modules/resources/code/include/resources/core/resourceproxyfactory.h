#ifndef __SHIRABE_IRESOURCEPROXYFACTORY_H__
#define __SHIRABE_IRESOURCEPROXYFACTORY_H__

#include <variant>

#include "core/enginetypehelper.h"
#include "core/enginestatus.h"

#include "resources/core/iresourceproxy.h"

namespace engine
{
    namespace resources
    {
        /**
         * The resource proxy factory is responsible for creating the correct proxy for a given proxy type and creation request.
         */
        class CResourceProxyFactory
        {
        public_typedefs:
            using CreatorMap_t = Map<EResourceSubType, Any_t>;

            template <typename TResource>
            using CreatorFn_t  = std::function<
                                        CStdSharedPtr_t<IResourceProxy<TResource>>(
                                            EProxyType                           const&,
                                            typename TResource::CCreationRequest const&)>;

        public_methods:
            /**
             * Create a new resource proxy given a specific proxy type and it's creation request.
             *
             * @tparam TResource       The resource type to create a proxy for.
             * @param aProxyType       The type of type of the proxy to be created.
             * @param aCreationRequest The creation request of the resource to be created.
             * @return
             */
            template <typename TResource>
            CStdSharedPtr_t<IResourceProxy<TResource>> create(
                    EProxyType                           const &aProxyType,
                    typename TResource::CCreationRequest const &aCreationRequest);

            /**
             * Register a new creator function for a provided resource type.

             * @tparam TResource The resource type of the creator.
             * @param aSubtype   The subtype of the resource to create.
             * @param aCreatorFn The function creating instances of type/subtype.
             * @return           True, of successful. False otherwise.
             */
            template <typename TResource>
            bool addCreator(
                    EResourceSubType       const &aSubtype,
                    CreatorFn_t<TResource> const &aCreatorFn);

        private_members:
            CreatorMap_t m_creators;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        CStdSharedPtr_t<IResourceProxy<TResource>>
        CResourceProxyFactory::create(
                EProxyType                           const &aProxyType,
                typename TResource::CCreationRequest const &aCreationRequest)
        {
            EResourceSubType const type = aCreationRequest.resourceDescriptor().subtype();

            if(m_creators.find(type) == m_creators.end())
                return nullptr;

            Any_t                    const &anyCreator = m_creators.at(type);
            CreatorFn_t<TResource> const &creator    = std::any_cast<CreatorFn_t<TResource>>(anyCreator);

            CStdSharedPtr_t<IResourceProxy<TResource>> const proxy = creator(aProxyType, aCreationRequest);
            if(!proxy)
                throw std::runtime_error("Cannot create proxy.");

            return proxy;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        template <typename TResource>
        bool CResourceProxyFactory::addCreator(
                EResourceSubType       const &aType,
                CreatorFn_t<TResource> const &aCreatorFn)
        {
            if(m_creators.find(aType) != m_creators.end())
                return false;

            m_creators[aType] = aCreatorFn;

            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}

#endif

/*!
 * @file      resourcemanager.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      10/09/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#ifndef __SHIRABE_ENGINE_RESOURCE_MANAGER_H__
#define __SHIRABE_ENGINE_RESOURCE_MANAGER_H__

#include <resources/core/resourcemanagerbase.h>

namespace engine
{
    using resources::CResourceManagerBase;
    using resources::CResourceProxyFactory;

    /**
     * Engine default implementation of the resource manager, which will register all
     * necessary type creators, destructors, updaters and queries.
     *
     */
    class CResourceManager
            : public CResourceManagerBase
    {
    public_constructors:
        /**
         * Create a new resource manager, with a resource proxy factory required
         * for the proxy resource management to work.
         *
         * @param aProxyFactory A point to a resource factory.
         */
        CResourceManager(Shared<CResourceProxyFactory> const &aProxyFactory);

    public_destructors:
        /**
         * Destroy and run...
         */
        virtual ~CResourceManager();

    public_methods:
        /**
         * Setup this resource manager for use.
         *
         * @return EEngineStatus::Ok on success. An error code otherwise.
         */
        CEngineResult<> initialize();

        /**
         * Deinitialize the resource manager and free..
         *
         * @return EEngineStatus::Ok on success. An error code otherwise.
         */
        CEngineResult<> deinitialize();

    private_methods:
        template <typename TType>
        auto addCreator(CResourceManagerBase *aResourceManager) -> bool
        {
            return CResourceManagerBase::addCreator<TType>
                   (
                        std::bind(&CResourceManager::createResourceImpl<TType>
                                  , aResourceManager
                                  , std::placeholders::_1
                                  , std::placeholders::_2
                                  , std::placeholders::_3)
                   ).successful();
        }

        template <typename TType>
        auto addLoader(CResourceManagerBase *aResourceManager) -> bool
        {
            return CResourceManagerBase::addLoader<TType>
                   (
                        std::bind(&CResourceManager::loadResourceImpl<TType>
                                  , aResourceManager
                                  , std::placeholders::_1)
                   ).successful();
        }

        template <typename TType>
        auto addUpdater(CResourceManagerBase *aResourceManager) -> bool
        {
            return CResourceManagerBase::addUpdater<TType>
                   (
                        std::bind(&CResourceManager::updateResourceImpl<TType>
                                  , aResourceManager
                                  , std::placeholders::_1
                                  , std::placeholders::_2)
                   ).successful();
        }

        template <typename TType>
        auto addUnloader(CResourceManagerBase *aResourceManager) -> bool
        {
            return CResourceManagerBase::addUnloader<TType>
                   (
                        std::bind(&CResourceManager::unloadResourceImpl<TType>
                                  , aResourceManager
                                  , std::placeholders::_1)
                   ).successful();
        }

        template <typename TType>
        auto addDestructor(CResourceManagerBase *aResourceManager) -> bool
        {
            return CResourceManagerBase::addDestructor<TType>
                   (
                        std::bind(&CResourceManager::destroyResourceImpl<TType>
                                  , aResourceManager
                                  , std::placeholders::_1)
                   ).successful();
        }
    };
}

#endif // __SHIRABE_ENGINE_RESOURCE_MANAGER_H__

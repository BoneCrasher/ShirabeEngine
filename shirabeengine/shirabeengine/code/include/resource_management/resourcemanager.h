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
        CResourceManager(CStdSharedPtr_t<CResourceProxyFactory> const &aProxyFactory);

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
        EEngineStatus initialize();

        /**
         * Deinitialize the resource manager and free..
         *
         * @return EEngineStatus::Ok on success. An error code otherwise.
         */
        EEngineStatus deinitialize();
    };
}

#endif // __SHIRABE_ENGINE_RESOURCE_MANAGER_H__

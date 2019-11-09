//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/enginestatus.h>

namespace engine
{
    namespace resources
    {
        class IGpuApiResourceObject;

        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT ILogicalResourceObject
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(ILogicalResourceObject);

        private_api:
            virtual void bindGpuApiResourceInterface(Unique<IGpuApiResourceObject> aGpuApiInterface) = 0;

            virtual Unique<IGpuApiResourceObject>& getGpuApiResourceInterface() = 0;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__

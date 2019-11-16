//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/enginestatus.h>
#include "resources/igpuapiresourceobject.h"

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
            virtual void setGpuApiResourceHandle(GpuApiHandle_t const &aHandle) = 0;

        public_api:
            [[nodiscard]]
            virtual GpuApiHandle_t getGpuApiResourceHandle() const = 0;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__

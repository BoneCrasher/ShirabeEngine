//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__

#include <platform/platform.h>

#include <core/enginestatus.h>

#include "resources/iresourceobjectprivate.h"

namespace engine
{
    namespace resources
    {
        class [[nodiscard]] SHIRABE_LIBRARY_EXPORT IResourceObject
                : private IResourceObjectPrivate
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(IResourceObject);

        public_api:
            virtual CEngineResult<> bind()     = 0;
            virtual CEngineResult<> unbind()   = 0;
            virtual CEngineResult<> transfer() = 0;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECT_H__

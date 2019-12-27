//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_ILOADABLEGPUAPIRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_ILOADABLEGPUAPIRESOURCEOBJECT_H__

#include <platform/platform.h>
#include <core/enginestatus.h>

#include "resources/resourcedescriptions.h"
#include "resources/ilogicalresourceobject.h"

namespace engine
{
    template <typename... TArgs>
    class CSubject;

    namespace resources
    {
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        class CResourceManager;
        class CGpuApiDependencyCollection;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT ILoadableGpuApiResourceObject
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(ILoadableGpuApiResourceObject);

        public_api:
            virtual CEngineResult<> load()   const = 0;
            virtual CEngineResult<> unload() const = 0;
        };
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

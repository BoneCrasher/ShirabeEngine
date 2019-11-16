//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_ITRANSFERRABLEGPUAPIRESOURCEOBJECT_H__
#define __SHIRABEDEVELOPMENT_ITRANSFERRABLEGPUAPIRESOURCEOBJECT_H__

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
            SHIRABE_LIBRARY_EXPORT ITransferrableGpuApiResourceObject
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(ITransferrableGpuApiResourceObject);

        public_api:
            virtual CEngineResult<> transfer() = 0;
        };
        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

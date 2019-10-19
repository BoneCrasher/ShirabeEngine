//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

#include <platform/platform.h>

#include <core/enginestatus.h>

namespace engine
{
    namespace resources
    {
        class [[nodiscard]] SHIRABE_LIBRARY_EXPORT IResourceObjectPrivate
        {
            SHIRABE_DECLARE_INTERFACE(IResourceObjectPrivate);

        public_api:
            virtual CEngineResult<> create()  = 0;
            virtual CEngineResult<> load()    = 0;
            virtual CEngineResult<> unload()  = 0;
            virtual CEngineResult<> destroy() = 0;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

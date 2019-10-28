//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

#include <platform/platform.h>
#include <core/enginestatus.h>
#include "resources/ilogicalresourceobject.h"

namespace engine
{
    template <typename... TArgs>
    class CSubject;

    namespace resources
    {
        class CResourceManager;

        enum class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT EGpuApiResourceState
        {
            Unknown = 0
            , Unavailable
            , Loading
            , Loaded
            , Discarded
            , Error
        };

        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT IGpuApiResourceObject
            : public ILogicalResourceObject // Inherits signature of bind, unbind and transfer
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(IGpuApiResourceObject);

        public_typedefs:
            using ObservableState_t = CSubject<Shared<IGpuApiResourceObject>, EGpuApiResourceState>;

        public_api:
            virtual CEngineResult<> create()  = 0;
            virtual CEngineResult<> load()    = 0;
            virtual CEngineResult<> unload()  = 0;
            virtual CEngineResult<> destroy() = 0;

            virtual ObservableState_t& observableState() = 0;
        };

    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

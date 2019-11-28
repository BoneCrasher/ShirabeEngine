//
// Created by dotti on 19.10.19.
//

#ifndef __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__
#define __SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

#include <platform/platform.h>
#include <core/enginestatus.h>

#include "resources/resourcedescriptions.h"

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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using GpuApiHandle_t = uint64_t;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using GpuApiResourceDependencies_t = std::unordered_map<ResourceId_t, GpuApiHandle_t>;
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
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
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        class
            [[nodiscard]]
            SHIRABE_LIBRARY_EXPORT IGpuApiResourceObject
        {
            friend class CResourceManager;

            SHIRABE_DECLARE_INTERFACE(IGpuApiResourceObject);

        public_typedefs:
            using ObservableState_t = CSubject<EGpuApiResourceState>;

        public_api:
            virtual resources::GpuApiHandle_t const getHandle() = 0;

            // virtual CEngineResult<> create(GpuApiResourceDependencies_t const &aDependencies) = 0;
            virtual CEngineResult<> destroy()                                                 = 0;

            [[nodiscard]]
            virtual EGpuApiResourceState getResourceState() const = 0;

            virtual Shared<ObservableState_t> observableState() = 0;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

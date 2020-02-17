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
        template <typename... TResources>
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
              Unknown      = 0
            , Unavailable  = 1
            , Creating     = 2
            , Created      = 4
            , Loading      = 8
            , Loaded       = 16
            , Transferring = 32
            , Transferred  = 64
            , Unloading    = 128
            , Unloaded     = 256
            , Discarding   = 512
            , Discarded    = 1024
            , Error        = 2048
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
        public_api:
            virtual resources::GpuApiHandle_t const getHandle() = 0;

            virtual CEngineResult<> destroy() = 0;
        };
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
    }
}

#endif //__SHIRABEDEVELOPMENT_IRESOURCEOBJECTPRIVATE_H__

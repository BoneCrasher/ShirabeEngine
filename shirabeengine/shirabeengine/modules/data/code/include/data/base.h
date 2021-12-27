//
// Created by dottideveloper on 25.12.21.
//
#ifndef __SHIRABEDEVELOPMENT_GPURESOURCE_BASE_H__
#define __SHIRABEDEVELOPMENT_GPURESOURCE_BASE_H__

#include <base/declaration.h>
#include <platform/platform.h>

namespace engine::data
{
    /**
     * Shared base class implementation for all Gpu Resources that integrate with the
     * RHI resource manager. These classes serve as a typed front-end to the engine without
     * dealing with any of the Platform Memory-intrinsics or RHI memory management and synchronization.
     *
     * @tparam TRHIResource    The RHI resource type to be managed by the resource frontend.
     * @tparam TUnderlyingData The underlying CPU-side data struct that corresponds with the specific data struct
     *                         in the shaders.
     */
    template <typename TRHIResource>
    class SHIRABEENGINE_LIBRARY_EXPORT AGpuResourceBase
    {
    protected_members:
        typename TRHIResource::RHIResourceDescriptor_t mRHIResourceDescriptor;
        Shared<TRHIResource>                           mRHIResourceState;

    public_constructors:
        AGpuResourceBase();

    public_destructors:
        virtual ~AGpuResourceBase() = default;

    public_methods:
        virtual EEngineStatus initRHIResource()    = 0;
        virtual EEngineStatus releaseRHIResource() = 0;
        virtual EEngineStatus updateRHIResource()  = 0;
    };

    #include "base.inl"
}

#endif //__SHIRABEDEVELOPMENT_GPURESOURCE_BASE_H__

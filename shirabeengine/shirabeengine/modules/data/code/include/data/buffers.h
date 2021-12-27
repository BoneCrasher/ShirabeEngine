//
// Created by dottideveloper on 25.12.21.
//
#ifndef __SHIRABEDEVELOPMENT_GPURESOURCE_BUFFERS_H__
#define __SHIRABEDEVELOPMENT_GPURESOURCE_BUFFERS_H__

#include <util/architecture/engine_locator.h>
#include <rhi/vulkan_resources/resources/types/vulkanbufferresource.h>
#include "data/base.h"

namespace engine::data
{
    enum class SHIRABEENGINE_LIBRARY_EXPORT EGpuResourceUpdatePolicy
    {
        Default = 0,
        KeepMapped,
        Immutable
    };

    /**
     * Generic base class implementation of a Gpu Data Buffer.
     * It's purpose is to enable the various Cpu<->Gpu data buffer patterns
     * for graphics and compute scenarios, such as:
     * - Default (GPU-read-write)
     * - Immutable buffers (GPU-only, CPU-init once)
     * - Dynamic buffers (GPU-read only, CPU-write only)
     * - Staging (CPU to GPU transfer)
     */
    template <typename TUnderlyingData, EGpuResourceUpdatePolicy UpdatePolicy = EGpuResourceUpdatePolicy::Default>
    class SHIRABEENGINE_LIBRARY_EXPORT CGpuDataBufferGeneric
        : public AGpuResourceBase<vulkan::RHIBufferResourceState_t>
    {
    private_members:

    public_constructors:
        CGpuDataBufferGeneric();

    public_destructors:
        ~CGpuDataBufferGeneric() override = default;

    protected_methods:
        EEngineStatus initRHIResource() override;
        EEngineStatus releaseRHIResource() override;

        EEngineStatus updateRHIResource() override;
    };

    #include "buffers.inl"
}

#endif //__SHIRABEDEVELOPMENT_GPURESOURCE_BUFFERS_H__

//
// Created by dottideveloper on 25.12.21.
//
#ifndef __SHIRABEDEVELOPMENT_BUFFERS_H__
#define __SHIRABEDEVELOPMENT_BUFFERS_H__

#include <base/declaration.h>
#include <platform/platform.h>
#include <rhi/vulkan_resources/resources/types/vulkanbufferresource.h>

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
        AGpuResourceBase()
            : mRHIResourceDescriptor()
            , mRHIResourceState(nullptr)
        {};

    public_destructors:
        virtual ~AGpuResourceBase() = default;

    public_methods:
        virtual EEngineStatus initRHIResource()    = 0;
        virtual EEngineStatus releaseRHIResource() = 0;
        virtual EEngineStatus updateRHIResource()  = 0;
    };

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
        virtual ~CGpuDataBufferGeneric() = default;

    public_methods:
        EEngineStatus update();

    protected_methods:
        EEngineStatus initRHIResource() override;
        EEngineStatus releaseRHIResource() override;

        EEngineStatus updateRHIResource() override;
    };
}

#endif //__SHIRABEDEVELOPMENT_BUFFERS_H__

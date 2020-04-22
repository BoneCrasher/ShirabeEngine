//
// Created by dottideveloper on 22.04.20.
//

#ifndef __SHIRABEDEVELOPMENT_EXTENSIBILITY_H__
#define __SHIRABEDEVELOPMENT_EXTENSIBILITY_H__

namespace engine::resources
{
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

    using ResourceId_t = std::string;

    template <typename T>
    struct SLogicalToGpuApiResourceTypeMap
    {
        using TGpuApiResource = void;
    };

    /**
     * The SResourceState struct groups a logical resource, an optional gpu api
     * resource and it's current load state.
     */
    template <typename TLogicalResource>
    struct SHIRABE_LIBRARY_EXPORT SResourceState
    {
        using ResourceState_t   = core::CBitField<EGpuApiResourceState>;
        using LogicalResource_t = TLogicalResource;
        using GpuApiResource_t  = typename SLogicalToGpuApiResourceTypeMap<TLogicalResource>::TGpuApiResource;
        using Descriptor_t      = typename LogicalResource_t::Descriptor_t;
        using GpuApiHandles_t   = typename GpuApiResource_t ::Handles_t;

        Descriptor_t    description;
        GpuApiHandles_t gpuApiHandles;
        ResourceState_t loadState;
    };
}

#endif //__SHIRABEDEVELOPMENT_EXTENSIBILITY_H__

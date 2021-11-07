//
// Created by dottideveloper on 22.04.20.
//

#ifndef __SHIRABEDEVELOPMENT_EXTENSIBILITY_H__
#define __SHIRABEDEVELOPMENT_EXTENSIBILITY_H__

#include <string>
#include <base/declaration.h>
#include <core/bitfield.h>

namespace engine::rhi
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
    struct SRHIResourceMap
    {
        using TMappedRHIResource = void;
    };

    /**
     * The SRHIResourceState struct groups a logical resource, an optional gpu api
     * resource and it's current load state.
     */
    template<typename TRHIResource>
    struct SHIRABE_LIBRARY_EXPORT SRHIResourceState
    {
        using RHIResourceState_t      = core::CBitField<EGpuApiResourceState>;
        using RHIResource_t           = TRHIResource;
        using RHIResourceDescriptor_t = typename RHIResource_t::Descriptor_t;
        using RHIMappedResource_t     = typename SRHIResourceMap<TRHIResource>::TMappedRHIResource;
        using RHIHandles_t            = typename RHIMappedResource_t ::Handles_t;

        RHIResourceDescriptor_t rhiCreateDesc;
        RHIResourceState_t      rhiLoadState;
        RHIHandles_t            rhiHandles;
    };
}

#endif //__SHIRABEDEVELOPMENT_EXTENSIBILITY_H__

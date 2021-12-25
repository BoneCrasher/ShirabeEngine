//
// Created by dottideveloper on 25.12.21.
//
#ifndef __SHIRABEDEVELOPMENT_BUFFERS_H__
#define __SHIRABEDEVELOPMENT_BUFFERS_H__

#include <base/declaration.h>
#include <platform/platform.h>

namespace engine::data
{
    /**
     * Generic base class implementation of a Gpu Data Buffer.
     * It's purpose is to enable the various Cpu<->Gpu data buffer patterns
     * for graphics and compute scenarios, such as:
     * - Default (GPU-read-write)
     * - Immutable buffers (GPU-only, CPU-init once)
     * - Dynamic buffers (GPU-read only, CPU-write only)
     * - Staging (CPU to GPU transfer)
     */
    class SHIRABEENGINE_LIBRARY_EXPORT CGpuDataBufferGeneric
    {
    private_members:
    public_constructors:
    public_destructors:
    public_methods:
    };
}

#endif //__SHIRABEDEVELOPMENT_BUFFERS_H__

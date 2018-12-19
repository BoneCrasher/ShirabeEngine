#ifndef __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__
#define __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__

#include <sstream>

#include <platform/platform.h>

#include <core/enginetypehelper.h>
#include <core/basictypes.h>

#include "graphicsapi/resources/types/definition.h"

namespace engine
{
    namespace gfxapi
    {
        /**
         * The EResourceUsage enum describes the permissions granted for CPU code and
         * GPU code to read/write resources.
         */
        enum class EResourceUsage
                : uint8_t
        {
            CPU_None_GPU_ReadWrite      = 1, //< Resource can be initialized w/ data on creation,
                                             //   but then only read or written by the GPU.
            CPU_InitConst_GPU_Read      = 2, //< Resource is initialized once and is then immutable,
                                             //   i.e. read-only from the GPU.
            CPU_Write_GPU_Read          = 4, //< Resource can be updated by the CPU and be immutably
                                             //   read by the GPU.
            CPU_ReadWrite_GPU_ReadWrite = 8  //< Resource can be used for duplex data exchange between
                                             //   CPU and GPU. Both sides have R/W access.
        };

        /**
         * The EBufferBinding enum describes the way a resource can be bound within a pipeline.
         */
        enum class EBufferBinding
                : uint16_t
        {
            VertexBuffer                 = 1,   //< The buffer resource can be used as a vertex source
                                                //   in the input assembly stage.
            IndexBuffer                  = 2,   //< The buffer resource can be used as an index source
                                                //   in the input assembly stage.
            UniformBuffer                = 4,   //< The buffer resource can be used as a uniform data
                                                //   source bound against the pipeline registers.
            TextureInput                 = 8,   //< The buffer resource can be used as a sampled input texture.
            InputAttachment              = 16,  //< The buffer resource can be used as a frame buffer input attachement (Read).
            ColorAttachment              = 32,  //< The buffer resource can be used as a frame buffer color attachement (Write).
            DepthAttachment              = 64,  //< The buffer resource can be used as a frame buffer depth attachement (Write).
            ShaderOutput_StreamOutput    = 128, //< The buffer resource can be used as a compute/shader data output target.
            UnorderedMultithreadedAccess = 256, //< The buffer resource can be accessed from multiple GPU threads in unordered fashion.
            CopySource                   = 512, //< The buffer will be used as a copy source buffer for copy operations.
            CopyTarget                   = 1024 //< The buffer will be used as a copy target buffer for copy operations.
        };

        /**
         * The EComparison enum describes comparison modes for various compare-operations in the system.
         */
        enum class EComparison
                : uint8_t
        {
            Never        =  1,
            Less         =  2,
            Equal        =  4,
            Greater      =  8,
            NotEqual     = 16,
            Always       = 32,
            LessEqual    = Less    | Equal,
            GreaterEqual = Greater | Equal,
        };

        using Format = resources::EFormat;
    }
}


#endif

#ifndef __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__
#define __SHIRABE_RESOURCES_CORE_DESCRIPTORS_H__

#include <sstream>

#include <platform/platform.h>

#include <core/enginetypehelper.h>
#include <core/basictypes.h>

namespace engine
{
    namespace graphicsapi
    {

        /**
         * The EGFXAPI enum identifies a specific graphics API
         */
        enum class EGFXAPI
        {
            Undefined = 0,
            DirectX   = 1,
            OpenGL    = 2,
            Vulkan    = 4
        };

        /**
         * The EGFXAPIVersion enum identifies a specific graphics API version requirement.
         */
        enum class EGFXAPIVersion
        {
            Undefined = 0,
            DirectX_11_0,
            DirectX_11_1,
            DirectX_11_2,
            DirectX_12_0,
            OpenGL_,
            Vulkan_1_0,
            Vulkan_1_1
        };

        enum class EFormat
                : uint16_t
        {
            Undefined = 0,
            SpecialFormatRangeFlag      = 1,
            Automatic,
            Structured,
            // 8-bit formats
            Format8BitFormatRangeFlag   = 8,
            R8_TYPELESS,
            R8_SINT,
            R8_UINT,
            R8_SNORM,
            R8_UNORM,
            // 16-bit formats
            Format16BitFormatRangeFlag  = 16,
            R16_TYPELESS,
            R16_SINT,
            R16_UINT,
            R16_SNORM,
            R16_UNORM,
            R16_FLOAT,
            // 32-bit formats
            Format32BitFormatRangeFlag  = 32,
            R8G8B8A8_TYPELESS,
            R8G8B8A8_SINT,
            R8G8B8A8_UINT,
            R8G8B8A8_SNORM,
            R8G8B8A8_UNORM,
            R8G8B8A8_UNORM_SRGB,
            R8G8B8A8_FLOAT,
            B8G8R8A8_TYPELESS,
            B8G8R8A8_SINT,
            B8G8R8A8_UINT,
            B8G8R8A8_SNORM,
            B8G8R8A8_UNORM,
            B8G8R8A8_UNORM_SRGB,
            B8G8R8A8_FLOAT,
            R24_UNORM_X8_TYPELESS,
            R32_TYPELESS,
            R32_SINT,
            R32_UINT,
            R32_SNORM,
            R32_UNORM,
            R32_FLOAT,
            D24_UNORM_S8_UINT,
            D32_FLOAT,
            // 64-bit formats
            Format64BitFormatRangeFlag  = 64,
            R16G16B16A16_TYPELESS = 64,
            R16G16B16A16_SINT,
            R16G16B16A16_UINT,
            R16G16B16A16_SNORM,
            R16G16B16A16_UNORM,
            R16G16B16A16_FLOAT,
            R32G32_UINT   ,
            R32G32_SINT   ,
            R32G32_SFLOAT ,
            R32_FLOAT_S8X24_TYPELESS,
            D32_FLOAT_S8X24_UINT,
            // 96-bit formats
            Format96BitFormatRangeFlag = 96,
            R32G32B32_UINT,
            R32G32B32_SINT,
            R32G32B32_SFLOAT,
            // 128-bit formats
            Format128BitFormatRangeFlag = 128,
            R32G32B32A32_TYPELESS,
            R32G32B32A32_SINT,
            R32G32B32A32_UINT,
            R32G32B32A32_FLOAT,
            Format256BitFormatRangeFlag = 256,
            // DXT/BC Compression and Video formats
            FormatCompressedFormatRangeFlag = 512,
            BC1_UNORM,
            BC1_UNORM_SRGB,
            BC1_TYPELESS,
            BC2_UNORM,
            BC2_UNORM_SRGB,
            BC2_TYPELESS,
            BC3_UNORM,
            BC3_UNORM_SRGB,
            BC3_TYPELESS,
            BC4_UNORM,
            BC4_SNORM,
            BC4_TYPELESS,
            BC5_UNORM,
            BC5_SNORM,
            BC5_TYPELESS,
            BC6H_SF16,
            BC6H_UF16,
            BC6H_TYPELESS,
            BC7_UNORM,
            BC7_UNORM_SRGB,
            BC7_TYPELESS
        };

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
            VertexBuffer                 = 1,    //< The buffer resource can be used as a vertex source
                                                 //   in the input assembly stage.
            IndexBuffer                  = 2,    //< The buffer resource can be used as an index source
                                                 //   in the input assembly stage.
            UniformBuffer                = 4,    //< The buffer resource can be used as a uniform data
                                                 //   source bound against the pipeline registers.
            TextureInput                 = 8,    //< The buffer resource can be used as a sampled input texture.
            InputAttachment              = 16,   //< The buffer resource can be used as a frame buffer input attachement (Read).
            ColorAttachment              = 32,   //< The buffer resource can be used as a frame buffer color attachement (Write).
            DepthAttachment              = 64,   //< The buffer resource can be used as a frame buffer depth attachement (Write).
            ShaderOutput_StreamOutput    = 128,  //< The buffer resource can be used as a compute/shader data output target.
            UnorderedMultithreadedAccess = 256,  //< The buffer resource can be accessed from multiple GPU threads in unordered fashion.
            CopySource                   = 512,  //< The buffer will be used as a copy source buffer for copy operations.
            CopyTarget                   = 1024, //< The buffer will be used as a copy target buffer for copy operations.
            PresentSource                = 2048
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

        /**
         * The SMultisapmling struct describes multisampling properties for various gfxapi
         * related components in the system.
         */
        struct SMultisampling
        {
        public_members:
            uint8_t
                    size,
                    quality;
        };

        /**
         * Explicit interface definition for array slice ranges.
         */
        using ArraySlices_t = CRange;
        /**
         * Explicit interface definition for mip slice ranges.
         */
        using MipSlices_t   = CRange;

        /**
         * The STextureInfo struct describes general texture attributes.
         */
        struct SHIRABE_TEST_EXPORT STextureInfo
        {
        public_members:
            uint32_t
                    width,  // 0 - Undefined
                    height, // At least 1
                    depth,  // At least 1
                    channels,
                    bitsPerChannel;
            EFormat
                    format;
            uint16_t
                    arraySize; // At least 1 (basically everything is a vector...)
            uint16_t
                    mipLevels;
            SMultisampling
                    multisampling;


        };

        /**
         * The EDepthWriteMask enum describes the depth write behaviour of the depth test/write stage.
         */
        enum class EDepthWriteMask
                : uint8_t
        {
            Zero = 0,
            All
        };

        /**
         * The EStencilOp enum describes how stencil values... (read up...) TBDone
         */
        enum class EStencilOp
                : uint8_t
        {
            Keep = 1,          // Do nothing
            Zero,              // Write 0
            Replace,           // Write RefValue
            IncrementSaturate, // Increment and clamp to MaxValue, if exceeded
            DecrementSaturate, // Decrement and clamp to 0, if exceeded
            Invert,            // Invert the bits
            Increment,         // Increment and wrap to 0, if exceeded
            Decrement          // Decrement and wrap to MaxValue, if exceeded
        };

        /**
         * The StencilCriteria struct binds together information
         * about depth stencil calculation behaviour.
         */
        struct SStencilCriteria
        {
            EStencilOp   failOp;
            EStencilOp   depthFailOp;
            EStencilOp   passOp;
            EComparison  stencilFunc;
        };

        /**
         * The EFillMode enum describes, how the rasterizer should adapt fragments from transformed output patches.
         */
        enum class EFillMode
        {
            WireFrame, //< Select only the pixels directly affected by the lines connecting the vertices.
            Solid      //< Select all pixels affected by the edges AND the area within the edges.
        };

        /**
         * The ECullMode enum describes, if and how backface culling should be performed.
         */
        enum class ECullMode
        {
            None,  //< Deactivate backface culling
            Front, //< Cull all faces, which are considered "front facing"
            Back   //< Cull all faces, which are considered "back facing".
        }; // In any case, we expect front to be counter clockwise!

        enum class EAttachmentLoadOp
        {
            LOAD      = 0,
            CLEAR     = 1,
            DONT_CARE = 2,
        };

        enum class EAttachmentStoreOp
        {
            STORE     = 0,
            DONT_CARE = 1,
        };


        enum EImageLayout
        {
            UNDEFINED                                      = 0,
            GENERAL                                        = 1,
            COLOR_ATTACHMENT_OPTIMAL                       = 2,
            DEPTH_STENCIL_ATTACHMENT_OPTIMAL               = 3,
            DEPTH_STENCIL_READ_ONLY_OPTIMAL                = 4,
            SHADER_READ_ONLY_OPTIMAL                       = 5,
            TRANSFER_SRC_OPTIMAL                           = 6,
            TRANSFER_DST_OPTIMAL                           = 7,
            PREINITIALIZED                                 = 8,
            DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL     = 1000117000,
            DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL     = 1000117001,
            PRESENT_SRC_KHR                                = 1000001002,
            SHARED_PRESENT_KHR                             = 1000111000,
            SHADING_RATE_OPTIMAL_NV                        = 1000164003,
            FRAGMENT_DENSITY_MAP_OPTIMAL_EXT               = 1000218000,
            DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR = DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
            DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR = DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
        };
    }
}


#endif

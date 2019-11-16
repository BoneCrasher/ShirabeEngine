#ifndef __SHIRABE_RESOURCES_DEFINITION_H__
#define __SHIRABE_RESOURCES_DEFINITION_H__

#include <cstdint>

namespace engine {
    namespace resources {
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
    }
}

#endif

#include <functional>

#include <graphicsapi/definitions.h>
#include "vulkan/vulkandevicecapabilities.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        VkFormat CVulkanDeviceCapsHelper::convertFormatToVk(EFormat const &aFormat)
        {
            switch(aFormat)
            {
            case EFormat::R32G32B32A32_UINT:        return VkFormat::VK_FORMAT_R32G32B32A32_UINT;
            case EFormat::R32G32B32A32_SINT:        return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
            case EFormat::R32G32B32A32_FLOAT:       return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
            case EFormat::R16G16B16A16_UNORM:       return VkFormat::VK_FORMAT_R16G16B16A16_UNORM;
            case EFormat::R16G16B16A16_SNORM:       return VkFormat::VK_FORMAT_R16G16B16A16_SNORM;
            case EFormat::R16G16B16A16_UINT:        return VkFormat::VK_FORMAT_R16G16B16A16_UINT;
            case EFormat::R16G16B16A16_SINT:        return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
            case EFormat::R16G16B16A16_FLOAT:       return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
            case EFormat::R8G8B8A8_UNORM:           return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
            case EFormat::R8G8B8A8_UNORM_SRGB:      return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
            case EFormat::R8G8B8A8_SNORM:           return VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
            case EFormat::R8G8B8A8_UINT:            return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
            case EFormat::R8G8B8A8_SINT:            return VkFormat::VK_FORMAT_R8G8B8A8_SINT;
            case EFormat::D24_UNORM_S8_UINT:        return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
            case EFormat::D32_FLOAT:                return VkFormat::VK_FORMAT_D32_SFLOAT;
            case EFormat::R32_FLOAT:                return VkFormat::VK_FORMAT_R32_SFLOAT;
            case EFormat::R32_UINT:                 return VkFormat::VK_FORMAT_R32_UINT;
            case EFormat::R32_SINT:                 return VkFormat::VK_FORMAT_R32_SINT;
            case EFormat::D32_FLOAT_S8X24_UINT:     return VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
            case EFormat::BC1_UNORM:                return VkFormat::VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            case EFormat::BC1_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
            case EFormat::BC2_UNORM:                return VkFormat::VK_FORMAT_BC2_UNORM_BLOCK;
            case EFormat::BC2_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC2_SRGB_BLOCK;
            case EFormat::BC3_UNORM:                return VkFormat::VK_FORMAT_BC3_UNORM_BLOCK;
            case EFormat::BC3_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC3_SRGB_BLOCK;
            case EFormat::BC4_UNORM:                return VkFormat::VK_FORMAT_BC4_UNORM_BLOCK;
            case EFormat::BC4_SNORM:                return VkFormat::VK_FORMAT_BC4_SNORM_BLOCK;
            case EFormat::BC5_UNORM:                return VkFormat::VK_FORMAT_BC5_UNORM_BLOCK;
            case EFormat::BC5_SNORM:                return VkFormat::VK_FORMAT_BC5_SNORM_BLOCK;
            case EFormat::BC6H_SF16:                return VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK;
            case EFormat::BC6H_UF16:                return VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK;
            case EFormat::BC7_UNORM:                return VkFormat::VK_FORMAT_BC7_UNORM_BLOCK;
            case EFormat::BC7_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC7_SRGB_BLOCK;
            default:                                return VkFormat::VK_FORMAT_UNDEFINED;
            }

        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EFormat CVulkanDeviceCapsHelper::convertFormatFromVk(VkFormat const &aFormat)
        {
            switch(aFormat)
            {
            case VkFormat::VK_FORMAT_R32G32B32A32_UINT:    return EFormat::R32G32B32A32_UINT;
            case VkFormat::VK_FORMAT_R32G32B32A32_SINT:    return EFormat::R32G32B32A32_SINT;
            case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:  return EFormat::R32G32B32A32_FLOAT;
            case VkFormat::VK_FORMAT_R16G16B16A16_UNORM:   return EFormat::R16G16B16A16_UNORM;
            case VkFormat::VK_FORMAT_R16G16B16A16_SNORM:   return EFormat::R16G16B16A16_SNORM;
            case VkFormat::VK_FORMAT_R16G16B16A16_UINT:    return EFormat::R16G16B16A16_UINT;
            case VkFormat::VK_FORMAT_R16G16B16A16_SINT:    return EFormat::R16G16B16A16_SINT;
            case VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT:  return EFormat::R16G16B16A16_FLOAT;
            case VkFormat::VK_FORMAT_R8G8B8A8_UNORM:       return EFormat::R8G8B8A8_UNORM;
            case VkFormat::VK_FORMAT_R8G8B8A8_SRGB:        return EFormat::R8G8B8A8_UNORM_SRGB;
            case VkFormat::VK_FORMAT_R8G8B8A8_SNORM:       return EFormat::R8G8B8A8_SNORM;
            case VkFormat::VK_FORMAT_R8G8B8A8_UINT:        return EFormat::R8G8B8A8_UINT;
            case VkFormat::VK_FORMAT_R8G8B8A8_SINT:        return EFormat::R8G8B8A8_SINT;
            case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:    return EFormat::D24_UNORM_S8_UINT;
            case VkFormat::VK_FORMAT_D32_SFLOAT:           return EFormat::D32_FLOAT;
            case VkFormat::VK_FORMAT_R32_SFLOAT:           return EFormat::R32_FLOAT;
            case VkFormat::VK_FORMAT_R32_UINT:             return EFormat::R32_UINT;
            case VkFormat::VK_FORMAT_R32_SINT:             return EFormat::R32_SINT;
            case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:   return EFormat::D32_FLOAT_S8X24_UINT;
            case VkFormat::VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return EFormat::BC1_UNORM;
            case VkFormat::VK_FORMAT_BC1_RGBA_SRGB_BLOCK:  return EFormat::BC1_UNORM_SRGB;
            case VkFormat::VK_FORMAT_BC2_UNORM_BLOCK:      return EFormat::BC2_UNORM;
            case VkFormat::VK_FORMAT_BC2_SRGB_BLOCK:       return EFormat::BC2_UNORM_SRGB;
            case VkFormat::VK_FORMAT_BC3_UNORM_BLOCK:      return EFormat::BC3_UNORM;
            case VkFormat::VK_FORMAT_BC3_SRGB_BLOCK:       return EFormat::BC3_UNORM_SRGB;
            case VkFormat::VK_FORMAT_BC4_UNORM_BLOCK:      return EFormat::BC4_UNORM;
            case VkFormat::VK_FORMAT_BC4_SNORM_BLOCK:      return EFormat::BC4_SNORM;
            case VkFormat::VK_FORMAT_BC5_UNORM_BLOCK:      return EFormat::BC5_UNORM;
            case VkFormat::VK_FORMAT_BC5_SNORM_BLOCK:      return EFormat::BC5_SNORM;
            case VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK:    return EFormat::BC6H_SF16;
            case VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK:    return EFormat::BC6H_UF16;
            case VkFormat::VK_FORMAT_BC7_UNORM_BLOCK:      return EFormat::BC7_UNORM;
            case VkFormat::VK_FORMAT_BC7_SRGB_BLOCK:       return EFormat::BC7_UNORM_SRGB;
            default:                                       return EFormat::Undefined;
            }

        }
        //<-----------------------------------------------------------------------------

    }
}

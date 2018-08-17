#include <functional>

#include "Vulkan/VulkanDeviceCapabilities.h"
#include "GraphicsAPI/Definitions.h"

namespace engine {
  namespace vulkan {

    using namespace engine::Resources;
    using engine::Core::BitField;

    VkFormat VulkanDeviceCapsHelper::convertFormatToVk(const Format& fmt) {
      switch(fmt) {
      case Format::R32G32B32A32_UINT:        return VkFormat::VK_FORMAT_R32G32B32A32_UINT;
      case Format::R32G32B32A32_SINT:        return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
      case Format::R32G32B32A32_FLOAT:       return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
      case Format::R16G16B16A16_UNORM:       return VkFormat::VK_FORMAT_R16G16B16A16_UNORM;
      case Format::R16G16B16A16_SNORM:       return VkFormat::VK_FORMAT_R16G16B16A16_SNORM;
      case Format::R16G16B16A16_UINT:        return VkFormat::VK_FORMAT_R16G16B16A16_UINT;
      case Format::R16G16B16A16_SINT:        return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
      case Format::R16G16B16A16_FLOAT:       return VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT;
      case Format::R8G8B8A8_UNORM:           return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
      case Format::R8G8B8A8_UNORM_SRGB:      return VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
      case Format::R8G8B8A8_SNORM:           return VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
      case Format::R8G8B8A8_UINT:            return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
      case Format::R8G8B8A8_SINT:            return VkFormat::VK_FORMAT_R8G8B8A8_SINT;
      case Format::D24_UNORM_S8_UINT:        return VkFormat::VK_FORMAT_D24_UNORM_S8_UINT;
      case Format::D32_FLOAT:                return VkFormat::VK_FORMAT_D32_SFLOAT;
      case Format::R32_FLOAT:                return VkFormat::VK_FORMAT_R32_SFLOAT;
      case Format::R32_UINT:                 return VkFormat::VK_FORMAT_R32_UINT;
      case Format::R32_SINT:                 return VkFormat::VK_FORMAT_R32_SINT;
      case Format::D32_FLOAT_S8X24_UINT:     return VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
      case Format::BC1_UNORM:                return VkFormat::VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
      case Format::BC1_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
      case Format::BC2_UNORM:                return VkFormat::VK_FORMAT_BC2_UNORM_BLOCK;
      case Format::BC2_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC2_SRGB_BLOCK;
      case Format::BC3_UNORM:                return VkFormat::VK_FORMAT_BC3_UNORM_BLOCK;
      case Format::BC3_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC3_SRGB_BLOCK;
      case Format::BC4_UNORM:                return VkFormat::VK_FORMAT_BC4_UNORM_BLOCK;
      case Format::BC4_SNORM:                return VkFormat::VK_FORMAT_BC4_SNORM_BLOCK;
      case Format::BC5_UNORM:                return VkFormat::VK_FORMAT_BC5_UNORM_BLOCK;
      case Format::BC5_SNORM:                return VkFormat::VK_FORMAT_BC5_SNORM_BLOCK;
      case Format::BC6H_SF16:                return VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK;
      case Format::BC6H_UF16:                return VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK;
      case Format::BC7_UNORM:                return VkFormat::VK_FORMAT_BC7_UNORM_BLOCK;
      case Format::BC7_UNORM_SRGB:           return VkFormat::VK_FORMAT_BC7_SRGB_BLOCK;
      }
      return VkFormat::VK_FORMAT_UNDEFINED;
    }

    Format VulkanDeviceCapsHelper::convertFormatFromVk(const VkFormat& fmt) {
      switch(fmt) {
      case VkFormat::VK_FORMAT_R32G32B32A32_UINT:    return Format::R32G32B32A32_UINT;   
      case VkFormat::VK_FORMAT_R32G32B32A32_SINT:    return Format::R32G32B32A32_SINT;   
      case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:  return Format::R32G32B32A32_FLOAT;  
      case VkFormat::VK_FORMAT_R16G16B16A16_UNORM:   return Format::R16G16B16A16_UNORM;  
      case VkFormat::VK_FORMAT_R16G16B16A16_SNORM:   return Format::R16G16B16A16_SNORM;  
      case VkFormat::VK_FORMAT_R16G16B16A16_UINT:    return Format::R16G16B16A16_UINT;   
      case VkFormat::VK_FORMAT_R16G16B16A16_SINT:    return Format::R16G16B16A16_SINT;   
      case VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT:  return Format::R16G16B16A16_FLOAT;  
      case VkFormat::VK_FORMAT_R8G8B8A8_UNORM:       return Format::R8G8B8A8_UNORM;      
      case VkFormat::VK_FORMAT_R8G8B8A8_SRGB:        return Format::R8G8B8A8_UNORM_SRGB; 
      case VkFormat::VK_FORMAT_R8G8B8A8_SNORM:       return Format::R8G8B8A8_SNORM;      
      case VkFormat::VK_FORMAT_R8G8B8A8_UINT:        return Format::R8G8B8A8_UINT;       
      case VkFormat::VK_FORMAT_R8G8B8A8_SINT:        return Format::R8G8B8A8_SINT;       
      case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:    return Format::D24_UNORM_S8_UINT;   
      case VkFormat::VK_FORMAT_D32_SFLOAT:           return Format::D32_FLOAT;           
      case VkFormat::VK_FORMAT_R32_SFLOAT:           return Format::R32_FLOAT;           
      case VkFormat::VK_FORMAT_R32_UINT:             return Format::R32_UINT;            
      case VkFormat::VK_FORMAT_R32_SINT:             return Format::R32_SINT;            
      case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:   return Format::D32_FLOAT_S8X24_UINT;
      case VkFormat::VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return Format::BC1_UNORM;           
      case VkFormat::VK_FORMAT_BC1_RGBA_SRGB_BLOCK:  return Format::BC1_UNORM_SRGB;      
      case VkFormat::VK_FORMAT_BC2_UNORM_BLOCK:      return Format::BC2_UNORM;           
      case VkFormat::VK_FORMAT_BC2_SRGB_BLOCK:       return Format::BC2_UNORM_SRGB;      
      case VkFormat::VK_FORMAT_BC3_UNORM_BLOCK:      return Format::BC3_UNORM;           
      case VkFormat::VK_FORMAT_BC3_SRGB_BLOCK:       return Format::BC3_UNORM_SRGB;      
      case VkFormat::VK_FORMAT_BC4_UNORM_BLOCK:      return Format::BC4_UNORM;           
      case VkFormat::VK_FORMAT_BC4_SNORM_BLOCK:      return Format::BC4_SNORM;           
      case VkFormat::VK_FORMAT_BC5_UNORM_BLOCK:      return Format::BC5_UNORM;           
      case VkFormat::VK_FORMAT_BC5_SNORM_BLOCK:      return Format::BC5_SNORM;           
      case VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK:    return Format::BC6H_SF16;           
      case VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK:    return Format::BC6H_UF16;           
      case VkFormat::VK_FORMAT_BC7_UNORM_BLOCK:      return Format::BC7_UNORM;           
      case VkFormat::VK_FORMAT_BC7_SRGB_BLOCK:       return Format::BC7_UNORM_SRGB;      
      }                                              
      return Format::Undefined;
    }

  }
}
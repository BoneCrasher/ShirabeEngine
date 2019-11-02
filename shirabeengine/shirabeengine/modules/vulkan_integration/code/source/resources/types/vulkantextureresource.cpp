//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::create(CGpiApiDependencyCollection const &aDependencies)
    {
        SHIRABE_UNUSED(aDependencies);

        VkDevice         const &vkLogicalDevice  = getVkContext()->getLogicalDevice();
        VkPhysicalDevice const &vkPhysicalDevice = getVkContext()->getPhysicalDevice();

        STextureDescription const desc = getDescription();

        VkImageType imageType = VkImageType::VK_IMAGE_TYPE_2D;
        if(1 < desc.textureInfo.depth)
        {
            imageType = VkImageType::VK_IMAGE_TYPE_3D;
        }
        else if(1 < desc.textureInfo.height)
        {
            imageType = VkImageType::VK_IMAGE_TYPE_2D;
        }
        else
        {
            imageType = VkImageType::VK_IMAGE_TYPE_1D;
        }

        VkImageUsageFlags imageUsage = {};
        VkImageLayout     layout     = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
        if(desc.gpuBinding.check(EBufferBinding::TextureInput))
        {
            imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if(desc.gpuBinding.check(EBufferBinding::InputAttachment))
        {
            imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
        if(desc.gpuBinding.check(EBufferBinding::CopySource))
        {
            imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if(desc.gpuBinding.check(EBufferBinding::CopyTarget))
        {
            imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if(desc.gpuBinding.check(EBufferBinding::ColorAttachment))
        {
            imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if(desc.gpuBinding.check(EBufferBinding::DepthAttachment))
        {
            imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if(desc.gpuBinding.check(EBufferBinding::PresentSource))
        {
            layout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        VkImageCreateInfo vkImageCreateInfo ={ };
        vkImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vkImageCreateInfo.imageType     = imageType;
        vkImageCreateInfo.extent.width  = desc.textureInfo.width;
        vkImageCreateInfo.extent.height = desc.textureInfo.height;
        vkImageCreateInfo.extent.depth  = desc.textureInfo.depth;
        vkImageCreateInfo.mipLevels     = desc.textureInfo.mipLevels;
        vkImageCreateInfo.arrayLayers   = desc.textureInfo.arraySize;
        vkImageCreateInfo.format        = CVulkanDeviceCapsHelper::convertFormatToVk(desc.textureInfo.format);
        vkImageCreateInfo.usage         = imageUsage;
        vkImageCreateInfo.initialLayout = layout;
        vkImageCreateInfo.tiling        = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
        vkImageCreateInfo.sharingMode   = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
        vkImageCreateInfo.samples       = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        vkImageCreateInfo.flags         = 0;
        vkImageCreateInfo.pNext         = nullptr;

        VkImage vkImage = VK_NULL_HANDLE;

        VkResult result = vkCreateImage(vkLogicalDevice, &vkImageCreateInfo, nullptr, &vkImage);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create texture. Vulkan result: %0", result));
            return { EEngineStatus::Error };
        }

        VkMemoryRequirements vkMemoryRequirements ={ };
        vkGetImageMemoryRequirements(vkLogicalDevice, vkImage, &vkMemoryRequirements);

        VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
        vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;

        CEngineResult<uint32_t> memoryTypeFetch =
                                        CVulkanDeviceCapsHelper::determineMemoryType(
                                                vkPhysicalDevice,
                                                vkMemoryRequirements.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if(not memoryTypeFetch.successful())
        {
            CLog::Error(logTag(), "Could not determine memory type index.");
            return { EEngineStatus::Error };
        }

        vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeFetch.data();

        VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;

        result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &vkImageMemory);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: %0", result));
            return { EEngineStatus::Error };
        }

        result = vkBindImageMemory(vkLogicalDevice, vkImage, vkImageMemory, 0);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to bind image memory on GPU. Vulkan error: %0", result));
            return { EEngineStatus::Error };
        }

        this->handle         = vkImage;
        this->attachedMemory = vkImageMemory;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::load()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::unload()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::destroy()
    {
        VkImage        vkImage         = this->handle;
        VkDeviceMemory vkDeviceMemory  = this->attachedMemory;
        VkDevice       vkLogicalDevice = getVkContext()->getLogicalDevice();

        vkFreeMemory  (vkLogicalDevice, vkDeviceMemory, nullptr);
        vkDestroyImage(vkLogicalDevice, vkImage, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::bind()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::transfer()
    {

    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::unbind()
    {

    }
    //<-----------------------------------------------------------------------------
}

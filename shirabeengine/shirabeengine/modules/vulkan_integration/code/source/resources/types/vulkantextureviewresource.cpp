//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureViewResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
        CVulkanTextureResource const *const textureResource = getVkContext()->getResourceStorage()->extract<CVulkanTextureResource>(aDependencies.at(getDescription().subjacentTextureId));
        if(nullptr == textureResource)
        {
            return { EEngineStatus::Error };
        }

        STextureViewDescription const desc = getDescription();

        uint8_t dimensionCount = 1;
        dimensionCount += (desc.subjacentTextureInfo.height > 1) ? 1 : 0;
        dimensionCount += (desc.subjacentTextureInfo.depth  > 1) ? 1 : 0;

        VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

        switch(dimensionCount)
        {
            case 1:
                if(1 < desc.subjacentTextureInfo.arraySize)
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                }
                else
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                }
                break;
            case 2:
                if(1 < desc.subjacentTextureInfo.arraySize)
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                }
                else
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
                }
                break;
            case 3:
                imageViewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                imageViewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
        }

        VkImage vkImage = textureResource->imageHandle; // TODO
        // VkDeviceMemory const vkDeviceMemory = texture->imageMemory; // TODO: Required?

        VkImageViewCreateInfo vkImageViewCreateInfo ={ };
        vkImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vkImageViewCreateInfo.format                          = CVulkanDeviceCapsHelper::convertFormatToVk(desc.textureFormat);
        vkImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT; // TODO: Care about the specific aspect bits here in more detail.
        vkImageViewCreateInfo.subresourceRange.baseArrayLayer = desc.arraySlices.offset;
        vkImageViewCreateInfo.subresourceRange.layerCount     = static_cast<uint32_t>(desc.arraySlices.length);
        vkImageViewCreateInfo.subresourceRange.baseMipLevel   = desc.mipMapSlices.offset;
        vkImageViewCreateInfo.subresourceRange.levelCount     = static_cast<uint32_t>(desc.mipMapSlices.length);
        vkImageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.image                           = vkImage;
        vkImageViewCreateInfo.viewType                        = imageViewType;
        vkImageViewCreateInfo.flags                           = 0; // Reserved
        vkImageViewCreateInfo.pNext                           = nullptr;

        VkImageView vkImageView = VK_NULL_HANDLE;

        VkResult result = vkCreateImageView(getVkContext()->getLogicalDevice(), &vkImageViewCreateInfo, nullptr, &vkImageView);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create render target view. Vulkan error: {}", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkImageView;

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureViewResource::destroy()
    {
        VkImageView vkImageView     = this->handle;
        VkDevice    vkLogicalDevice = getVkContext()->getLogicalDevice();

        vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

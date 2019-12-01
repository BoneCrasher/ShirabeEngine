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
    CEngineResult<> CVulkanTextureViewResource::create(  STextureViewDescription      const &aDescription
                                                       , STextureViewDependencies     const &aDependencies
                                                       , GpuApiResourceDependencies_t const &aResolvedDependencies)
    {
        if(EGpuApiResourceState::Loaded == getResourceState()
           || EGpuApiResourceState::Loading == getResourceState())
        {
            return EEngineStatus::Ok;
        }

        setResourceState(EGpuApiResourceState::Loading);

        CVkApiResource<STextureView>::create(aDescription, aDependencies, aResolvedDependencies);

        auto const *const textureResource = getVkContext()->getResourceStorage()->extract<CVulkanTextureResource>(aResolvedDependencies.at(aDependencies.subjacentTextureId));
        if(nullptr == textureResource)
        {
            return { EEngineStatus::Error };
        }

        uint8_t dimensionCount = 1;
        dimensionCount += (aDescription.subjacentTextureInfo.height > 1) ? 1 : 0;
        dimensionCount += (aDescription.subjacentTextureInfo.depth  > 1) ? 1 : 0;

        VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

        switch(dimensionCount)
        {
            case 1:
                if(1 < aDescription.subjacentTextureInfo.arraySize)
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                }
                else
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                }
                break;
            case 2:
                if(1 < aDescription.subjacentTextureInfo.arraySize)
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
        vkImageViewCreateInfo.format                          = CVulkanDeviceCapsHelper::convertFormatToVk(aDescription.textureFormat);
        vkImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT; // TODO: Care about the specific aspect bits here in more detail.
        vkImageViewCreateInfo.subresourceRange.baseArrayLayer = aDescription.arraySlices.offset;
        vkImageViewCreateInfo.subresourceRange.layerCount     = static_cast<uint32_t>(aDescription.arraySlices.length);
        vkImageViewCreateInfo.subresourceRange.baseMipLevel   = aDescription.mipMapSlices.offset;
        vkImageViewCreateInfo.subresourceRange.levelCount     = static_cast<uint32_t>(aDescription.mipMapSlices.length);
        vkImageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        vkImageViewCreateInfo.image                           = vkImage;
        vkImageViewCreateInfo.viewType                        = imageViewType;
        vkImageViewCreateInfo.flags                           = 0; // Reserved
        vkImageViewCreateInfo.pNext                           = nullptr;

        VkImageView vkImageView = VK_NULL_HANDLE;

        // CLog::Debug(logTag(), "Creating textureview w/ name {}", aDescription.name);

        VkResult result = vkCreateImageView(getVkContext()->getLogicalDevice(), &vkImageViewCreateInfo, nullptr, &vkImageView);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create render target view. Vulkan error: {}", result));
            return { EEngineStatus::Error };
        }

        this->handle = vkImageView;

        setResourceState(EGpuApiResourceState::Loaded);

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

        // CLog::Debug(logTag(), "Destroying textureview w/ name {}", getCurrentDescriptor()->name);

        vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

        setResourceState(EGpuApiResourceState::Discarded);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

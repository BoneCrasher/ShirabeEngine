//
// Created by dottideveloper on 29.10.19.
//
#include "vulkan_integration/resources/types/vulkanbufferresource.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/vulkandevicecapabilities.h"

namespace engine::vulkan
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::create(GpuApiResourceDependencies_t const &aDependencies)
    {
        SHIRABE_UNUSED(aDependencies);

        VkDevice         const &vkLogicalDevice  = getVkContext()->getLogicalDevice();
        VkPhysicalDevice const &vkPhysicalDevice = getVkContext()->getPhysicalDevice();

        STextureDescription const desc = getDescription();

        VkImage        vkImage               = VK_NULL_HANDLE;
        VkDeviceMemory vkImageMemory         = VK_NULL_HANDLE;
        VkSampler      vkSampler             = VK_NULL_HANDLE;
        VkBuffer       vkStagingBuffer       = VK_NULL_HANDLE;
        VkDeviceMemory vkStagingBufferMemory = VK_NULL_HANDLE;

        VkImageCreateInfo    vkImageCreateInfo                ={ };
        VkMemoryRequirements vkMemoryRequirements             ={ };
        VkMemoryAllocateInfo vkImageMemoryAllocateInfo        ={ };
        VkSamplerCreateInfo  vkSamplerCreateInfo              ={ };
        VkBufferCreateInfo   vkStagingBufferCreateInfo        = {};
        VkMemoryAllocateInfo vkStagingBufferMemoryAllocateInfo={ };

        CEngineResult<uint32_t>                    memoryTypeFetch       = { EEngineStatus::Ok };
        CEngineResult<SVulkanBufferCreationResult> stagingBufferCreation = { EEngineStatus::Ok };
        SVulkanBufferCreationResult                bufferCreationResult  = {};

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

        VkResult result = vkCreateImage(vkLogicalDevice, &vkImageCreateInfo, nullptr, &vkImage);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to create texture. Vulkan result: %0", result));
            goto fail;
        }

        vkGetImageMemoryRequirements(vkLogicalDevice, vkImage, &vkMemoryRequirements);

        vkImageMemoryAllocateInfo.sType         = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkImageMemoryAllocateInfo.allocationSize= vkMemoryRequirements.size;

        memoryTypeFetch = CVulkanDeviceCapsHelper::determineMemoryType(
                                   vkPhysicalDevice,
                                   vkMemoryRequirements.memoryTypeBits,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if(not memoryTypeFetch.successful())
        {
            CLog::Error(logTag(), "Could not determine memory type index.");
            goto fail;
        }

        vkImageMemoryAllocateInfo.memoryTypeIndex= memoryTypeFetch.data();

        result = vkAllocateMemory(vkLogicalDevice, &vkImageMemoryAllocateInfo, nullptr, &vkImageMemory);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: %0", result));
            goto fail;
        }

        result = vkBindImageMemory(vkLogicalDevice, vkImage, vkImageMemory, 0);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to bind image memory on GPU. Vulkan error: %0", result));
            goto fail;
        }

        vkSamplerCreateInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vkSamplerCreateInfo.pNext                   = nullptr;
        vkSamplerCreateInfo.minFilter               = VK_FILTER_LINEAR;
        vkSamplerCreateInfo.magFilter               = VK_FILTER_LINEAR;
        vkSamplerCreateInfo.addressModeU            = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkSamplerCreateInfo.addressModeV            = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkSamplerCreateInfo.addressModeW            = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vkSamplerCreateInfo.anisotropyEnable        = VK_FALSE;
        vkSamplerCreateInfo.maxAnisotropy           = 16;
        vkSamplerCreateInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE; // Make sure we use 0.0 to 1.0 range UVW-Coords.
        vkSamplerCreateInfo.compareEnable           = VK_FALSE;
        vkSamplerCreateInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        vkSamplerCreateInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        vkSamplerCreateInfo.mipLodBias              = 0.0f;
        vkSamplerCreateInfo.minLod                  = 0.0f;
        vkSamplerCreateInfo.maxLod                  = 0.0f;

        result = vkCreateSampler(vkLogicalDevice, &vkSamplerCreateInfo, nullptr, &vkSampler);
        if(VkResult::VK_SUCCESS != result)
        {
            CLog::Error(logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: %0", result));
            goto fail;
        }

        stagingBufferCreation = __createVkBuffer(vkPhysicalDevice
                                                 , vkLogicalDevice
                                                 , desc.textureInfo.width * desc.textureInfo.height  * 4
                                                 , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
                                                 , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if(not stagingBufferCreation.successful())
        {
            CLog::Error(logTag(), CString::format("Failed to create buffer and allocate memory on GPU. Vulkan error: %0", result));
            goto fail;
        }

        bufferCreationResult = stagingBufferCreation.data();
        vkStagingBuffer       = bufferCreationResult.buffer;
        vkStagingBufferMemory = bufferCreationResult.attachedMemory;

        success:
        this->imageHandle         = vkImage;
        this->imageMemory         = vkImageMemory;
        this->attachedSampler     = vkSampler;
        this->stagingBuffer       = vkStagingBuffer;
        this->stagingBufferMemory = vkStagingBufferMemory;

        return { EEngineStatus::Ok };

        fail:
        vkDestroyImage  (vkLogicalDevice, vkImage,               nullptr);
        vkFreeMemory    (vkLogicalDevice, vkImageMemory,         nullptr);
        vkDestroySampler(vkLogicalDevice, vkSampler,             nullptr);
        vkDestroyBuffer (vkLogicalDevice, vkStagingBuffer,       nullptr);
        vkFreeMemory    (vkLogicalDevice, vkStagingBufferMemory, nullptr);
        return { EEngineStatus::Error };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::load()
    {
        CResourceDataSource const &dataSource = getDescription().initialData[0];
        ByteBuffer          const &data       = dataSource.getData();

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::unload()
    {
        vkDestroyBuffer(getVkContext()->getLogicalDevice(), this->stagingBuffer, nullptr);
        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::destroy()
    {
        VkImage        vkImage         = this->imageHandle;
        VkDeviceMemory vkImageMemory   = this->imageMemory;
        VkSampler      vkSampler       = this->attachedSampler;
        VkBuffer       vkBuffer        = this->stagingBuffer;
        VkDeviceMemory vkBufferMemory  = this->stagingBufferMemory;

        VkDevice       vkLogicalDevice = getVkContext()->getLogicalDevice();

        vkDestroySampler(vkLogicalDevice, vkSampler,      nullptr);
        vkFreeMemory    (vkLogicalDevice, vkImageMemory,  nullptr);
        vkDestroyImage  (vkLogicalDevice, vkImage,        nullptr);
        vkFreeMemory    (vkLogicalDevice, vkBufferMemory, nullptr);
        vkDestroyBuffer (vkLogicalDevice, vkBuffer,       nullptr);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CVulkanTextureResource::transfer()
    {
        Shared<IVkFrameContext> frameContext = getVkContext()->getVkCurrentFrameContext();

        VkBufferImageCopy region {};
        region.bufferOffset      = 0;
        region.bufferRowLength   = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = { getDescription().textureInfo.width, getDescription().textureInfo.height, 1 };

        // vkCmdCopyBufferToImage(frameContext->getTransferCommandBuffer()
        //                        , this->stagingBuffer
        //                        , this->imageHandle
        //                        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        //                        , 1, &region);

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

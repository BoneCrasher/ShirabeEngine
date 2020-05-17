#ifndef __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "renderer/resource_management/resourcetypes.h"
#include "renderer/resource_management/extensibility.h"
#include "renderer/vulkan_resources/resources/types/vulkanbufferresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanTextureResource;
    }

    namespace resources
    {
        template <> struct SLogicalToGpuApiResourceTypeMap<STexture>  { using TGpuApiResource = vulkan::SVulkanTextureResource;  };
    }

    namespace texture_log
    {
        SHIRABE_DECLARE_LOG_TAG(SVulkanTextureResource);
    }

    namespace vulkan
    {
        using namespace resources;

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanTextureResource
        {
            static constexpr bool is_loadable      = true;
            static constexpr bool is_unloadable    = true;
            static constexpr bool is_transferrable = true;

            struct Handles_t
            {
                VkBuffer       stagingBuffer;
                VkDeviceMemory stagingBufferMemory;
                VkImage        imageHandle;
                VkDeviceMemory imageMemory;
                VkSampler      attachedSampler;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(STextureDescription const &aDescription
                                          , Handles_t                 &aGpuApiHandles
                                          , TResourceManager          *aResourceManager
                                          , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus load(STextureDescription const &aDescription
                                    , Handles_t                 &aGpuApiHandles
                                    , TResourceManager          *aResourceManager
                                    , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus transfer(STextureDescription const &aDescription
                                        , Handles_t                 &aGpuApiHandles
                                        , TResourceManager          *aResourceManager
                                        , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus unload(STextureDescription const &aDescription
                                      , Handles_t                 &aGpuApiHandles
                                      , TResourceManager          *aResourceManager
                                      , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(STextureDescription const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , TResourceManager          *aResourceManager
                                            , IVkGlobalContext          *aVulkanEnvironment);
        };

        auto __performImageLayoutTransfer(IVkGlobalContext                          *aVulkanEnvironment
                                          , SVulkanTextureResource::Handles_t const &aTexture
                                          , CRange                            const &aArrayRange
                                          , CRange                            const &aMipRange
                                          , VkImageAspectFlags                const &aAspectFlags
                                          , VkImageLayout                     const &aSourceLayout
                                          , VkImageLayout                     const &aTargetLayout) -> EEngineStatus;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using TextureResourceState_t  = SResourceState<STexture>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanTextureResource::initialize(STextureDescription const &aDescription
                                                       , Handles_t                 &aGpuApiHandles
                                                       , TResourceManager          *aResourceManager
                                                       , IVkGlobalContext          *aVulkanEnvironment)
        {
            /// CLog::Debug(logTag(), "Creating texture w/ name {}", aDescription.name);

            VkDevice         const &vkLogicalDevice  = aVulkanEnvironment->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = aVulkanEnvironment->getPhysicalDevice();

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
            if(1 < aDescription.textureInfo.depth)
            {
                imageType = VkImageType::VK_IMAGE_TYPE_3D;
            }
            else if(1 < aDescription.textureInfo.height)
            {
                imageType = VkImageType::VK_IMAGE_TYPE_2D;
            }
            else
            {
                imageType = VkImageType::VK_IMAGE_TYPE_1D;
            }

            VkImageUsageFlags imageUsage = {};
            VkImageLayout     layout     = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
            if(aDescription.gpuBinding.check(EBufferBinding::TextureInput))
            {
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
            }
            if(aDescription.gpuBinding.check(EBufferBinding::InputAttachment))
            {
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            }
            if(aDescription.gpuBinding.check(EBufferBinding::CopySource))
            {
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            }
            if(aDescription.gpuBinding.check(EBufferBinding::CopyTarget))
            {
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                layout      = VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            }
            if(aDescription.gpuBinding.check(EBufferBinding::ColorAttachment))
            {
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
            if(aDescription.gpuBinding.check(EBufferBinding::DepthAttachment))
            {
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
            if(aDescription.gpuBinding.check(EBufferBinding::PresentSource))
            {
                layout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }

            vkImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            vkImageCreateInfo.imageType     = imageType;
            vkImageCreateInfo.extent.width  = aDescription.textureInfo.width;
            vkImageCreateInfo.extent.height = aDescription.textureInfo.height;
            vkImageCreateInfo.extent.depth  = aDescription.textureInfo.depth;
            vkImageCreateInfo.mipLevels     = aDescription.textureInfo.mipLevels;
            vkImageCreateInfo.arrayLayers   = aDescription.textureInfo.arraySize;
            vkImageCreateInfo.format        = CVulkanDeviceCapsHelper::convertFormatToVk(aDescription.textureInfo.format);
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
                CLog::Error(texture_log::logTag(), CString::format("Failed to create texture. Vulkan result: {}", result));
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
                CLog::Error(texture_log::logTag(), "Could not determine memory type index.");
                goto fail;
            }

            vkImageMemoryAllocateInfo.memoryTypeIndex= memoryTypeFetch.data();

            result = vkAllocateMemory(vkLogicalDevice, &vkImageMemoryAllocateInfo, nullptr, &vkImageMemory);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(texture_log::logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: {}", result));
                goto fail;
            }

            result = vkBindImageMemory(vkLogicalDevice, vkImage, vkImageMemory, 0);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(texture_log::logTag(), CString::format("Failed to bind image memory on GPU. Vulkan error: {}", result));
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
                CLog::Error(texture_log::logTag(), CString::format("Failed to create sampler. Vulkan error: {}", result));
                goto fail;
            }

            stagingBufferCreation = __createVkBuffer(vkPhysicalDevice
                                                     , vkLogicalDevice
                                                     , aDescription.textureInfo.width * aDescription.textureInfo.height  * 4
                                                     , VK_BUFFER_USAGE_TRANSFER_SRC_BIT
                                                     , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            if(not stagingBufferCreation.successful())
            {
                CLog::Error(texture_log::logTag(), CString::format("Failed to create buffer and allocate memory on GPU. Vulkan error: {}", result));
                goto fail;
            }

            bufferCreationResult = stagingBufferCreation.data();
            vkStagingBuffer       = bufferCreationResult.buffer;
            vkStagingBufferMemory = bufferCreationResult.attachedMemory;

            success:
            aGpuApiHandles.imageHandle         = vkImage;
            aGpuApiHandles.imageMemory         = vkImageMemory;
            aGpuApiHandles.attachedSampler     = vkSampler;
            aGpuApiHandles.stagingBuffer       = vkStagingBuffer;
            aGpuApiHandles.stagingBufferMemory = vkStagingBufferMemory;

            // getVkContext()->registerDebugObjectName((uint64_t)this->imageHandle,         VK_OBJECT_TYPE_IMAGE,         aDescription.name);
            // getVkContext()->registerDebugObjectName((uint64_t)this->imageMemory,         VK_OBJECT_TYPE_DEVICE_MEMORY, std::string(aDescription.name) + "_Memory");
            // getVkContext()->registerDebugObjectName((uint64_t)this->attachedSampler,     VK_OBJECT_TYPE_SAMPLER,       std::string(aDescription.name) + "_Sampler");
            // getVkContext()->registerDebugObjectName((uint64_t)this->stagingBuffer,       VK_OBJECT_TYPE_BUFFER,        std::string(aDescription.name) + "_StagingBuffer");
            // getVkContext()->registerDebugObjectName((uint64_t)this->stagingBufferMemory, VK_OBJECT_TYPE_DEVICE_MEMORY, std::string(aDescription.name) + "_StagingBufferMemory");

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
        template <typename TResourceManager>
        EEngineStatus SVulkanTextureResource::load(STextureDescription const &aDescription
                                                 , Handles_t                 &aGpuApiHandles
                                                 , TResourceManager          *aResourceManager
                                                 , IVkGlobalContext          *aVulkanEnvironment)
        {
            VkDevice device = aVulkanEnvironment->getLogicalDevice();

            if(aDescription.initialData)
            {
                ByteBuffer const &data = aDescription.initialData();

                void *mappedData = nullptr;
                vkMapMemory(device, aGpuApiHandles.stagingBufferMemory, 0, data.size(), 0, &mappedData);
                memcpy(mappedData, data.data(), data.size());
                vkUnmapMemory(device, aGpuApiHandles.stagingBufferMemory);
            }

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanTextureResource::transfer(STextureDescription const &aDescription
                                                     , Handles_t                 &aGpuApiHandles
                                                     , TResourceManager          *aResourceManager
                                                     , IVkGlobalContext          *aVulkanEnvironment)
        {
            Shared<IVkFrameContext> frameContext = aVulkanEnvironment->getVkCurrentFrameContext();

            VkBufferImageCopy region {};
            region.bufferOffset      = 0;
            region.bufferRowLength   = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel       = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount     = 1;

            region.imageOffset = {0, 0, 0};
            region.imageExtent = { aDescription.textureInfo.width, aDescription.textureInfo.height, 1 };

            EEngineStatus const layoutTransferForTransfer =
                                    __performImageLayoutTransfer(aVulkanEnvironment
                                                                 , aGpuApiHandles
                                                                 , CRange(0, 1)
                                                                 , CRange(0, 1)
                                                                 , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                                                 , VK_IMAGE_LAYOUT_UNDEFINED
                                                                 , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
            EngineStatusPrintOnError(layoutTransferForTransfer, texture_log::logTag(), "Failed to transfer texture.");
            SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransferForTransfer);

            vkCmdCopyBufferToImage(frameContext->getTransferCommandBuffer()
                                 , aGpuApiHandles.stagingBuffer
                                 , aGpuApiHandles.imageHandle
                                 , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                                 , 1, &region);

            EEngineStatus const layoutTransferForRead =
                                    __performImageLayoutTransfer(aVulkanEnvironment
                                                                 , aGpuApiHandles
                                                                 , CRange(0, 1)
                                                                 , CRange(0, 1)
                                                                 , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
                                                                 , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                                                                 , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            EngineStatusPrintOnError(layoutTransferForRead, texture_log::logTag(), "Failed to transfer texture.");
            SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransferForRead);

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanTextureResource::unload(STextureDescription const &aDescription
                                                   , Handles_t                 &aGpuApiHandles
                                                   , TResourceManager          *aResourceManager
                                                   , IVkGlobalContext          *aVulkanEnvironment)
        {
            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanTextureResource::deinitialize(STextureDescription const &aDescription
                                                         , Handles_t                 &aGpuApiHandles
                                                         , TResourceManager          *aResourceManager
                                                         , IVkGlobalContext          *aVulkanEnvironment)
        {
            VkImage        vkImage         = aGpuApiHandles.imageHandle;
            VkDeviceMemory vkImageMemory   = aGpuApiHandles.imageMemory;
            VkSampler      vkSampler       = aGpuApiHandles.attachedSampler;
            VkBuffer       vkBuffer        = aGpuApiHandles.stagingBuffer;
            VkDeviceMemory vkBufferMemory  = aGpuApiHandles.stagingBufferMemory;

            VkDevice       vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            // CLog::Debug(logTag(), "Destroying texture w/ name {}", getCurrentDescriptor()->name);

            vkDestroySampler(vkLogicalDevice, vkSampler,      nullptr);
            vkFreeMemory    (vkLogicalDevice, vkImageMemory,  nullptr);
            vkDestroyImage  (vkLogicalDevice, vkImage,        nullptr);
            vkFreeMemory    (vkLogicalDevice, vkBufferMemory, nullptr);
            vkDestroyBuffer (vkLogicalDevice, vkBuffer,       nullptr);

            aGpuApiHandles.imageHandle         = VK_NULL_HANDLE;
            aGpuApiHandles.imageMemory         = VK_NULL_HANDLE;
            aGpuApiHandles.attachedSampler     = VK_NULL_HANDLE;
            aGpuApiHandles.stagingBuffer       = VK_NULL_HANDLE;
            aGpuApiHandles.stagingBufferMemory = VK_NULL_HANDLE;

            return { EEngineStatus::Ok };

        }
        //<-----------------------------------------------------------------------------
    }
}

#endif

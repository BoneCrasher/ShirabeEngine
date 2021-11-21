#ifndef __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTURE_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanRHIImageResource;
    }

    namespace rhi
    {
        template <> struct SRHIResourceMap<SRHIImage>  { using TMappedRHIResource = vulkan::SVulkanRHIImageResource;  };
    }

    namespace texture_log
    {
        SHIRABE_DECLARE_LOG_TAG(SVulkanRHIImage)
    }

    namespace vulkan
    {
        using namespace rhi;

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIImageResource
        {
            struct Handles_t
            {
                VkImage   imageHandle;
                VkSampler attachedSampler;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIImageDescription const &aDescription
                                            , Handles_t                 &aGpuApiHandles
                                            , TResourceManager          *aResourceManager
                                            , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIImageDescription const &aDescription
                                              , Handles_t                 &aGpuApiHandles
                                              , TResourceManager          *aResourceManager
                                              , IVkGlobalContext          *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus bindImageAndMemory(TResourceManager   *aResourceManager
                                                    , IVkGlobalContext *aVulkanEnvironment
                                                    , VkImage           aVkImage
                                                    , VkDeviceMemory    aVkMemory
                                                    , VkDeviceSize      aVkMemoryOffset = 0);
        };

        auto __performImageLayoutTransfer(IVkGlobalContext                          *aVulkanEnvironment
                                          , SVulkanRHIImageResource::Handles_t const &aTexture
                                          , CRange                            const &aArrayRange
                                          , CRange                            const &aMipRange
                                          , VkImageAspectFlags                const &aAspectFlags
                                          , VkImageLayout                     const &aSourceLayout
                                          , VkImageLayout                     const &aTargetLayout) -> EEngineStatus;

        //<--------
        // ---------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIImageResourceState_t = SRHIResourceState<SRHIImage>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIImageResource::initialize(SRHIImageDescription const &aDescription
                                                          , Handles_t                 &aGpuApiHandles
                                                          , TResourceManager          *aResourceManager
                                                          , IVkGlobalContext          *aVulkanEnvironment)
        {
            /// CLog::Debug(logTag(), "Creating texture w/ name {}", aDescription.name);

            VkDevice         const &vkLogicalDevice  = aVulkanEnvironment->getLogicalDevice();
            VkPhysicalDevice const &vkPhysicalDevice = aVulkanEnvironment->getPhysicalDevice();

            VkImage        vkImage   = VK_NULL_HANDLE;
            VkSampler      vkSampler = VK_NULL_HANDLE;

            VkImageCreateInfo    vkImageCreateInfo;
            VkMemoryRequirements vkMemoryRequirements;
            VkSamplerCreateInfo  vkSamplerCreateInfo;

            auto memoryTypeFetch       = CEngineResult<uint32_t>(EEngineStatus::Ok);
            auto stagingBufferCreation = CEngineResult<std::tuple<VkBuffer, VkMemoryRequirements>>(EEngineStatus::Ok);
            auto bufferCreationResult  = std::tuple<VkBuffer, VkMemoryRequirements>();

            VkImageType imageType = VkImageType::VK_IMAGE_TYPE_MAX_ENUM;
            if(1 < aDescription.imageInfo.depth)
            {
                imageType = VkImageType::VK_IMAGE_TYPE_3D;
            }
            else if(1 < aDescription.imageInfo.height)
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
            vkImageCreateInfo.extent.width  = aDescription.imageInfo.width;
            vkImageCreateInfo.extent.height = aDescription.imageInfo.height;
            vkImageCreateInfo.extent.depth  = aDescription.imageInfo.depth;
            vkImageCreateInfo.mipLevels     = aDescription.imageInfo.mipLevels;
            vkImageCreateInfo.arrayLayers   = aDescription.imageInfo.arraySize;
            vkImageCreateInfo.format        = CVulkanDeviceCapsHelper::convertFormatToVk(aDescription.imageInfo.format);
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
                CLog::Error(texture_log::logTag(), StaticStringHelpers::format("Failed to create texture. Vulkan result: {}", result));
                goto fail;
            }

            vkGetImageMemoryRequirements(vkLogicalDevice, vkImage, &vkMemoryRequirements);

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
                CLog::Error(texture_log::logTag(), StaticStringHelpers::format("Failed to create sampler. Vulkan error: {}", result));
                goto fail;
            }

            success:
            aGpuApiHandles.imageHandle     = vkImage;
            aGpuApiHandles.attachedSampler = vkSampler;

            // getVkContext()->registerDebugObjectName((uint64_t)this->imageHandle,     VK_OBJECT_TYPE_IMAGE,         aDescription.name);
            // getVkContext()->registerDebugObjectName((uint64_t)this->attachedSampler, VK_OBJECT_TYPE_SAMPLER,       std::string(aDescription.name) + "_Sampler");

            return EEngineStatus::Ok;

            fail:
            vkDestroyImage  (vkLogicalDevice, vkImage,   nullptr);
            vkDestroySampler(vkLogicalDevice, vkSampler, nullptr);

            return EEngineStatus::Error;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIImageResource::deinitialize(SRHIImageDescription const &aDescription
                                                            , Handles_t                 &aGpuApiHandles
                                                            , TResourceManager          *aResourceManager
                                                            , IVkGlobalContext          *aVulkanEnvironment)
        {
            SHIRABE_UNUSED(aDescription);
            SHIRABE_UNUSED(aResourceManager);

            VkImage   vkImage   = aGpuApiHandles.imageHandle;
            VkSampler vkSampler = aGpuApiHandles.attachedSampler;

            VkDevice  vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            // CLog::Debug(logTag(), "Destroying texture w/ name {}", getCurrentDescriptor()->name);

            vkDestroySampler(vkLogicalDevice, vkSampler, nullptr);
            vkDestroyImage  (vkLogicalDevice, vkImage,   nullptr);

            aGpuApiHandles.imageHandle     = VK_NULL_HANDLE;
            aGpuApiHandles.attachedSampler = VK_NULL_HANDLE;

            return EEngineStatus::Ok;

        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIImageResource::bindImageAndMemory(TResourceManager   *aResourceManager
                                                                  , IVkGlobalContext *aVulkanEnvironment
                                                                  , VkImage           aVkImage
                                                                  , VkDeviceMemory    aVkMemory
                                                                  , VkDeviceSize      aVkMemoryOffset)
        {
            VkDevice vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();
            VkResult const result = vkBindImageMemory(vkLogicalDevice, aVkImage, aVkMemory, aVkMemoryOffset);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(texture_log::logTag(), StaticStringHelpers::format("Failed to bind image memory on GPU. Vulkan error: {}", result));
                return EEngineStatus::Error;
            }
            return EEngineStatus::Ok;
        }

        //<-----------------------------------------------------------------------------
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        //template <typename TResourceManager>
        //EEngineStatus SVulkanRHIImageResource::load(SRHIImageDescription const &aDescription
        //                                         , Handles_t                 &aGpuApiHandles
        //                                         , TResourceManager          *aResourceManager
        //                                         , IVkGlobalContext          *aVulkanEnvironment)
        //{
        //    VkDevice device = aVulkanEnvironment->getLogicalDevice();
//
        //    if(aDescription.initialData)
        //    {
        //        ByteBuffer const &data = aDescription.initialData();
//
        //        void *mappedData = nullptr;
        //        vkMapMemory(device, aGpuApiHandles.stagingBufferMemory, 0, data.size(), 0, &mappedData);
        //        memcpy(mappedData, data.data(), data.size());
        //        vkUnmapMemory(device, aGpuApiHandles.stagingBufferMemory);
        //    }
//
        //    return { EEngineStatus::Ok };
        //}
        ////<-----------------------------------------------------------------------------
//
        ////<-----------------------------------------------------------------------------
        ////
        ////<-----------------------------------------------------------------------------
        //template <typename TResourceManager>
        //EEngineStatus SVulkanRHIImageResource::transfer(SRHIImageDescription const &aDescription
        //                                             , Handles_t                 &aGpuApiHandles
        //                                             , TResourceManager          *aResourceManager
        //                                             , IVkGlobalContext          *aVulkanEnvironment)
        //{
        //    Shared<IVkFrameContext> frameContext = aVulkanEnvironment->getVkCurrentFrameContext();
//
        //    VkBufferImageCopy region {};
        //    region.bufferOffset      = 0;
        //    region.bufferRowLength   = 0;
        //    region.bufferImageHeight = 0;
//
        //    region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        //    region.imageSubresource.mipLevel       = 0;
        //    region.imageSubresource.baseArrayLayer = 0;
        //    region.imageSubresource.layerCount     = 1;
//
        //    region.imageOffset = {0, 0, 0};
        //    region.imageExtent = { aDescription.textureInfo.width, aDescription.textureInfo.height, 1 };
//
        //    EEngineStatus const layoutTransferForTransfer =
        //                            __performImageLayoutTransfer(aVulkanEnvironment
        //                                                         , aGpuApiHandles
        //                                                         , CRange(0, 1)
        //                                                         , CRange(0, 1)
        //                                                         , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
        //                                                         , VK_IMAGE_LAYOUT_UNDEFINED
        //                                                         , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        //    EngineStatusPrintOnError(layoutTransferForTransfer, texture_log::logTag(), "Failed to transfer texture.");
        //    SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransferForTransfer);
//
        //    vkCmdCopyBufferToImage(frameContext->getTransferCommandBuffer()
        //                         , aGpuApiHandles.stagingBuffer
        //                         , aGpuApiHandles.imageHandle
        //                         , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        //                         , 1, &region);
//
        //    EEngineStatus const layoutTransferForRead =
        //                            __performImageLayoutTransfer(aVulkanEnvironment
        //                                                         , aGpuApiHandles
        //                                                         , CRange(0, 1)
        //                                                         , CRange(0, 1)
        //                                                         , VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
        //                                                         , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        //                                                         , VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        //    EngineStatusPrintOnError(layoutTransferForRead, texture_log::logTag(), "Failed to transfer texture.");
        //    SHIRABE_RETURN_RESULT_ON_ERROR(layoutTransferForRead);
//
        //    return { EEngineStatus::Ok };
        //}
        ////<-----------------------------------------------------------------------------
//
        ////<-----------------------------------------------------------------------------
        ////
        ////<-----------------------------------------------------------------------------
        //template <typename TResourceManager>
        //EEngineStatus SVulkanRHIImageResource::unload(SRHIImageDescription const &aDescription
        //                                           , Handles_t                 &aGpuApiHandles
        //                                           , TResourceManager          *aResourceManager
        //                                           , IVkGlobalContext          *aVulkanEnvironment)
        //{
        //    return { EEngineStatus::Ok };
        //}
        //<-----------------------------------------------------------------------------

    }
}

#endif

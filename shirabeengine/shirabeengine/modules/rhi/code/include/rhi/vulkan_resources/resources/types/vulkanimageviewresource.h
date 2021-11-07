#ifndef __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include "rhi/resource_management/resourcetypes.h"
#include "rhi/resource_management/extensibility.h"
#include "rhi/vulkan_resources/resources/cvkapiresource.h"
#include "rhi/vulkan_resources/resources/ivkglobalcontext.h"
#include "rhi/vulkan_resources/resources/types/vulkanimageresource.h"

namespace engine
{
    namespace vulkan
    {
        struct SVulkanRHIImageViewResource;
    }

    namespace rhi
    {
        template <> struct SRHIResourceMap<SRHIImageView>  { using TMappedRHIResource = vulkan::SVulkanRHIImageViewResource;  };
    }

    namespace vulkan
    {
        using namespace rhi;

        namespace textureview_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanTextureViewResource)
        }

        /**
         * The SVulkanRHIImageResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanRHIImageViewResource
        {
            struct Handles_t
            {
                VkImageView handle;
            };

            template <typename TResourceManager>
            static EEngineStatus initialize(SRHIImageViewDescription const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                            , TResourceManager              *aResourceManager
                                            , IVkGlobalContext              *aVulkanEnvironment);

            template <typename TResourceManager>
            static EEngineStatus deinitialize(SRHIImageViewDescription const &aDescription
                                              , Handles_t                     &aGpuApiHandles
                                              , TResourceManager              *aResourceManager
                                              , IVkGlobalContext              *aVulkanEnvironment);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using RHIImageViewResourceState_t = SRHIResourceState<SRHIImageView>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIImageViewResource::initialize(SRHIImageViewDescription const &aDescription
                                                              , Handles_t                    &aGpuApiHandles
                                                              , TResourceManager             *aResourceManager
                                                              , IVkGlobalContext             *aVulkanEnvironment)
        {
            OptionalRef_t<RHIImageResourceState_t> const textureOpt = aResourceManager->getResource(aDescription.subjacentImageId, aVulkanEnvironment);
            if(not textureOpt.has_value())
            {
                return EEngineStatus::ResourceError_DependencyNotFound;
            }
            RHIImageResourceState_t const &textureState = *textureOpt;

            uint8_t dimensionCount = 1;
            dimensionCount += (textureState.rhiCreateDesc.imageInfo.height > 1) ? 1 : 0;
            dimensionCount += (textureState.rhiCreateDesc.imageInfo.depth  > 1) ? 1 : 0;

            VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

            switch(dimensionCount)
            {
                case 1:
                    if(1 < textureState.rhiCreateDesc.imageInfo.arraySize)
                    {
                        imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                    }
                    else
                    {
                        imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                    }
                    break;
                case 2:
                    if(1 < textureState.rhiCreateDesc.imageInfo.arraySize)
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

            VkImage vkImage = textureState.rhiHandles.imageHandle; // TODO
            // VkDeviceMemory const vkDeviceMemory = texture->imageMemory; // TODO: Required?

            auto const isDepthStencilImageOp = [](EFormat const aFormat) -> bool
                {
                switch(aFormat)
                {
                    case EFormat::D24_UNORM_S8_UINT:
                    case EFormat::D32_FLOAT:
                    case EFormat::D32_FLOAT_S8X24_UINT:
                        return true;
                    default:
                        return false;
                }
                };

            VkImageViewCreateInfo vkImageViewCreateInfo ={ };
            vkImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            vkImageViewCreateInfo.format                          = CVulkanDeviceCapsHelper::convertFormatToVk(aDescription.imageFormat);
            vkImageViewCreateInfo.subresourceRange.aspectMask     = isDepthStencilImageOp(aDescription.imageFormat) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
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

            VkResult result = vkCreateImageView(aVulkanEnvironment->getLogicalDevice(), &vkImageViewCreateInfo, nullptr, &vkImageView);
            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(textureview_log::logTag(), CString::format("Failed to create render target view. Vulkan error: {}", result));
                return EEngineStatus::Error;
            }

            aGpuApiHandles.handle = vkImageView;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        template <typename TResourceManager>
        EEngineStatus SVulkanRHIImageViewResource::deinitialize(SRHIImageViewDescription const &aDescription
                                                                , Handles_t                    &aGpuApiHandles
                                                                , TResourceManager             *aResourceManager
                                                                , IVkGlobalContext             *aVulkanEnvironment)
        {
            VkImageView vkImageView     = aGpuApiHandles.handle;
            VkDevice    vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            // CLog::Debug(logTag(), "Destroying textureview w/ name {}", getCurrentDescriptor()->name);

            vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

            aGpuApiHandles.handle = VK_NULL_HANDLE;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

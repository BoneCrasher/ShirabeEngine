#ifndef __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__
#define __SHIRABE_VULKAN_TEXTUREVIEW_RESOURCE_H__

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <base/declaration.h>
#include <resources/resourcetypes.h>
#include <resources/cresourcemanager.h>

#include "vulkan_integration/resources/cvkapiresource.h"
#include "vulkan_integration/resources/ivkglobalcontext.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/rendering/vulkanrendercontext.h"

namespace engine
{
    namespace vulkan
    {
        using namespace resources;

        namespace textureview_log
        {
            SHIRABE_DECLARE_LOG_TAG(SVulkanTextureViewResource);
        }

        /**
         * The SVulkanTextureResource struct describes the relevant data to deal
         * with textures inside the vulkan API.
         */
        struct SVulkanTextureViewResource
        {
            struct Handles_t
            {
                VkImageView handle;
            };

            static EEngineStatus initialize(STextureViewDescription const &aDescription
                                          , Handles_t                     &aGpuApiHandles
                                          , IVkGlobalContext              *aVulkanEnvironment
                                          , VulkanResourceManager_t       *aResourceManager);

            static EEngineStatus deinitialize(STextureViewDescription const &aDescription
                                            , Handles_t                     &aGpuApiHandles
                                            , IVkGlobalContext              *aVulkanEnvironment
                                            , VulkanResourceManager_t       *aResourceManager);
        };

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        using TextureViewResourceState_t = SResourceState<STextureView,  SVulkanTextureViewResource>;

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        static EEngineStatus SVulkanTextureViewResource::initialize(STextureViewDescription const &aDescription
                                                                    , Handles_t                     &aGpuApiHandles
                                                                    , IVkGlobalContext              *aVulkanEnvironment
                                                                    , VulkanResourceManager_t   *aResourceManager)
        {
            OptionalRef_t<TextureResourceState_t> const textureOpt = aResourceManager->getResource(aDescription.subjacentBufferId);
            if(not textureOpt.has_value())
            {
                return EEngineStatus::ResourceError_DependencyNotFound;
            }
            TextureResourceState_t textureState = *textureOpt;

            uint8_t dimensionCount = 1;
            dimensionCount += (textureState.description.textureInfo.height > 1) ? 1 : 0;
            dimensionCount += (textureState.description.textureInfo.depth  > 1) ? 1 : 0;

            VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

            switch(dimensionCount)
            {
                case 1:
                    if(1 < textureState.description.textureInfo.arraySize)
                    {
                        imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                    }
                    else
                    {
                        imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                    }
                    break;
                case 2:
                    if(1 < textureState.description.textureInfo.arraySize)
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

            VkImage vkImage = textureState.gpuApiHandles.imageHandle; // TODO
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
            vkImageViewCreateInfo.format                          = CVulkanDeviceCapsHelper::convertFormatToVk(aDescription.textureFormat);
            vkImageViewCreateInfo.subresourceRange.aspectMask     = isDepthStencilImageOp(aDescription.textureFormat) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
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
                return { EEngineStatus::Error };
            }

            aGpuApiHandles.handle = vkImageView;

            return { EEngineStatus::Ok };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        static EEngineStatus SVulkanTextureViewResource::deinitialize(STextureViewDescription const &aDescription
                                                                      , Handles_t                     &aGpuApiHandles
                                                                      , IVkGlobalContext              *aVulkanEnvironment
                                                                      , VulkanResourceManager_t   *aResourceManager)
        {
            VkImageView vkImageView     = aGpuApiHandles.handle;
            VkDevice    vkLogicalDevice = aVulkanEnvironment->getLogicalDevice();

            // CLog::Debug(logTag(), "Destroying textureview w/ name {}", getCurrentDescriptor()->name);

            vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

            aGpuApiHandles.handle = VK_NULL_HANDLE;

            return { EEngineStatus::Ok };

        }
        //<-----------------------------------------------------------------------------

    }
}

#endif

#include <graphicsapi/resources/types/texture.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkantextureresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnTextureCreationTask(
                CTexture::CCreationRequest     const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CTexture::SDescriptor const &desc = aRequest.resourceDescriptor();

            VkImageType imageType = VkImageType::VK_IMAGE_TYPE_2D;
            if(desc.textureInfo.depth > 1) {
                imageType = VkImageType::VK_IMAGE_TYPE_3D;
            }
            else if(desc.textureInfo.height > 1) {
                imageType = VkImageType::VK_IMAGE_TYPE_2D;
            }
            else {
                imageType = VkImageType::VK_IMAGE_TYPE_1D;
            }

            VkImageUsageFlags imageUsage{};
            if(desc.gpuBinding.check(EBufferBinding::TextureInput))
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
            if(desc.gpuBinding.check(EBufferBinding::InputAttachement))
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            if(desc.gpuBinding.check(EBufferBinding::CopySource))
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            if(desc.gpuBinding.check(EBufferBinding::CopyTarget))
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            if(desc.gpuBinding.check(EBufferBinding::ColorAttachement))
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            if(desc.gpuBinding.check(EBufferBinding::DepthAttachement))
                imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                VkImageCreateInfo vkImageCreateInfo ={ };
                vkImageCreateInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                vkImageCreateInfo.imageType     = imageType;
                vkImageCreateInfo.extent.width  = desc.textureInfo.width;
                vkImageCreateInfo.extent.height = desc.textureInfo.height;
                vkImageCreateInfo.extent.depth  = desc.textureInfo.depth;
                vkImageCreateInfo.mipLevels     = desc.textureInfo.mipLevels;
                vkImageCreateInfo.arrayLayers   = desc.textureInfo.arraySize;
                vkImageCreateInfo.format        = VulkanDeviceCapsHelper::convertFormatToVk(desc.textureInfo.format);
                vkImageCreateInfo.usage         = imageUsage;
                vkImageCreateInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
                vkImageCreateInfo.tiling        = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
                vkImageCreateInfo.sharingMode   = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
                vkImageCreateInfo.samples       = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
                vkImageCreateInfo.flags         = 0;
                vkImageCreateInfo.pNext         = nullptr;

                VkImage vkImage = VK_NULL_HANDLE;

                VkDevice         const&vkLogicalDevice  = mVulkanEnvironment->getState().selectedLogicalDevice;
                VkPhysicalDevice const&vkPhysicalDevice = mVulkanEnvironment->getState().supportedPhysicalDevices.at(mVulkanEnvironment->getState().selectedPhysicalDevice).handle;

                VkResult result = vkCreateImage(vkLogicalDevice, &vkImageCreateInfo, nullptr, &vkImage);
                if(VkResult::VK_SUCCESS != result)
                {
                    throw CVulkanError("Failed to create texture.", result);
                }

                VkMemoryRequirements vkMemoryRequirements ={ };
                vkGetImageMemoryRequirements(vkLogicalDevice, vkImage, &vkMemoryRequirements);

                VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
                vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;
                vkMemoryAllocateInfo.memoryTypeIndex =
                        VulkanDeviceCapsHelper::determineMemoryType(
                            vkPhysicalDevice,
                            vkMemoryRequirements.memoryTypeBits,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


                VkDeviceMemory vkImageMemory = VK_NULL_HANDLE;

                result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &vkImageMemory);
                if(VkResult::VK_SUCCESS != result)
                {
                    throw CVulkanError("Failed to allocate image memory on GPU.", result);
                }

                result = vkBindImageMemory(vkLogicalDevice, vkImage, vkImageMemory, 0);
                if(VkResult::VK_SUCCESS != result)
                {
                    throw CVulkanError("Failed to bind image memory on GPU.", result);
                }

                SVulkanTextureResource *textureResource = new SVulkanTextureResource();
                textureResource->handle         = vkImage;
                textureResource->attachedMemory = vkImageMemory;

                SGFXAPIResourceHandleAssignment assignment ={ };

                assignment.publicResourceHandle   = desc.name; // Just abuse the pointer target address of the handle...
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanTextureResource>(textureResource, [] (SVulkanTextureResource const*p) { if(p) delete p; });

                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnTextureUpdateTask(
                CTexture::CUpdateRequest        const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnTextureDestructionTask(
                CTexture::CDestructionRequest   const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CStdSharedPtr_t<SVulkanTextureResource> texture =
                        std::static_pointer_cast<SVulkanTextureResource>(aAssignment.internalResourceHandle);
                if(!texture)
                    throw CVulkanError("Invalid internal data provided for texture destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);

                VkImage        vkImage         = texture->handle;
                VkDeviceMemory vkDeviceMemory  = texture->attachedMemory;
                VkDevice       vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);
                vkDestroyImage(vkLogicalDevice, vkImage, nullptr);

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnTextureQueryTask(
                CTexture::CQuery                const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}

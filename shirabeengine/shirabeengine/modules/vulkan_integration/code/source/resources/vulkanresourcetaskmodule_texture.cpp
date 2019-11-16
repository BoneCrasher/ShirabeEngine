#include <resources/resourcetypes.h>
#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/resources/vulkanresourcetaskbackend.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureCreationTask(
                CTexture::CCreationRequest     const &aRequest,
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            CTexture::SDescriptor const &desc = aRequest.resourceDescriptor();

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

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
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

                VkDevice         const &vkLogicalDevice  = mVulkanEnvironment->getState().selectedLogicalDevice;
                VkPhysicalDevice const &vkPhysicalDevice = mVulkanEnvironment->getState().supportedPhysicalDevices.at(mVulkanEnvironment->getState().selectedPhysicalDevice).handle;

                VkResult result = vkCreateImage(vkLogicalDevice, &vkImageCreateInfo, nullptr, &vkImage);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to create texture. Vulkan result: {}", result));
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
                    CLog::Error(logTag(), CString::format("Failed to allocate image memory on GPU. Vulkan error: {}", result));
                    return { EEngineStatus::Error };
                }

                result = vkBindImageMemory(vkLogicalDevice, vkImage, vkImageMemory, 0);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to bind image memory on GPU. Vulkan error: {}", result));
                    return { EEngineStatus::Error };
                }

                SVulkanTextureResource *textureResource = new SVulkanTextureResource();
                textureResource->handle         = vkImage;
                textureResource->attachedMemory = vkImageMemory;

                auto const deleter = [] (SVulkanTextureResource const*p)
                {
                    if(p)
                    {
                        delete p;
                    }
                };

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = desc.name;
                assignment.internalResourceHandle = Shared<SVulkanTextureResource>(textureResource, deleter);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureUpdateTask(
                CTexture::CUpdateRequest        const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aDependencies);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureDestructionTask(
                CTexture::CDestructionRequest   const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                Shared<SVulkanTextureResource> texture = std::static_pointer_cast<SVulkanTextureResource>(aAssignment.internalResourceHandle);
                if(nullptr == texture)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for texture destruction. Vulkan error: {}", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkImage        vkImage         = texture->handle;
                VkDeviceMemory vkDeviceMemory  = texture->attachedMemory;
                VkDevice       vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);
                vkDestroyImage(vkLogicalDevice, vkImage, nullptr);

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureQueryTask(
                CTexture::CQuery                const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return { status };
        }
        //<-----------------------------------------------------------------------------
    }
}

#include <graphicsapi/resources/types/textureview.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan_integration/vulkandevicecapabilities.h"
#include "vulkan_integration/resources/vulkanresourcetaskbackend.h"
#include "vulkan_integration/resources/types/vulkantextureresource.h"
#include "vulkan_integration/resources/types/vulkantextureviewresource.h"

#include <vulkan/vulkan.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureViewCreationTask(
                CTextureView::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CTextureView::SDescriptor const &desc = aRequest.resourceDescriptor();

            uint8_t dimensionCount = 1;
            dimensionCount += (desc.subjacentTexture.height > 1) ? 1 : 0;
            dimensionCount += (desc.subjacentTexture.depth  > 1) ? 1 : 0;

            VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

            switch(dimensionCount)
            {
            case 1:
                if(1 < desc.subjacentTexture.arraySize)
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                }
                else
                {
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                }
                break;
            case 2:
                if(1 < desc.subjacentTexture.arraySize)
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
            }

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                PublicResourceId_t const &underlyingTextureHandle = aRequest.underlyingTextureHandle();
                Shared<void>     privateDependencyHandle = aDepencies.at(underlyingTextureHandle);
                if(not privateDependencyHandle)
                {
                    CLog::Error(logTag(), "Failed to create TextureView due to missing dependency.");
                    return { EEngineStatus::DXDevice_CreateRTV_Failed };
                }

                Shared<SVulkanTextureResource> texture = std::static_pointer_cast<SVulkanTextureResource>(privateDependencyHandle);
                if(not texture)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for texture destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkImage        const vkImage        = texture->handle;
                // VkDeviceMemory const vkDeviceMemory = texture->attachedMemory; // TODO: Required?

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

                VkResult result = vkCreateImageView(mVulkanEnvironment->getState().selectedLogicalDevice, &vkImageViewCreateInfo, nullptr, &vkImageView);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to create render target view. Vulkan error: %0", result));
                    return { EEngineStatus::Error };
                }

                SVulkanTextureViewResource *textureViewResource = new SVulkanTextureViewResource();
                textureViewResource->handle = vkImageView;

                SGFXAPIResourceHandleAssignment assignment ={ };

                auto const textureViewDeleter = [] (SVulkanTextureViewResource const *aPointer)
                {
                    if(aPointer)
                    {
                        delete aPointer;
                    }
                };

                assignment.publicResourceHandle   = desc.name; // Just abuse the pointer target address of the handle...
                assignment.internalResourceHandle = Shared<SVulkanTextureViewResource>(textureViewResource, textureViewDeleter);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureViewUpdateTask(
                CTextureView::CUpdateRequest    const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDependencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aAssignment);
            SHIRABE_UNUSED(aDependencies);
            SHIRABE_UNUSED(aOutTask);

            EEngineStatus status = EEngineStatus::Ok;

            return { status };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureViewDestructionTask(
                CTextureView::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment   const &aAssignment,
                ResolvedDependencyCollection_t    const &aDependencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                Shared<SVulkanTextureViewResource> textureView = std::static_pointer_cast<SVulkanTextureViewResource>(aAssignment.internalResourceHandle);
                if(nullptr == textureView)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for texture view destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkImageView vkImageView     = textureView->handle;
                VkDevice    vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

                SGFXAPIResourceHandleAssignment assignment = aAssignment;
                assignment.internalResourceHandle = nullptr;

                return { EEngineStatus::Ok, assignment };
            };

            return { status };
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnTextureViewQueryTask(
                CTextureView::CQuery            const &aRequest,
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

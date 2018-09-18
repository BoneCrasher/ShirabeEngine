#include <graphicsapi/resources/types/textureview.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkantextureresource.h"
#include "vulkan/resources/types/vulkantextureviewresource.h"

#include <vulkan/vulkan.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnTextureViewCreationTask(
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
                if(desc.subjacentTexture.arraySize > 1)
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                else
                    imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case 2:
                if(desc.subjacentTexture.arraySize > 1)
                    imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                else
                    imageViewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case 3:
                imageViewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            }

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                PublicResourceId_t const &underlyingTextureHandle = aRequest.underlyingTextureHandle();
                CStdSharedPtr_t<void> privateDependencyHandle = aDepencies.at(underlyingTextureHandle);
                if(!privateDependencyHandle)
                {
                    HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create TextureView due to missing dependency.");
                }

                CStdSharedPtr_t<SVulkanTextureResource> texture = std::static_pointer_cast<SVulkanTextureResource>(privateDependencyHandle);
                if(!texture)
                {
                    throw CVulkanError("Invalid internal data provided for texture destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                }

                VkImage        const vkImage        = texture->handle;
                VkDeviceMemory const vkDeviceMemory = texture->attachedMemory;

                VkImageViewCreateInfo vkImageViewCreateInfo ={ };
                vkImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                vkImageViewCreateInfo.format                          = CVulkanDeviceCapsHelper::convertFormatToVk(desc.textureFormat);
                vkImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT; // TODO: Care about the specific aspect bits here in more detail.
                vkImageViewCreateInfo.subresourceRange.baseArrayLayer = desc.arraySlices.offset;
                vkImageViewCreateInfo.subresourceRange.layerCount     = desc.arraySlices.length;
                vkImageViewCreateInfo.subresourceRange.baseMipLevel   = desc.mipMapSlices.offset;
                vkImageViewCreateInfo.subresourceRange.levelCount     = desc.mipMapSlices.length;
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
                    throw CVulkanError("Failed to create render target view.", result);
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
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanTextureViewResource>(textureViewResource, textureViewDeleter);

                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnTextureViewUpdateTask(
                CTextureView::CUpdateRequest    const &aRequest,
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
        EEngineStatus CVulkanResourceTaskBackend::fnTextureViewDestructionTask(
                CTextureView::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment   const &aAssignment,
                ResolvedDependencyCollection_t    const &aDepencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CStdSharedPtr_t<SVulkanTextureViewResource> textureView = std::static_pointer_cast<SVulkanTextureViewResource>(aAssignment.internalResourceHandle);
                if(!textureView)
                {
                    throw CVulkanError("Invalid internal data provided for texture view destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                }

                VkImageView vkImageView     = textureView->handle;
                VkDevice    vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

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
        EEngineStatus
        CVulkanResourceTaskBackend::
        fnTextureViewQueryTask(
                CTextureView::CQuery            const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}

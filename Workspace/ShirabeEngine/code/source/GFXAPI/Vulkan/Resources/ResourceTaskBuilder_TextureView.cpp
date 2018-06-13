#include "GFXAPI/Vulkan/DeviceCapabilities.h"
#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"
#include "GFXAPI/Vulkan/Resources/Types/VulkanTextureResource.h"
#include "GFXAPI/Vulkan/Resources/Types/VulkanTextureViewResource.h"

#include <vulkan/vulkan.h>

namespace Engine {
  namespace Vulkan {
    using namespace Engine::Resources;

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureViewCreationTask(
        TextureView::CreationRequest   const&request,
        ResolvedDependencyCollection   const&resolvedDependencies,
        ResourceTaskFn_t                    &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      TextureView::Descriptor const&desc = request.resourceDescriptor();

      uint8_t dimensionCount = 1;
      dimensionCount += (desc.subjacentTexture.height > 1) ? 1 : 0;
      dimensionCount += (desc.subjacentTexture.depth  > 1) ? 1 : 0;

      VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

      switch(dimensionCount) {
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

      outTask = [=] () -> GFXAPIResourceHandleAssignment
      {
        Ptr<void> privateDependencyHandle = resolvedDependencies.at(request.underlyingTextureHandle());
        if(!privateDependencyHandle) {
          HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create RTV due to missing dependency.");
        }
        
        Ptr<VulkanTextureResource> texture = std::static_pointer_cast<VulkanTextureResource>(privateDependencyHandle);
        if(!texture)
          throw VulkanError("Invalid internal data provided for texture destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);

        VkImage        vkImage         = texture->handle;
        VkDeviceMemory vkDeviceMemory  = texture->attachedMemory;
        
        VkImageViewCreateInfo vkImageViewCreateInfo{ };
        vkImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vkImageViewCreateInfo.format                          = VulkanDeviceCapsHelper::convertFormatToVk(desc.textureFormat);
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

        VkResult result = vkCreateImageView(m_vulkanEnvironment->getState().selectedLogicalDevice, &vkImageViewCreateInfo, nullptr, &vkImageView);
        if(VkResult::VK_SUCCESS != result)
          throw VulkanError("Failed to create render target view.", result);

        VulkanTextureViewResource *textureViewResource = new VulkanTextureViewResource();
        textureViewResource->handle = vkImageView;

        GFXAPIResourceHandleAssignment assignment ={ };

        assignment.publicHandle   = desc.name; // Just abuse the pointer target address of the handle...
        assignment.internalHandle = Ptr<VulkanTextureViewResource>(textureViewResource, [] (VulkanTextureViewResource const*p) { if(p) delete p; });

        return assignment;
      };

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureViewUpdateTask(
        TextureView::UpdateRequest     const&request,
        GFXAPIResourceHandleAssignment const&assignment,
        ResolvedDependencyCollection   const&resolvedDependencies,
        ResourceTaskFn_t                    &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureViewDestructionTask(
        TextureView::DestructionRequest const&request,
        GFXAPIResourceHandleAssignment  const&inAssignment,
        ResolvedDependencyCollection    const&resolvedDependencies,
        ResourceTaskFn_t                     &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;
      
      outTask = [=] () -> GFXAPIResourceHandleAssignment
      {
        Ptr<VulkanTextureViewResource> textureView = std::static_pointer_cast<VulkanTextureViewResource>(inAssignment.internalHandle);
        if(!textureView)
          throw VulkanError("Invalid internal data provided for texture view destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);

        VkImageView vkImageView     = textureView->handle;
        VkDevice    vkLogicalDevice = m_vulkanEnvironment->getState().selectedLogicalDevice;

        vkDestroyImageView(vkLogicalDevice, vkImageView, nullptr);

        GFXAPIResourceHandleAssignment assignment = inAssignment;
        assignment.internalHandle = nullptr;

        return assignment;
      };

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureViewQueryTask(
        TextureView::Query             const&request,
        GFXAPIResourceHandleAssignment const&assignment,
        ResourceTaskFn_t                    &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

  }
}
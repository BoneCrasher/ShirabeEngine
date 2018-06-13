#include "GFXAPI/Vulkan/DeviceCapabilities.h"
#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"

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
        uint64_t privateDependencyHandle = resolvedDependencies.at(request.underlyingTextureHandle());
        if(!privateDependencyHandle) {
          HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create RTV due to missing dependency.");
        }

        VkImage underlyingTexture = static_cast<VkImage>(privateDependencyHandle);
        
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
        vkImageViewCreateInfo.image                           = underlyingTexture;
        vkImageViewCreateInfo.viewType                        = imageViewType;
        vkImageViewCreateInfo.flags                           = 0; // Reserved
        vkImageViewCreateInfo.pNext                           = nullptr;

        VkImageView vkImageView = VK_NULL_HANDLE;

        VkResult result = vkCreateImageView(m_vulkanEnvironment->getState().selectedLogicalDevice, &vkImageViewCreateInfo, nullptr, &vkImageView);
        if(VkResult::VK_SUCCESS != result)
          throw VulkanError("Failed to create render target view.", result);

        GFXAPIResourceHandleAssignment assignment ={ };

        assignment.publicHandle   = desc.name; // Just abuse the pointer target address of the handle...
        assignment.internalHandle = static_cast<uint64_t>(vkImageView);

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
        VkImageView imageView = static_cast<VkImageView>(inAssignment.internalHandle);

        vkDestroyImageView(m_vulkanEnvironment->getState().selectedLogicalDevice, imageView, nullptr);

        GFXAPIResourceHandleAssignment assignment ={ };

        assignment.publicHandle   = inAssignment.publicHandle; // Just abuse the pointer target address of the handle...
        assignment.internalHandle = 0;

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
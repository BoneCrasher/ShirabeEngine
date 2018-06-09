#include "GFXAPI/Vulkan/DeviceCapabilities.h"
#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"

#include <vulkan/vulkan.h>

namespace Engine {
  namespace Vulkan {
    using namespace Engine::Resources;

    //EEngineStatus
    //  VulkanResourceTaskBackend::
    //  creationTask(
    //    RenderTargetView::CreationRequest   const&request,
    //    ResolvedDependencyCollection        const&resolvedDependencies,
    //    ResourceTaskFn_t                         &outTask)
    //{
    //  EEngineStatus status = EEngineStatus::Ok;

    //  RenderTargetView::Descriptor const&desc = request.resourceDescriptor();

    //  uint8_t dimensionCount = 1;
    //  dimensionCount += (desc.subjacentTexture.height > 1) ? 1 : 0;
    //  dimensionCount += (desc.subjacentTexture.depth  > 1) ? 1 : 0;

    //  VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;

    //  switch(dimensionCount) {
    //  case 1:
    //    if(desc.subjacentTexture.arraySize > 1)
    //      imageViewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    //    else
    //      imageViewType = VK_IMAGE_VIEW_TYPE_1D;
    //    break;
    //  case 2:
    //    if(desc.subjacentTexture.arraySize > 1)
    //      imageViewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    //    else
    //      imageViewType = VK_IMAGE_VIEW_TYPE_2D;
    //    break;
    //  case 3:
    //    imageViewType = VK_IMAGE_VIEW_TYPE_3D;
    //    break;
    //  }

    //  outTask = [&, this] () -> GFXAPIResourceHandleAssignment
    //  {
    //    Ptr<void> privateDependencyHandle = resolvedDependencies.at(request.underlyingTextureHandle());
    //    if(!privateDependencyHandle) {
    //      HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create RTV due to missing dependency.");
    //    }

    //    Ptr<VkImage> underlyingTexture = std::static_pointer_cast<VkImage>(privateDependencyHandle);

    //    GFXAPIResourceHandleAssignment assignment ={};

    //    VkImageViewCreateInfo vkImageViewCreateInfo{ };
    //    vkImageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //    vkImageViewCreateInfo.format                          = VulkanDeviceCapsHelper::convertFormatToVk(desc.textureFormat);
    //    vkImageViewCreateInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    //    vkImageViewCreateInfo.subresourceRange.baseArrayLayer = desc.arraySlices.offset;
    //    vkImageViewCreateInfo.subresourceRange.layerCount     = desc.arraySlices.length;
    //    vkImageViewCreateInfo.subresourceRange.baseMipLevel   = desc.mipMapSlices.offset;
    //    vkImageViewCreateInfo.subresourceRange.levelCount     = desc.mipMapSlices.length;
    //    vkImageViewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    //    vkImageViewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    //    vkImageViewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    //    vkImageViewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    //    vkImageViewCreateInfo.image                           = *underlyingTexture;
    //    vkImageViewCreateInfo.viewType                        = imageViewType;
    //    vkImageViewCreateInfo.flags                           = 0; // Reserved
    //    vkImageViewCreateInfo.pNext                           = nullptr;

    //    VkImageView vkImageView = VK_NULL_HANDLE;

    //    VkResult result = vkCreateImageView(m_vulkanEnvironment->getState().selectedLogicalDevice, &vkImageViewCreateInfo, nullptr, &vkImageView);
    //    if(VkResult::VK_SUCCESS != result)
    //      throw VulkanError("Failed to create render target view.", result);

    //    assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(vkImageView); // Just abuse the pointer target address of the handle...
    //    assignment.internalHandle = Ptr<void>((void*)vkImageView);

    //    return assignment;
    //  };

    //  return status;
    //}

    //EEngineStatus
    //  VulkanResourceTaskBackend::
    //  updateTask(
    //    RenderTargetView::UpdateRequest const&request,
    //    ResolvedDependencyCollection    const&resolvedDependencies,
    //    ResourceTaskFn_t                     &outTask)
    //{
    //  EEngineStatus status = EEngineStatus::Ok;

    //  return status;
    //}

    //EEngineStatus
    //  VulkanResourceTaskBackend::
    //  destructionTask(
    //    RenderTargetView::DestructionRequest const&request,
    //    ResolvedDependencyCollection         const&resolvedDependencies,
    //    ResourceTaskFn_t                          &outTask)
    //{
    //  EEngineStatus status = EEngineStatus::Ok;

    //  return status;
    //}

    //EEngineStatus
    //  VulkanResourceTaskBackend::
    //  queryTask(
    //    RenderTargetView::Query const&request,
    //    ResourceTaskFn_t             &outTask)
    //{
    //  EEngineStatus status = EEngineStatus::Ok;

    //  return status;
    //}

  }
}
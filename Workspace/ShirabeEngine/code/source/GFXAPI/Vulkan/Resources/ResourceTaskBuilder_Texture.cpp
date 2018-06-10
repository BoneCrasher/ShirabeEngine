#include "GFXAPI/Vulkan/DeviceCapabilities.h"
#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"

namespace Engine {
  namespace Vulkan {
    using namespace Engine::Resources;
    using namespace Engine::DX::_11;

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureCreationTask(
        Texture::CreationRequest     const& request,
        ResolvedDependencyCollection const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      Texture::Descriptor const&desc = request.resourceDescriptor();
      
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
      if(desc.gpuBinding.check(BufferBinding::ShaderInput)) 
        imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;      
      if(desc.gpuBinding.check(BufferBinding::InputAttachement)) 
        imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
      if(desc.gpuBinding.check(BufferBinding::CopySource))
        imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
      if(desc.gpuBinding.check(BufferBinding::CopyTarget)) 
        imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;      
      if(desc.gpuBinding.check(BufferBinding::ColorAttachement))
        imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      if(desc.gpuBinding.check(BufferBinding::DepthAttachement))
        imageUsage |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

      outTask = [&, this] () -> GFXAPIResourceHandleAssignment
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
        vkImageCreateInfo.flags         = 0;
        vkImageCreateInfo.pNext         = nullptr;

        VkImage vkImage = VK_NULL_HANDLE;

        VkResult result = vkCreateImage(m_vulkanEnvironment->getState().selectedLogicalDevice, &vkImageCreateInfo, nullptr, &vkImage);
        if(VkResult::VK_SUCCESS != result)
          throw VulkanError("Failed to create texture.", result);
        
        GFXAPIResourceHandleAssignment assignment ={ };

        assignment.publicHandle   = reinterpret_cast<GFXAPIResourceHandle_t>(vkImage); // Just abuse the pointer target address of the handle...
        assignment.internalHandle = Ptr<void>((void*)vkImage);

        return assignment;
      };

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureUpdateTask(
        Texture::UpdateRequest         const&request,
        GFXAPIResourceHandleAssignment const&assignment,
        ResolvedDependencyCollection   const&resolvedDependencies,
        ResourceTaskFn_t                  &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureDestructionTask(
        Texture::DestructionRequest    const&request,
        GFXAPIResourceHandleAssignment const&assignment,
        ResolvedDependencyCollection   const&resolvedDependencies,
        ResourceTaskFn_t                    &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      VkImage image = *std::static_pointer_cast<VkImage>(assignment.internalHandle);

      vkDestroyImage(m_vulkanEnvironment->getState().selectedLogicalDevice, image, nullptr);

      return status;
    }

    EEngineStatus
      VulkanResourceTaskBackend::
      fnTextureQueryTask(
        Texture::Query                 const&request,
        GFXAPIResourceHandleAssignment const&assignment,
        ResourceTaskFn_t                    &outTask)
    {
      EEngineStatus status = EEngineStatus::Ok;

      return status;
    }

  }
}
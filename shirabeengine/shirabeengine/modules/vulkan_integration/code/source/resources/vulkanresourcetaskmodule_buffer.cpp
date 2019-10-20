#include <graphicsapi/resources/types/buffer.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanbufferresource.h"

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferCreationTask(
                CBuffer::CCreationRequest      const &aRequest,
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            CBuffer::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                VkDevice         const &vkLogicalDevice  = mVulkanEnvironment->getState().selectedLogicalDevice;
                VkPhysicalDevice const &vkPhysicalDevice = mVulkanEnvironment->getState().supportedPhysicalDevices.at(mVulkanEnvironment->getState().selectedPhysicalDevice).handle;

                VkBufferCreateInfo createInfo = desc.createInfo;
                createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                // createInfo.queueFamilyIndexCount = ...;
                // createInfo.pQueueFamilyIndices   = ...;

                VkBuffer buffer = VK_NULL_HANDLE;

                VkResult result = vkCreateBuffer(vkLogicalDevice, &createInfo, nullptr, &buffer);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to create buffer. Vulkan result: %0", result));
                    return { EEngineStatus::Error };
                }

                VkMemoryRequirements vkMemoryRequirements ={ };
                vkGetBufferMemoryRequirements(vkLogicalDevice, buffer, &vkMemoryRequirements);

                VkMemoryAllocateInfo vkMemoryAllocateInfo ={ };
                vkMemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                vkMemoryAllocateInfo.allocationSize  = vkMemoryRequirements.size;

                CEngineResult<uint32_t> memoryTypeFetch =
                        CVulkanDeviceCapsHelper::determineMemoryType(
                            vkPhysicalDevice,
                            vkMemoryRequirements.memoryTypeBits,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                if(not memoryTypeFetch.successful())
                {
                    CLog::Error(logTag(), "Could not determine memory type index.");
                    return { EEngineStatus::Error };
                }

                vkMemoryAllocateInfo.memoryTypeIndex = memoryTypeFetch.data();

                VkDeviceMemory memory = VK_NULL_HANDLE;

                result = vkAllocateMemory(vkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &memory);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to allocate buffer memory on GPU. Vulkan error: %0", result));
                    return { EEngineStatus::Error };
                }

                result = vkBindBufferMemory(vkLogicalDevice, buffer, memory, 0);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to bind buffer memory on GPU. Vulkan error: %0", result));
                    return { EEngineStatus::Error };
                }

                SVulkanBufferResource *textureResource = new SVulkanBufferResource();
                textureResource->handle         = buffer;
                textureResource->attachedMemory = memory;

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = desc.name;
                assignment.internalResourceHandle = Shared<SVulkanBufferResource>(textureResource);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferUpdateTask(
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
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferDestructionTask(
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
                Shared<SVulkanBufferResource> texture = std::static_pointer_cast<SVulkanBufferResource>(aAssignment.internalResourceHandle);
                if(nullptr == texture)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for buffer destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkBuffer       vkBuffer         = texture->handle;
                VkDeviceMemory vkDeviceMemory  = texture->attachedMemory;
                VkDevice       vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkFreeMemory(vkLogicalDevice, vkDeviceMemory, nullptr);
                vkDestroyBuffer(vkLogicalDevice, vkBuffer, nullptr);

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
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferQueryTask(
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

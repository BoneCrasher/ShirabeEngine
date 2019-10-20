#include <graphicsapi/resources/types/bufferview.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanbufferresource.h"
#include "vulkan/resources/types/vulkanbufferviewresource.h"

#include <vulkan/vulkan.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferViewCreationTask(
                CBufferView::CCreationRequest  const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CBufferView::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                PublicResourceId_t const &subjacentResourceHandle = aRequest.underlyingBufferHandle();
                Shared<void>     privateDependencyHandle = aDepencies.at(subjacentResourceHandle);
                if(not privateDependencyHandle)
                {
                    CLog::Error(logTag(), "Failed to create BufferView due to missing dependency.");
                    return { EEngineStatus::DXDevice_CreateRTV_Failed };
                }

                Shared<SVulkanBufferResource> buffer = std::static_pointer_cast<SVulkanBufferResource>(privateDependencyHandle);
                if(not buffer)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for buffer creatopm. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkBuffer const vkBuffer = buffer->handle;
                // VkDeviceMemory const vkDeviceMemory = texture->attachedMemory; // TODO: Required?

                VkBufferView vkBufferView = VK_NULL_HANDLE;

                VkResult result = vkCreateBufferView(mVulkanEnvironment->getState().selectedLogicalDevice, &(desc.createInfo), nullptr, &vkBufferView);
                if(VkResult::VK_SUCCESS != result)
                {
                    CLog::Error(logTag(), CString::format("Failed to create buffer view. Vulkan error: %0", result));
                    return { EEngineStatus::Error };
                }

                SVulkanBufferViewResource *resource = new SVulkanBufferViewResource();
                resource->handle = vkBufferView;

                SGFXAPIResourceHandleAssignment assignment ={ };

                assignment.publicResourceHandle   = desc.name; // Just abuse the pointer target address of the handle...
                assignment.internalResourceHandle = Shared<SVulkanBufferViewResource>(resource);

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferViewUpdateTask(
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
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferViewDestructionTask(
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
                Shared<SVulkanBufferViewResource> bufferView = std::static_pointer_cast<SVulkanBufferViewResource>(aAssignment.internalResourceHandle);
                if(nullptr == bufferView)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for buffer view destruction. Vulkan error: %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkBufferView vkBufferView    = bufferView->handle;
                VkDevice     vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyBufferView(vkLogicalDevice, vkBufferView, nullptr);

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
        CEngineResult<> CVulkanResourceTaskBackend::fnBufferViewQueryTask(
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

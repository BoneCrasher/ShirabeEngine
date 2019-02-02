#include <graphicsapi/resources/types/framebuffer.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkandevicecapabilities.h"
#include "vulkan/resources/vulkanresourcetaskbackend.h"
#include "vulkan/resources/types/vulkanrenderpassresource.h"
#include "vulkan/resources/types/vulkantextureviewresource.h"
#include "vulkan/resources/types/vulkanframebufferresource.h"

#include <vulkan/vulkan.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnFrameBufferCreationTask(
                CFrameBuffer::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CFrameBuffer::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                PublicResourceId_t     const &renderPassHandle   = aRequest.renderPassHandle();
                PublicResourceIdList_t const &textureViewHandles = aRequest.textureViewHandles();

                CStdSharedPtr_t<void> renderPassPrivateHandle = aDepencies.at(renderPassHandle);
                if(!renderPassPrivateHandle)
                {
                    HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create frame buffer due to missing dependency.");
                }

                CStdSharedPtr_t<SVulkanRenderPassResource> renderPass = std::static_pointer_cast<SVulkanRenderPassResource>(renderPassPrivateHandle);
                if(!renderPass)
                {
                    throw CVulkanError("Invalid internal data provided for frame buffer creation.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                }

                std::vector<VkImageView> textureViews {};

                for(PublicResourceId_t const &id : textureViewHandles)
                {
                    CStdSharedPtr_t<void> textureViewPrivateHandle = aDepencies.at(id);
                    if(!textureViewPrivateHandle)
                    {
                        HandleEngineStatusError(EEngineStatus::DXDevice_CreateRTV_Failed, "Failed to create frame buffer due to missing dependency.");
                    }

                    CStdSharedPtr_t<SVulkanTextureViewResource> textureView = std::static_pointer_cast<SVulkanTextureViewResource>(textureViewPrivateHandle);
                    if(!renderPass)
                    {
                        throw CVulkanError("Invalid internal data provided for frame buffer creation.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                    }

                    textureViews.push_back(textureView->handle);
                }

                VkFramebufferCreateInfo vkFrameBufferCreateInfo {};
                vkFrameBufferCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                vkFrameBufferCreateInfo.pNext           = nullptr;
                vkFrameBufferCreateInfo.pAttachments    = textureViews.data();
                vkFrameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(textureViews.size());
                vkFrameBufferCreateInfo.renderPass      = renderPass->handle;
                vkFrameBufferCreateInfo.width           = desc.width;
                vkFrameBufferCreateInfo.height          = desc.height;
                vkFrameBufferCreateInfo.layers          = desc.layers;
                vkFrameBufferCreateInfo.flags           = 0;

                VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;

                VkResult result = vkCreateFramebuffer(mVulkanEnvironment->getState().selectedLogicalDevice, &vkFrameBufferCreateInfo, nullptr, &vkFrameBuffer);
                if(VkResult::VK_SUCCESS != result)
                {
                    throw CVulkanError("Failed to create frame buffer instance.", result);
                }

                SVulkanFrameBufferResource *frameBufferResource = new SVulkanFrameBufferResource();
                frameBufferResource->handle = vkFrameBuffer;

                SGFXAPIResourceHandleAssignment assignment ={ };

                auto const frameBufferDeleter = [] (SVulkanFrameBufferResource const *aPointer)
                {
                    if(aPointer)
                    {
                        delete aPointer;
                    }
                };

                assignment.publicResourceHandle   = desc.name; // Just abuse the pointer target address of the handle...
                assignment.internalResourceHandle = CStdSharedPtr_t<SVulkanFrameBufferResource>(frameBufferResource, frameBufferDeleter);

                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanResourceTaskBackend::fnFrameBufferUpdateTask(
                CFrameBuffer::CUpdateRequest    const &aRequest,
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
        EEngineStatus CVulkanResourceTaskBackend::fnFrameBufferDestructionTask(
                CFrameBuffer::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment   const &aAssignment,
                ResolvedDependencyCollection_t    const &aDepencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CStdSharedPtr_t<SVulkanFrameBufferResource> FrameBuffer = std::static_pointer_cast<SVulkanFrameBufferResource>(aAssignment.internalResourceHandle);
                if(!FrameBuffer)
                {
                    throw CVulkanError("Invalid internal data provided for frame buffer destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                }

                VkFramebuffer vkFrameBuffer   = FrameBuffer->handle;
                VkDevice      vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyFramebuffer(vkLogicalDevice, vkFrameBuffer, nullptr);

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
        EEngineStatus CVulkanResourceTaskBackend::fnFrameBufferQueryTask(
                CFrameBuffer::CQuery            const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}

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
                ResolvedDependencyCollection_t const &aDependencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            CFrameBuffer::SDescriptor const &desc = aRequest.resourceDescriptor();

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                PublicResourceId_t     const &renderPassHandle   = aRequest.renderPassHandle();
                PublicResourceIdList_t const &textureViewHandles = aRequest.textureViewHandles();

                CStdSharedPtr_t<void> renderPassPrivateHandle = aDependencies.at(renderPassHandle);
                if(not renderPassPrivateHandle)
                {
                    CLog::Error(logTag(), "Failed to create frame buffer due to missing dependency.");
                    return { EEngineStatus::DXDevice_CreateRTV_Failed };
                }

                CStdSharedPtr_t<SVulkanRenderPassResource> renderPass = std::static_pointer_cast<SVulkanRenderPassResource>(renderPassPrivateHandle);
                if(not renderPass)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for frame buffer creation. %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                std::vector<VkImageView> textureViews {};

                for(PublicResourceId_t const &id : textureViewHandles)
                {
                    CStdSharedPtr_t<void> textureViewPrivateHandle = aDependencies.at(id);
                    if(not textureViewPrivateHandle)
                    {
                        CLog::Error(logTag(), "Failed to create frame buffer due to missing dependency.");
                        return { EEngineStatus::DXDevice_CreateRTV_Failed };
                    }

                    CStdSharedPtr_t<SVulkanTextureViewResource> textureView = std::static_pointer_cast<SVulkanTextureViewResource>(textureViewPrivateHandle);
                    if(not textureView)
                    {
                        CLog::Error(logTag(), "Invalid internal data provided for frame buffer creation. %0", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE);
                        return { EEngineStatus::Error };
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
                    CLog::Error(logTag(), CString::format("Failed to create frame buffer instance. Vulkan result: %0", result));
                    return { EEngineStatus::Error };
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

                return { EEngineStatus::Ok, assignment };
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanResourceTaskBackend::fnFrameBufferUpdateTask(
                CFrameBuffer::CUpdateRequest    const &aRequest,
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
        CEngineResult<> CVulkanResourceTaskBackend::fnFrameBufferDestructionTask(
                CFrameBuffer::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment   const &aAssignment,
                ResolvedDependencyCollection_t    const &aDependencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            SHIRABE_UNUSED(aRequest);
            SHIRABE_UNUSED(aDependencies);

            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> CEngineResult<SGFXAPIResourceHandleAssignment>
            {
                CStdSharedPtr_t<SVulkanFrameBufferResource> frameBuffer = std::static_pointer_cast<SVulkanFrameBufferResource>(aAssignment.internalResourceHandle);
                if(not frameBuffer)
                {
                    CLog::Error(logTag(), CString::format("Invalid internal data provided for frame buffer destruction.", VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE));
                    return { EEngineStatus::Error };
                }

                VkFramebuffer vkFrameBuffer   = frameBuffer->handle;
                VkDevice      vkLogicalDevice = mVulkanEnvironment->getState().selectedLogicalDevice;

                vkDestroyFramebuffer(vkLogicalDevice, vkFrameBuffer, nullptr);

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
        CEngineResult<> CVulkanResourceTaskBackend::fnFrameBufferQueryTask(
                CFrameBuffer::CQuery            const &aRequest,
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

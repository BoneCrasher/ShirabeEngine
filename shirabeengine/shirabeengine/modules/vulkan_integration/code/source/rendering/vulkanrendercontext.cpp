#include "vulkan/rendering/vulkanrendercontext.h"

#include <thread>
#include <core/string.h>

namespace engine
{
    namespace vulkan
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------        
        bool CVulkanRenderContext::initialize(
                CStdSharedPtr_t<CVulkanEnvironment>             const &aVulkanEnvironment,
                CStdSharedPtr_t<gfxapi::CGFXAPIResourceBackend> const &aGraphicsAPIResourceBackend)
        {
            assert(nullptr != aVulkanEnvironment);
            assert(nullptr != aGraphicsAPIResourceBackend);

            mVulkanEnvironment          = aVulkanEnvironment;
            mGraphicsAPIResourceBackend = aGraphicsAPIResourceBackend;

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CVulkanRenderContext::deinitialize()
        {
            mGraphicsAPIResourceBackend = nullptr;

            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindSwapChain(PublicResourceId_t const &aSwapChainResourceId)
        {
            CVulkanEnvironment::SVulkanState     &vkState     = mVulkanEnvironment->getState();
            CVulkanEnvironment::SVulkanSwapChain &vkSwapChain = vkState.swapChain;

            uint32_t nextImageIndex = 0;

            VkDevice       device    = vkState.selectedLogicalDevice;
            VkSwapchainKHR swapChain = vkState.swapChain.handle;
            VkSemaphore    semaphore = vkState.swapChain.imageAvailableSemaphore;
            uint64_t const timeout   =  std::numeric_limits<uint64_t>::max();

            VkResult result = VK_SUCCESS;

            do
            {
                result =
                    vkAcquireNextImageKHR(
                        device,
                        swapChain,
                        timeout,
                        semaphore,
                        VK_NULL_HANDLE,
                        &nextImageIndex);

                if(VkResult::VK_ERROR_OUT_OF_DATE_KHR == result)
                {
                    mVulkanEnvironment->recreateSwapChain();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

            }
            while(VkResult::VK_SUCCESS != result);

            if(VkResult::VK_SUCCESS != result)
            {
                CLog::Error(logTag(), CString::format("AquireNextImageKHR failed with VkResult: %0", result));
                throw CVulkanError("Failed to execute 'vkAcquireNextImageKHR'.", result);
            }

            vkState.swapChain.currentSwapChainImageIndex = nextImageIndex;

            VkImage               &image    = vkSwapChain.swapChainImages.at(static_cast<uint64_t>(nextImageIndex));
            CStdSharedPtr_t<void>  resource = CStdSharedPtr_t<void>(static_cast<void*>(&image), [] (void*) {});

            EEngineStatus const status =
                    mGraphicsAPIResourceBackend->registerResource(
                        aSwapChainResourceId,
                        resource,
                        gfxapi::CGFXAPIResourceBackend::EImportStorageMode::Overwrite);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::present()
        {
            CVulkanEnvironment::SVulkanState &vkState = mVulkanEnvironment->getState();

            VkQueue presentQueue = mVulkanEnvironment->getPresentQueue();

            VkSwapchainKHR swapChains[]       = { vkState.swapChain.handle                   };
            VkSemaphore    waitSemaphores[]   = { vkState.swapChain.imageAvailableSemaphore  };
            VkSemaphore    signalSemaphores[] = { vkState.swapChain.renderCompletedSemaphore };

            VkPresentInfoKHR vkPresentInfo {};
            vkPresentInfo.sType              =  VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            vkPresentInfo.pNext              =  nullptr;
            vkPresentInfo.waitSemaphoreCount =  1;
            vkPresentInfo.pWaitSemaphores    =  signalSemaphores;
            vkPresentInfo.swapchainCount     =  1;
            vkPresentInfo.pSwapchains        =  swapChains;
            vkPresentInfo.pImageIndices      = &(vkState.swapChain.currentSwapChainImageIndex);
            vkPresentInfo.pResults           =  nullptr;

            VkResult result = vkQueuePresentKHR(presentQueue, &vkPresentInfo);
            if(VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to execute 'vkQueuePresentKHR'", result);
            }

            // Temporary workaround to avoid memory depletion from GPU workloads using validation layers.
            // Implement better synchronization and throttling, once ready.
            // result = vkQueueWaitIdle(presentQueue);
            // if(VK_SUCCESS != result)
            // {
            //     throw CVulkanError("Failed to execute 'vkQueueWaitIdle' for temporary synchronization implementation", result);
            // }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::bindResource(PublicResourceId_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Binding resource with id %0", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::unbindResource(PublicResourceId_t const &aId)
        {
            CLog::Verbose(logTag(), CString::format("Unbinding resource with id %0", aId));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CVulkanRenderContext::render(SRenderable const &aRenderable)
        {
            CLog::Verbose(logTag(), CString::format("Rendering renderable: %0", to_string(aRenderable)));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

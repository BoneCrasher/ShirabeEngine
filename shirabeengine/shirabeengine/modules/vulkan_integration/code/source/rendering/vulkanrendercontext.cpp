#include "vulkan/rendering/vulkanrendercontext.h"

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

            VkResult result =
                    vkAcquireNextImageKHR(
                        device,
                        swapChain,
                        timeout,
                        semaphore,
                        VK_NULL_HANDLE,
                        &nextImageIndex);

            if(VkResult::VK_SUCCESS != result)
            {
                throw CVulkanError("Failed to execute 'vkAcquireNextImageKHR'.", result);
            }

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

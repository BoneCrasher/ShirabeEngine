//
// Created by dottideveloper on 28.12.21.
//

#include <graphicsapi/definitions.h>
#include <wsi/x11/x11window.h>

#include "rhi/vulkan_core/vulkanenvironment.h"
#include "rhi/vulkan_core/vulkandevicecapabilities.h"
#include "rhi/vulkan_wsi/wsi/x11surface.h"
#include "rhi/rhilayer.h"

namespace engine::rhi
{
    SHIRABE_DECLARE_LOG_TAG(RHILayer);

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CRHILayer::initializeLayer(SRHILayerInitArgs const &aInitArgs)
    {
        using namespace graphicsapi;

        if(mIsInitialized)
        {
            return EEngineStatus::RHI_Generic_AlreadyInitialized;
        }

        mGfxApi        = EGFXAPI::Vulkan;
        mGfxApiVersion = EGFXAPIVersion::Vulkan_1_1;

        if(EGFXAPI::Vulkan == mGfxApi)
        {
            Shared<vulkan::CVulkanEnvironment> vkEnvironment = makeShared<vulkan::CVulkanEnvironment>();

            EEngineStatus status = vkEnvironment->initialize(*(aInitArgs.applicationEnvironment));
            if(CheckEngineError(status))
            {
                EngineStatusPrintOnError(status, logTag(), "Vulkan initialization failed.");
                vkEnvironment->deinitialize();
                return status;
            }

            CEngineResult<VkSurfaceKHR> surfaceCreation = vulkan::CX11VulkanSurface::create(vkEnvironment, aInitArgs.display, aInitArgs.mainWindow);
            if(not surfaceCreation.successful())
            {
                CLog::Error(logTag(), "Failed to create vk surface.");
                vkEnvironment->deinitialize();

                return surfaceCreation.result();
            }

            vkEnvironment->setSurface(surfaceCreation.data());

            VkFormat const requiredFormat = vulkan::CVulkanDeviceCapsHelper::convertFormatToVk(EFormat::R8G8B8A8_UNORM);
            vkEnvironment->createSwapChain(
                aInitArgs.displayDescriptor.bounds,
                requiredFormat,
                VK_COLORSPACE_SRGB_NONLINEAR_KHR);

            vkEnvironment->initializeRecordingAndSubmission();

            mVkGlobalContext   = std::static_pointer_cast<IVkGlobalContext>(vkEnvironment);
            mVulkanEnvironment = vkEnvironment;
        }

        mIsInitialized = true;
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CRHILayer::deinitializeLayer()
    {
        if(not mIsInitialized)
        {
            return EEngineStatus::RHI_Generic_NotInitialized;
        }

        mGfxApi            = graphicsapi::EGFXAPI::Undefined;
        mGfxApiVersion     = graphicsapi::EGFXAPIVersion::Undefined;
        mVulkanEnvironment->deinitialize();
        mVulkanEnvironment = nullptr;
        mVkGlobalContext   = nullptr;

        mIsInitialized = false;
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<rhi::IVkGlobalContext> CRHILayer::getRHIGlobalContext()
    {
        return mVkGlobalContext;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<SRenderGraphRenderContext> CRHILayer::getRHIRenderContext()
    {
        return mRenderContext;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<SRenderGraphResourceContext> CRHILayer::getRHIResourceContext()
    {
        return mResourceContext;
    }
    //<-----------------------------------------------------------------------------
}

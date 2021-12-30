//
// Created by dottideveloper on 28.12.21.
//

#ifndef __SHIRABEDEVELOPMENT_RHILAYER_H__
#define __SHIRABEDEVELOPMENT_RHILAYER_H__

#include <base/declaration.h>
#include <platform/platform.h>
#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <graphicsapi/definitions.h>

/**
 * Endless predeclarations to avoid including all these headers :D
 */
namespace engine
{
    namespace wsi::x11
    {
        class CX11Display;
        class CX11Window;
    }
    namespace os
    {
        struct SOSDisplayDescriptor;
        class SApplicationEnvironment;
    }
    namespace rhi
    {
        class IVkGlobalContext;
    }
}

namespace engine::rhi
{
    using engine::os::SApplicationEnvironment;
    using engine::os::SOSDisplayDescriptor;
    using engine::wsi::x11::CX11Display;
    using engine::wsi::x11::CX11Window;
    using vulkan::CVulkanEnvironment;
    using rhi::IVkGlobalContext;

    /**
     * Initialization values required to initialize the RHI layer.
     */
    struct SHIRABEENGINE_LIBRARY_EXPORT SRHILayerInitArgs
    {
        Shared<SApplicationEnvironment> applicationEnvironment;
        struct SOSDisplayDescriptor     displayDescriptor;
#if defined SHIRABE_PLATFORM_LINUX
        Shared<CX11Display> display;
        Shared<CX11Window>  mainWindow;
#else
#endif
    };

    /**
     * RHI abstraction layer that will serve as a frontend to other RHI dependent engine
     * systems.
     *
     * Currently "hard bound" to vulkan...
     */
    class SHIRABEENGINE_LIBRARY_EXPORT CRHILayer final
    {
    private_members:
        Atomic<bool> mIsInitialized = false;

        EGFXAPI        mGfxApi;
        EGFXAPIVersion mGfxApiVersion;

        Shared<CVulkanEnvironment> mVulkanEnvironment;
        Shared<IVkGlobalContext>   mVkGlobalContext;

    public_constructors:
        CRHILayer()                 = default;
        CRHILayer(CRHILayer const&) = delete;
        CRHILayer(CRHILayer &&)     = delete;

    public_destructors:
        ~CRHILayer() = default;

    public_operators:
        CRHILayer& operator=(CRHILayer const&) = delete;
        CRHILayer& operator=(CRHILayer &&)     = delete;

    public_methods:
        [[nodiscard]] SHIRABE_INLINE EGFXAPI getSelectedGraphicsAPI() const
        {
            return mGfxApi;
        };

        [[nodiscard]] SHIRABE_INLINE EGFXAPIVersion getSelectedGraphicsAPIVersion() const
        {
            return mGfxApiVersion;
        }

        Shared<class IVkGlobalContext>            getRHIGlobalContext();
        Shared<class SRenderGraphRenderContext>   getRHIRenderContext();
        Shared<class SRenderGraphResourceContext> getRHIResourceContext();

        /**
         * Initialize the RHI layer and prepare all rhi subsystems for use.
         *
         * @return
         */
        EEngineStatus initializeLayer(SRHILayerInitArgs const &aInitArgs);

        /**
         * Shutdown RHI
         *
         * @return
         */
        EEngineStatus deinitializeLayer();

    };
}

#endif //__SHIRABEDEVELOPMENT_RHILAYER_H__

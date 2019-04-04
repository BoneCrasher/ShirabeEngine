#include <asset/assetindex.h>
#include <core/enginestatus.h>
#include <resources/core/resourcemanagerbase.h>
#include <resources/core/resourceproxyfactory.h>
#include <renderer/renderer.h>
#include <renderer/framegraph/framegraphrendercontext.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include <graphicsapi/resources/gfxapiresourceproxy.h>
#include <graphicsapi/resources/types/all.h>
#include <material/material_loader.h>
#include <vulkan/resources/vulkanresourcetaskbackend.h>
#include <vulkan/rendering/vulkanrendercontext.h>
#include <vulkan/vulkandevicecapabilities.h>

#include <wsi/display.h>

#if defined SHIRABE_PLATFORM_LINUX
    #include <wsi/x11/x11display.h>
    #include <wsi/x11/x11windowfactory.h>
    #include <vulkan/wsi/x11surface.h>
#elif defined SHIRABE_PLATFORM_WINDOWS
    #include <wsi/windows/windowsdisplay.h>
    #include <wsi/windows/windowswindowfactory.h>
#endif

#include "resource_management/resourcemanager.h"
#include "core/engine.h"

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    class CTestDummy
            : public IWindow::IEventCallback
    {
        SHIRABE_DECLARE_LOG_TAG("TestDummy")

        public_methods:
            void onResume(CStdSharedPtr_t<IWindow> const &)
        {
            //Log::Status(logTag(), "OnResume");
        }

        void onShow(CStdSharedPtr_t<IWindow> const &)
        {
            //Log::Status(logTag(), "onShow");
        }

        void onBoundsChanged(
                CStdSharedPtr_t<IWindow> const &,
                CRect                    const &)
        {
            //Log::Status(logTag(), String::format("onBoundsChanged: %0/%1/%2/%3", r.m_position.x(), r.m_position.y(), r.m_size.x(), r.m_size.y()));
        }

        void onHide(CStdSharedPtr_t<IWindow> const &)
        {
            //Log::Status(logTag(), "onHide");
        }

        void onPause(CStdSharedPtr_t<IWindow> const &)
        {
            //Log::Status(logTag(), "onPause");
        }

        void onClose(CStdSharedPtr_t<IWindow> const &)
        {
            // Log::Status(logTag(), "onClose");
            // PostQuitMessage(0);
        }

        void onDestroy(CStdSharedPtr_t<IWindow> const &)
        {

        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineInstance::CEngineInstance(CStdSharedPtr_t<os::SApplicationEnvironment> const &aEnvironment)
        : mApplicationEnvironment(aEnvironment)
        , mWindowManager    (nullptr) // Do not initialize here, to avoid exceptions in constructor. Memory leaks!!!
        , mMainWindow       (nullptr)
        , mAssetStorage     (nullptr)
        , mProxyFactory     (nullptr)
        , mResourceManager  (nullptr)
        , mVulkanEnvironment(nullptr)
        , mRenderer         (nullptr)
        , mScene            ({})
    { }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineInstance::~CEngineInstance()
    {
        // Fool-Proof redundant check
        if(mWindowManager)
        {
            mWindowManager = nullptr;
        }
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    /**
     * Templated helper struct to spawn a typed gfxapi backend resource proxy creator functor.
     */
    template <typename TResource>
    struct SSpawnProxy
    {
        /**
         * Create and return a gfxapi resource backend proxy creation functor from TResource.
         *
         * @param aBackend The backend to create a proxy for.
         * @return         A valid creation functor for a proxy of type TResource.
         */
        static CResourceProxyFactory::CreatorFn_t<TResource> forGFXAPIBackend(CStdSharedPtr_t<CGFXAPIResourceBackend> aBackend)
        {
            auto const creator = [=]() -> CResourceProxyFactory::CreatorFn_t<TResource>
            {
                return
                        [=](resources::EProxyType const &aType, typename TResource::CCreationRequest const &aRequest)
                        -> CStdSharedPtr_t<IResourceProxy<TResource>>
                {
                    return makeCStdSharedPtr<CGFXAPIResourceProxy<TResource>>(aType, aRequest, aBackend);
                };
            };

            return creator();
        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CEngineInstance::initialize()
    {
        EEngineStatus status = EEngineStatus::Ok;

        CStdSharedPtr_t<CGFXAPIResourceBackend> resourceBackend = nullptr;
        CStdSharedPtr_t<CWSIDisplay>            display         = nullptr;

#if defined SHIRABE_PLATFORM_LINUX
        CStdSharedPtr_t<x11::CX11Display> x11Display = makeCStdSharedPtr<x11::CX11Display>();
        display = x11Display;
#elif defined SHIRABE_PLATFORM_WINDOWS
#endif

        status = display->initialize();

        SOSDisplayDescriptor const&displayDesc = display->screenInfo()[display->primaryScreenIndex()];

        uint32_t const
                windowWidth  = displayDesc.bounds.size.x(),
                windowHeight = displayDesc.bounds.size.y();

        EGFXAPI        const gfxApi        = EGFXAPI::Vulkan;
        EGFXAPIVersion const gfxApiVersion = EGFXAPIVersion::Vulkan_1_1;

        auto const fnCreatePlatformWindowSystem = [&, this] () -> CEngineResult<>
        {
            CEngineResult<> result = { EEngineStatus::Ok };

            CStdSharedPtr_t<IWindowFactory> factory = nullptr;

#ifdef SHIRABE_PLATFORM_WINDOWS
            factory = makeCStdSharedPtr<WSI::Windows::WindowsWindowFactory>((HINSTANCE)aApplicationEnvironment.instanceHandle);
#elif defined SHIRABE_PLATFORM_LINUX
            factory = makeCStdSharedPtr<x11::CX11WindowFactory>(x11Display);
#endif // SHIRABE_PLATFORM_WINDOWS

            mWindowManager = makeCStdSharedPtr<CWindowManager>();

            CWindowManager::EWindowManagerError windowManagerError = mWindowManager->initialize(*mApplicationEnvironment, factory);
            if(nullptr == mWindowManager && not CheckWindowManagerError(windowManagerError))
            {
                CLog::Error(logTag(), "Failed to create WindowManager.");
                result = CEngineResult<>(EEngineStatus::InitializationError);

                return result;
            }

            mMainWindow = mWindowManager->createWindow("MainWindow", CRect(0, 0, windowWidth, windowHeight));
            if(nullptr == mMainWindow)
            {
                CLog::Error(logTag(), "Failed to create main window in WindowManager.");
                result = CEngineResult<>(EEngineStatus::WindowCreationError);

                return result;
            }

            EEngineStatus status = mMainWindow->resume();
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), "Failed to resume operation in main window.");
                return { status };
            }

            status = mMainWindow->show();
            if(CheckEngineError(status))
            {
                CLog::Error(logTag(), "Failed to show main window.");
                return { status };
            }

            CStdSharedPtr_t<IWindow::IEventCallback> dummy = makeCStdSharedPtr<CTestDummy>();
            mMainWindow->registerCallback(dummy);

            return { EEngineStatus::Ok };
        };

        SRendererConfiguration rendererConfiguration = {};
        rendererConfiguration.enableVSync             = true;
        rendererConfiguration.frustum                 = CVector4D_t({ static_cast<float const>(windowWidth), static_cast<float const>(windowHeight), 0.1f, 1000.0f });
        rendererConfiguration.preferredBackBufferSize = CVector2D<uint32_t>({ windowWidth, windowHeight });
        rendererConfiguration.preferredWindowSize     = rendererConfiguration.preferredBackBufferSize;
        rendererConfiguration.requestFullscreen       = false;

        CStdSharedPtr_t<material::CMaterialLoader> materialLoader = nullptr;

        auto const fnCreateDefaultGFXAPI = [&, this] () -> CEngineResult<>
        {
            if(EGFXAPI::Vulkan == gfxApi)
            {
                mVulkanEnvironment = makeCStdSharedPtr<CVulkanEnvironment>();
                EEngineStatus status = mVulkanEnvironment->initialize(*mApplicationEnvironment);

                if(CheckEngineError(status))
                {
                    EngineStatusPrintOnError(status, logTag(), "Vulkan initialization failed.");

                    return { status };
                }


                CEngineResult<VkSurfaceKHR> surfaceCreation = vulkan::CX11VulkanSurface::create(mVulkanEnvironment, x11Display, std::static_pointer_cast<x11::CX11Window>(mMainWindow));
                if(not surfaceCreation.successful())
                {
                    CLog::Error(logTag(), "Failed to create vk surface.");
                    mVulkanEnvironment->deinitialize();

                    return { surfaceCreation.result() };
                }

                mVulkanEnvironment->setSurface(surfaceCreation.data());

                VkFormat const requiredFormat = CVulkanDeviceCapsHelper::convertFormatToVk(Format::R8G8B8A8_UNORM);
                mVulkanEnvironment->createSwapChain(
                             displayDesc.bounds,
                             requiredFormat,
                             VK_COLORSPACE_SRGB_NONLINEAR_KHR);
            }

            return { EEngineStatus::Ok };
        };

        auto const fnCreatePlatformResourceSystem = [&, this] () -> CEngineResult<>
        {
            // Instantiate the appropriate gfx api from engine config, BUT match it against
            // the platform capabilities!
            // --> #ifndef WIN32 Fallback to Vk. If Vk is not available, fallback to OpenGL, put that into "ChooseGfxApi(preferred) : EGfxApiID"

            // Create all necessary subsystems.
            // Their life-cycle management will become the manager's task.
            // The resourceBackend-swithc for the desired platform will be here (if(dx11) ... elseif(vulkan1) ... ).
            //

            CAssetStorage::AssetRegistry_t assetIndex ={}; // AssetIndex::loadIndexById("");

            CStdUniquePtr_t<IAssetDataSource> assetDataSource = nullptr;
            CStdSharedPtr_t<CAssetStorage>    assetStorage    = makeCStdSharedPtr<CAssetStorage>(std::move(assetDataSource));
            mAssetStorage->readIndex(assetIndex);
            mAssetStorage = assetStorage;

            materialLoader = makeCStdSharedPtr<material::CMaterialLoader>(assetStorage);

            CStdSharedPtr_t<CGFXAPIResourceTaskBackend> resourceTaskBackend = nullptr;

            // The graphics API resource backend is static and does not have to be replaced.
            // On switching the graphics API the task backend (also containing the effective API handles),
            // will be reset and replaced!
            // This way, the resources can simply be reloaded on demand.
            // Nonetheless, a regular backend reset has to take place in order to have the proxies remain in a valid state!
            // The reset of the taskbackend should thus occur through the resource backend.
            if(EGFXAPI::Vulkan == gfxApi)
            {
                CStdSharedPtr_t<CVulkanResourceTaskBackend> vkResourceTaskBackend = makeCStdSharedPtr<CVulkanResourceTaskBackend>(mVulkanEnvironment);
                vkResourceTaskBackend->initialize();

                resourceTaskBackend = vkResourceTaskBackend;
            }

            resourceBackend = makeCStdSharedPtr<CGFXAPIResourceBackend>();
            resourceBackend->setResourceTaskBackend(resourceTaskBackend);
            resourceBackend->initialize();

            mProxyFactory = makeCStdSharedPtr<CResourceProxyFactory>();
            mProxyFactory->addCreator<CTexture>    (EResourceSubType::TEXTURE_2D,   SSpawnProxy<CTexture>::forGFXAPIBackend(resourceBackend));
            mProxyFactory->addCreator<CTextureView>(EResourceSubType::TEXTURE_VIEW, SSpawnProxy<CTextureView>::forGFXAPIBackend(resourceBackend));
            mProxyFactory->addCreator<CRenderPass> (EResourceSubType::RENDER_PASS,  SSpawnProxy<CRenderPass>::forGFXAPIBackend(resourceBackend));
            mProxyFactory->addCreator<CFrameBuffer>(EResourceSubType::FRAME_BUFFER, SSpawnProxy<CFrameBuffer>::forGFXAPIBackend(resourceBackend));

            CStdSharedPtr_t<CResourceManagerBase> manager = makeCStdSharedPtr<CResourceManager>(mProxyFactory);
            mResourceManager = manager;            
            mResourceManager->initialize();

            return { EEngineStatus::Ok };
        };

        auto const fnCreatePlatformRenderer = [&, this] () -> CEngineResult<>
        {
            using engine::framegraph::IFrameGraphRenderContext;
            using engine::framegraph::CFrameGraphRenderContext;

            // How to decouple?
            CStdSharedPtr_t<IRenderContext> gfxApiRenderContext = nullptr;
            if(EGFXAPI::Vulkan == gfxApi)
            {
                CStdSharedPtr_t<CVulkanRenderContext> vulkanRenderContext = makeCStdSharedPtr<CVulkanRenderContext>();
                vulkanRenderContext->initialize(mVulkanEnvironment, resourceBackend);

                gfxApiRenderContext = vulkanRenderContext;
            }

            CEngineResult<CStdSharedPtr_t<IFrameGraphRenderContext>> frameGraphRenderContext = CFrameGraphRenderContext::create(mAssetStorage, materialLoader, mResourceManager, gfxApiRenderContext);

            mRenderer = makeCStdSharedPtr<CRenderer>();
            status    = mRenderer->initialize(mApplicationEnvironment, display, rendererConfiguration, frameGraphRenderContext.data());
            if(false == CheckEngineError(status))
            {
                CEngineResult<> initialization = mScene.initialize();
                status = initialization.result();
            }

            return { status };
        };

        try
        {
            CEngineResult<> creation = { EEngineStatus::Ok };

            mTimer.initialize();
            mTimer.setTickDeltaMilliseconds(1000.0 / 60.0);

            creation = fnCreatePlatformWindowSystem();
            creation = fnCreateDefaultGFXAPI();
            creation = fnCreatePlatformResourceSystem();
            creation = fnCreatePlatformRenderer();

        }
        catch(std::exception const stde)
        {
            CLog::Error(logTag(), stde.what());
            return EEngineStatus::Error;
        }
        catch(...)
        {
            CLog::Error(logTag(), "Unknown error occurred.");
            return EEngineStatus::Error;
        }

        return status;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CEngineInstance::deinitialize()
    {
        EEngineStatus status = EEngineStatus::Ok;

        if(nullptr != mResourceManager)
        {
            mResourceManager->clear(); // Will implicitely clear all subsystems!
            mResourceManager = nullptr;
        }

        if(nullptr != mProxyFactory)
        {
            mProxyFactory = nullptr;
        }

        if(nullptr != mRenderer)
        {
                status = mRenderer->deinitialize();
        }

        if(nullptr != mMainWindow)
        {
                mMainWindow->hide();
                mMainWindow->pause();
                // TODO: Handle errors
                mMainWindow = nullptr;
        }

        mTimer.cleanup();

        mWindowManager = nullptr;

        return status;
   }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CEngineInstance::update()
    {
        mTimer.update();

        if(CheckWindowManagerError(mWindowManager->update()))
        {
                CLog::Error(logTag(), "Failed to update window manager.");
                return { EEngineStatus::UpdateError };
        }        

        if(not mTimer.isTick())
        {
            return { EEngineStatus::Ok };
        }

        mTimer.resetTick();

        mScene.update();

        if(mRenderer)
        {
            mRenderer->renderScene();
        }

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

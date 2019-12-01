#include <filesystem>

#include <asset/assetindex.h>
#include <asset/filesystemassetdatasource.h>
#include <core/enginestatus.h>
#include <renderer/renderer.h>
#include <renderer/framegraph/framegraphrendercontext.h>
#include <graphicsapi/definitions.h>
#include <material/material_loader.h>
#include <material/assetloader.h>
#include <resources/agpuapiresourceobjectfactory.h>
#include <vulkan_integration/rendering/vulkanrendercontext.h>
#include <vulkan_integration/vulkandevicecapabilities.h>
#include <vulkan_integration/resources/cvulkanprivateresourceobjectfactory.h>

#include <wsi/display.h>

#include "ecws/meshcomponent.h"
#include "ecws/materialcomponent.h"

#if defined SHIRABE_PLATFORM_LINUX
    #include <wsi/x11/x11display.h>
    #include <wsi/x11/x11windowfactory.h>
    #include <vulkan_integration/wsi/x11surface.h>
#elif defined SHIRABE_PLATFORM_WINDOWS
    #include <wsi/windows/windowsdisplay.h>
    #include <wsi/windows/windowswindowfactory.h>
#endif

#include <material/assetloader.h>
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
        void onResume(Shared<IWindow> const &) override
        {
            //Log::Status(logTag(), "OnResume");
        }

        void onShow(Shared<IWindow> const &) override
        {
            //Log::Status(logTag(), "onShow");
        }

        void onBoundsChanged(
                Shared<IWindow> const &,
                CRect                    const &) override
        {
            //Log::Status(logTag(), String::format("onBoundsChanged: {}/{}/{}/{}", r.m_position.x(), r.m_position.y(), r.m_size.x(), r.m_size.y()));
        }

        void onHide(Shared<IWindow> const &) override
        {
            //Log::Status(logTag(), "onHide");
        }

        void onPause(Shared<IWindow> const &) override
        {
            //Log::Status(logTag(), "onPause");
        }

        void onClose(Shared<IWindow> const &) override
        {
            // Log::Status(logTag(), "onClose");
            // PostQuitMessage(0);
        }

        void onDestroy(Shared<IWindow> const &) override
        {

        }
    };
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineInstance::CEngineInstance(Shared<os::SApplicationEnvironment> aEnvironment)
        : mApplicationEnvironment(std::move(aEnvironment))
        , mWindowManager    (nullptr) // Do not initialize here, to avoid exceptions in constructor. Memory leaks!!!
        , mMainWindow       (nullptr)
        , mAssetStorage     (nullptr)
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

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CEngineInstance::initialize()
    {
        EEngineStatus status = EEngineStatus::Ok;

        Shared<CWSIDisplay> display = nullptr;

#if defined SHIRABE_PLATFORM_LINUX
        Shared<x11::CX11Display> x11Display = makeShared<x11::CX11Display>();
        display = x11Display;
#elif defined SHIRABE_PLATFORM_WINDOWS
#endif

        status = display->initialize();

        SOSDisplayDescriptor const&displayDesc = display->screenInfo()[display->primaryScreenIndex()];

        uint32_t const
                windowWidth  = displayDesc.bounds.size.x(),
                windowHeight = displayDesc.bounds.size.y();

        EGFXAPI        gfxApi        = EGFXAPI::Vulkan;
        EGFXAPIVersion gfxApiVersion = EGFXAPIVersion::Vulkan_1_1;

        auto const fnCreatePlatformWindowSystem = [&, this] () -> CEngineResult<>
        {
            CEngineResult<> result = { EEngineStatus::Ok };

            Shared<IWindowFactory> factory = nullptr;

#ifdef SHIRABE_PLATFORM_WINDOWS
            factory = makeShared<WSI::Windows::WindowsWindowFactory>((HINSTANCE)aApplicationEnvironment.instanceHandle);
#elif defined SHIRABE_PLATFORM_LINUX
            factory = makeShared<x11::CX11WindowFactory>(x11Display);
#endif // SHIRABE_PLATFORM_WINDOWS

            mWindowManager = makeShared<CWindowManager>();

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

            Shared<IWindow::IEventCallback> dummy = makeShared<CTestDummy>();
            mMainWindow->registerCallback(dummy);

            return { EEngineStatus::Ok };
        };

        SRendererConfiguration rendererConfiguration = {};
        rendererConfiguration.enableVSync             = false;
        rendererConfiguration.frustum                 = CVector4D_t({ static_cast<float const>(windowWidth), static_cast<float const>(windowHeight), 0.1f, 1000.0f });
        rendererConfiguration.preferredBackBufferSize = CVector2D<uint32_t>({ windowWidth, windowHeight });
        rendererConfiguration.preferredWindowSize     = rendererConfiguration.preferredBackBufferSize;
        rendererConfiguration.requestFullscreen       = false;

        Shared<CGpuApiResourceStorage> gpuApiResourceStorage = makeShared<CGpuApiResourceStorage>();

        auto const fnCreateDefaultGFXAPI = [&, this] () -> CEngineResult<>
        {
            if(EGFXAPI::Vulkan == gfxApi)
            {
                mVulkanEnvironment = makeShared<CVulkanEnvironment>();
                EEngineStatus status = mVulkanEnvironment->initialize(*mApplicationEnvironment, gpuApiResourceStorage);

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

                VkFormat const requiredFormat = CVulkanDeviceCapsHelper::convertFormatToVk(EFormat::R8G8B8A8_UNORM);
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


            std::filesystem::path const root          = std::filesystem::current_path();
            std::filesystem::path const resourcesPath = root/"data/output/resources";

            CAssetStorage::AssetRegistry_t assetIndex = asset::CAssetIndex::loadIndexById(resourcesPath/"game.assetindex.xml");

            Unique<IAssetDataSource> assetDataSource = makeUnique<CFileSystemAssetDataSource>(resourcesPath);
            Shared<CAssetStorage>    assetStorage    = makeShared<CAssetStorage>(std::move(assetDataSource));
            assetStorage->readIndex(assetIndex);
            mAssetStorage = assetStorage;

            mMaterialLoader = makeShared<material::CMaterialLoader>();

            Unique<CGpuApiResourceObjectFactory> gpuApiResourceFactory = nullptr;

            // The graphics API resource backend is static and does not have to be replaced.
            // On switching the graphics API the task backend (also containing the effective API handles),
            // will be reset and replaced!
            // This way, the resources can simply be reloaded on demand.
            // Nonetheless, a regular backend reset has to take place in order to have the proxies remain in a valid state!
            // The reset of the taskbackend should thus occur through the resource backend.
            if(EGFXAPI::Vulkan == gfxApi)
            {
                gpuApiResourceFactory = makeUnique<CVulkanResourceObjectFactory>(gpuApiResourceStorage);

                CEngineResult<> initializationResult = ((CVulkanResourceObjectFactory *)gpuApiResourceFactory.get())->initialize(mVulkanEnvironment);
                if(CheckEngineError(initializationResult.result()))
                {
                    CLog::Error(logTag(), "Failed to initialize vulkan resource factory.");
                    return { EEngineStatus::Error };
                }
            }

            Shared<CResourceManager> manager = makeShared<CResourceManager>(std::move(gpuApiResourceFactory), mAssetStorage);
            // manager->addAssetLoader<SMesh>(...);
            manager->addAssetLoader<SMaterial>(material::getAssetLoader(manager, mAssetStorage, mMaterialLoader));
            // manager->addAssetLoader<SBuffer>  (makeUnique<CResourceFromAssetResourceObjectCreator<SBuffer>>  ([assetStorage](ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject> { return nullptr; }));
            // manager->addAssetLoader<STexture> (makeUnique<CResourceFromAssetResourceObjectCreator<STexture>> ([assetStorage](ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject> { return nullptr; }));
            mResourceManager = manager;

            return { EEngineStatus::Ok };
        };

        auto const fnCreatePlatformRenderer = [&, this] () -> CEngineResult<>
        {
            using engine::framegraph::IFrameGraphRenderContext;
            using engine::framegraph::CFrameGraphRenderContext;

            // How to decouple?
            Shared<IRenderContext> gfxApiRenderContext = nullptr;
            if(EGFXAPI::Vulkan == gfxApi)
            {
                Shared<CVulkanRenderContext> vulkanRenderContext = makeShared<CVulkanRenderContext>();
                vulkanRenderContext->initialize(mVulkanEnvironment, gpuApiResourceStorage);

                gfxApiRenderContext = vulkanRenderContext;
            }

            CEngineResult<Shared<IFrameGraphRenderContext>> frameGraphRenderContext = CFrameGraphRenderContext::create(mAssetStorage, mMaterialLoader, mResourceManager, gfxApiRenderContext);

            mRenderer = makeShared<CRenderer>();
            status    = mRenderer->initialize(mApplicationEnvironment, display, rendererConfiguration, frameGraphRenderContext.data());

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

            // Setup scene
            CEngineResult<> initialization = mScene.initialize();
            status = initialization.result();

            auto const &[result, material] = mMaterialLoader->loadMaterialInstance(mAssetStorage, 317299467, true);

            auto materialComponent = makeShared<ecws::CMaterialComponent>();
            materialComponent->setMaterialInstance(material);
            materialComponent->getMutableConfiguration().setBufferValue<float>("struct_modelMatrices", "horizontalScale", 0.5f);

            auto cube = makeUnique<ecws::CEntity>();
            cube->addComponent<ecws::CMaterialComponent>(materialComponent);

            mScene.addEntity(std::move(cube));

        }
        catch(std::exception &stde)
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
            // mResourceManager->reset(); // Will implicitely clear all subsystems!
            mResourceManager = nullptr;
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
            RenderableList renderableCollection {};

            Vector<Unique<ecws::CEntity>> const &entities = mScene.getEntities();
            for(auto const &entity : entities)
            {
                std::string const &name = entity->name();
                ecws::CBoundedCollection<Shared<ecws::CMeshComponent>>     meshes    = entity->getTypedComponentsOfType<ecws::CMeshComponent>();
                ecws::CBoundedCollection<Shared<ecws::CMaterialComponent>> materials = entity->getTypedComponentsOfType<ecws::CMaterialComponent>();

                // for(auto const &mesh : meshes)
                    for(auto const &material : materials)
                        renderableCollection.push_back({ name
                                                         , ""
                                                         , 0
                                                         , material->getMaterialInstance()->name()
                                                         , material->getMaterialInstance()->master()->getAssetId() });
            }

            mRenderer->renderScene(renderableCollection);
        }

        return { EEngineStatus::Ok };
    }
    //<-----------------------------------------------------------------------------
}

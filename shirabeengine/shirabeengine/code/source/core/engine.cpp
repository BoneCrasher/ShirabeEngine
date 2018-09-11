#include <asset/assetindex.h>
#include <core/enginestatus.h>
#include <resources/core/resourcemanagerbase.h>
#include <resources/core/resourceproxyfactory.h>
#include <renderer/renderer.h>
#include <renderer/framegraph/framegraphrendercontext.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include <graphicsapi/resources/gfxapiresourceproxy.h>
#include <graphicsapi/resources/types/all.h>
#include <vulkan/resources/vulkanresourcetaskbackend.h>
#include <vulkan/rendering/vulkanrendercontext.h>

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
    EEngineStatus CEngineInstance::initialize()
    {
        EEngineStatus status = EEngineStatus::Ok;

        uint32_t const windowWidth  = mApplicationEnvironment->osDisplays[0].bounds.size.x();
        uint32_t const windowHeight = mApplicationEnvironment->osDisplays[0].bounds.size.y();

        EGFXAPI        const gfxApi        = EGFXAPI::Vulkan;
        EGFXAPIVersion const gfxApiVersion = EGFXAPIVersion::Vulkan_1_1;

        auto const fnCreatePlatformWindowSystem = [&, this] () -> void
        {
            EEngineStatus status = EEngineStatus::Ok;

            mWindowManager = makeCStdSharedPtr<CWindowManager>();

            CWindowManager::EWindowManagerError windowManagerError = mWindowManager->initialize(*mApplicationEnvironment);
            if(!(mWindowManager && !CheckWindowManagerError(windowManagerError)))
            {
                status = EEngineStatus::EngineComponentInitializationError;
                HandleEngineStatusError(status, "Failed to create WindowManager.");
            }

            mMainWindow = mWindowManager->createWindow("MainWindow", CRect(0, 0, windowWidth, windowHeight));
            if(!mMainWindow)
            {
                status = EEngineStatus::WindowCreationError;
                HandleEngineStatusError(status, "Failed to create main window in WindowManager.");
            }
            else
            {
                status = mMainWindow->resume();
                HandleEngineStatusError(status, "Failed to resume operation in main window.");

                if(!CheckEngineError(status))
                {
                    status = mMainWindow->show();
                    HandleEngineStatusError(status, "Failed to show main window.");
                }
            }

            CStdSharedPtr_t<IWindow::IEventCallback> dummy = makeCStdSharedPtr<CTestDummy>();
            mMainWindow->registerCallback(dummy);
        };

        SRendererConfiguration rendererConfiguration = {};
        rendererConfiguration.enableVSync             = true;
        rendererConfiguration.frustum                 = CVector4D_t({ static_cast<float const>(windowWidth), static_cast<float const>(windowHeight), 0.1f, 1000.0f });
        rendererConfiguration.preferredBackBufferSize = CVector2D<uint32_t>({ windowWidth, windowHeight });
        rendererConfiguration.preferredWindowSize     = rendererConfiguration.preferredBackBufferSize;
        rendererConfiguration.requestFullscreen       = false;

        std::function<void()> fnCreateDefaultGFXAPI
                = [&, this] () -> void
        {
            mVulkanEnvironment = makeCStdSharedPtr<CVulkanEnvironment>();

            EEngineStatus status = mVulkanEnvironment->initialize(*mApplicationEnvironment);
            HandleEngineStatusError(status, "Vulkan initialization failed.");
        };

        std::function<void()> fnCreatePlatformResourceSystem
                = [&, this] () -> void
        {
            // Instantiate the appropriate gfx api from engine config, BUT match it against
            // the platform capabilities!
            // --> #ifndef WIN32 Fallback to Vk. If Vk is not available, fallback to OpenGL, put that into "ChooseGfxApi(preferred) : EGfxApiID"

            // Create all necessary subsystems.
            // Their life-cycle management will become the manager's task.
            // The resourceBackend-swithc for the desired platform will be here (if(dx11) ... elseif(vulkan1) ... ).
            //

            CAssetStorage::AssetIndex_t assetIndex ={}; // AssetIndex::loadIndexById("");
            CStdSharedPtr_t<CAssetStorage> assetStorage = makeCStdSharedPtr<CAssetStorage>();
            mAssetStorage->readIndex(assetIndex);
            mAssetStorage = assetStorage;

            CStdSharedPtr_t<CGFXAPIResourceBackend>     resourceBackend     = makeCStdSharedPtr<CGFXAPIResourceBackend>();
            CStdSharedPtr_t<CGFXAPIResourceTaskBackend> resourceTaskBackend = nullptr;

            mProxyFactory = makeCStdSharedPtr<CResourceProxyFactory>();
            mProxyFactory->addCreator<CTexture>    (EResourceSubType::TEXTURE_2D,   SSpawnProxy<CTexture>::forGFXAPIBackend(resourceBackend));
            mProxyFactory->addCreator<CTextureView>(EResourceSubType::TEXTURE_VIEW, SSpawnProxy<CTextureView>::forGFXAPIBackend(resourceBackend));

            CStdSharedPtr_t<CResourceManagerBase> manager = makeCStdSharedPtr<CResourceManager>(mProxyFactory);
            mResourceManager = manager;

            // The graphics API resource backend is static and does not have to be replaced.
            // On switching the graphics API the task backend (also containing the effective API handles),
            // will be reset and replaced!
            // This way, the resources can simply be reloaded on demand.
            // Nonetheless, a regular backend reset has to take place in order to have the proxies remain in a valid state!
            // The reset of the taskbackend should thus occur through the resource backend.
            if(gfxApi == EGFXAPI::Vulkan)
            {
                CStdSharedPtr_t<CVulkanResourceTaskBackend> vkResourceTaskBackend = makeCStdSharedPtr<CVulkanResourceTaskBackend>(mVulkanEnvironment);
                vkResourceTaskBackend->initialize();

                resourceTaskBackend = vkResourceTaskBackend;
            }

            resourceBackend->setResourceTaskBackend(resourceTaskBackend);
            resourceBackend->initialize();
        };

        auto const fnCreatePlatformRenderer
                = [&, this] () -> void
        {
            using engine::framegraph::IFrameGraphRenderContext;
            using engine::framegraph::CFrameGraphRenderContext;

            // How to decouple?
            CStdSharedPtr_t<IRenderContext> gfxApiRenderContext = nullptr;
            if(gfxApi == EGFXAPI::Vulkan)
                gfxApiRenderContext = makeCStdSharedPtr<CVulkanRenderContext>();

            CStdSharedPtr_t<IFrameGraphRenderContext> frameGraphRenderContext = CFrameGraphRenderContext::create(mAssetStorage, mResourceManager, gfxApiRenderContext);

            mRenderer = makeCStdSharedPtr<CRenderer>();
            status = mRenderer->initialize(mApplicationEnvironment, rendererConfiguration, frameGraphRenderContext);
            if(!CheckEngineError(status))
            {
                status = mScene.initialize();
            }
        };

        try
        {
            fnCreatePlatformWindowSystem();
            fnCreateDefaultGFXAPI();
            fnCreatePlatformResourceSystem();
            fnCreatePlatformRenderer();

        }
        catch(CEngineException const e)
        {
            CLog::Error(logTag(), e.message());
            return e.status();
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
    EEngineStatus CEngineInstance::deinitialize()
    {
        EEngineStatus status = EEngineStatus::Ok;

        if(mResourceManager)
        {
            mResourceManager->clear(); // Will implicitely clear all subsystems!
            mResourceManager = nullptr;
        }

        if(mProxyFactory)
        {
            mProxyFactory = nullptr;
        }

        if(mRenderer)
        {
                status = mRenderer->deinitialize();
        }

        if(mMainWindow)
        {
                mMainWindow->hide();
                mMainWindow->pause();
                // TODO: Handle errors
                mMainWindow = nullptr;
        }

        mWindowManager = nullptr;

        return status;
   }

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    EEngineStatus CEngineInstance::update()
    {
        if(CheckWindowManagerError(mWindowManager->update()))
        {
                CLog::Error(logTag(), "Failed to update window manager.");
                return EEngineStatus::EngineComponentUpdateError;
        }

        mScene.update();

        mRenderer->renderScene();

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

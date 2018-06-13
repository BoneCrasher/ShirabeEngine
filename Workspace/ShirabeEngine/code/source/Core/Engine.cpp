#include "Core/Engine.h"

#include "Asset/AssetIndex.h"
#include "Resources/Core/ResourceManager.h"

#include "Renderer/Renderer.h"
#include "Renderer/FrameGraph/FrameGraphRenderContext.h"

#include "GFXAPI/Vulkan/Resources/ResourceTaskBackend.h"
#include "GFXAPI/Vulkan/Rendering/VulkanRenderContext.h"

namespace Engine {

  class TestDummy
    : public IWindow::IEventCallback {
  public:
    DeclareLogTag("TestDummy")

      void onResume(IWindowPtr const&) {
      //Log::Status(logTag(), "OnResume");
    }

    void onShow(IWindowPtr const&) {
      //Log::Status(logTag(), "onShow");
    }

    void onBoundsChanged(IWindowPtr const&,
      Rect const& r) {
      //Log::Status(logTag(), String::format("onBoundsChanged: %0/%1/%2/%3", r.m_position.x(), r.m_position.y(), r.m_size.x(), r.m_size.y()));
    }

    void onHide(IWindowPtr const&) {
      //Log::Status(logTag(), "onHide");
    }

    void onPause(IWindowPtr const&) {
      //Log::Status(logTag(), "onPause");
    }

    void onClose(IWindowPtr const&) {
      //Log::Status(logTag(), "onClose");
      PostQuitMessage(0);
    }

  };

  EngineInstance::EngineInstance(
    Ptr<Platform::ApplicationEnvironment> const&environment)
    : m_environment(environment)
    , m_windowManager(nullptr)   // Do not initialize here, to avoid exceptions in constructor. Memory leaks!!!
    , m_mainWindow(nullptr)
    , m_vulkanEnvironment(nullptr)
  {
  }

  EngineInstance::~EngineInstance() {
    // Fool-Proof redundant check
    if(m_windowManager)
      m_windowManager = nullptr;
  }

  EEngineStatus EngineInstance::initialize() {
    using namespace Engine::DX::_11;


    EEngineStatus status = EEngineStatus::Ok;

    unsigned long const& windowWidth  = m_environment->osDisplays[0].bounds.size.x();
    unsigned long const& windowHeight = m_environment->osDisplays[0].bounds.size.y();

    EGFXAPI        gfxApi        = EGFXAPI::Vulkan;
    EGFXAPIVersion gfxApiVersion = EGFXAPIVersion::Vulkan_1_1;

    std::function<void()> fnCreatePlatformWindowSystem
      = [&, this] () -> void
    {
      EEngineStatus status = EEngineStatus::Ok;

      m_windowManager = MakeSharedPointerType<WindowManager>();
      if(!(m_windowManager && !CheckWindowManagerError(m_windowManager->initialize(*m_environment)))) {
        status = EEngineStatus::EngineComponentInitializationError;
        HandleEngineStatusError(status, "Failed to create WindowManager.");
      }

      m_mainWindow = m_windowManager->createWindow("MainWindow", Rect(0, 0, windowWidth, windowHeight));
      if(!m_mainWindow) {
        status = EEngineStatus::WindowCreationError;
        HandleEngineStatusError(status, "Failed to create main window in WindowManager.");
      }
      else {
        m_environment->primaryWindowHandle = m_mainWindow->handle();

        status = m_mainWindow->resume();
        HandleEngineStatusError(status, "Failed to resume operation in main window.");

        if(!CheckEngineError(status)) {
          status = m_mainWindow->show();
          HandleEngineStatusError(status, "Failed to show main window.");
        }
      }

      IWindow::IEventCallbackPtr dummy = MakeSharedPointerType<TestDummy>();
      m_mainWindow->registerCallback(dummy);
    };

    RendererConfiguration rendererConfiguration;
    rendererConfiguration.enableVSync             = true;
    rendererConfiguration.frustum                 ={ static_cast<float const>(windowWidth), static_cast<float const>(windowHeight), 0.1f, 1000.0f };
    rendererConfiguration.preferredBackBufferSize ={ windowWidth, windowHeight };
    rendererConfiguration.preferredWindowSize     = rendererConfiguration.preferredBackBufferSize;
    rendererConfiguration.requestFullscreen       = false;

    std::function<void()> fnCreateDefaultGFXAPI
      = [&, this] () -> void
    {
      m_vulkanEnvironment = MakeSharedPointerType<VulkanEnvironment>();

      EEngineStatus status = m_vulkanEnvironment->initialize(*m_environment);
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

      AssetStorage::AssetIndex assetIndex ={}; // AssetIndex::loadIndexById("");
      Ptr<AssetStorage> assetStorage = MakeSharedPointerType<AssetStorage>();
      m_assetStorage->readIndex(assetIndex);
      m_assetStorage = assetStorage;

      Ptr<GFXAPIResourceBackend>     resourceBackend     = MakeSharedPointerType<GFXAPIResourceBackend>();
      Ptr<GFXAPIResourceTaskBackend> resourceTaskBackend = nullptr;

      m_proxyFactory = MakeSharedPointerType<ResourceProxyFactory>(resourceBackend);

      Ptr<ResourceManager> manager = MakeSharedPointerType<ResourceManager>(m_proxyFactory);
      manager->setResourceBackend(resourceBackend);
      m_resourceManager = manager;

      if(gfxApi == EGFXAPI::Vulkan) {
        Ptr<VulkanResourceTaskBackend> vkResourceTaskBackend = MakeSharedPointerType<VulkanResourceTaskBackend>(m_vulkanEnvironment);
        vkResourceTaskBackend->initialize();

        resourceTaskBackend = vkResourceTaskBackend;
      }

      resourceBackend->setResourceTaskBackend(resourceTaskBackend);
      resourceBackend->initialize();
    };

    std::function<void()> fnCreatePlatformRenderer
      = [&, this] () -> void
    {
      using Engine::FrameGraph::IFrameGraphRenderContext;
      using Engine::FrameGraph::FrameGraphRenderContext;

      // How to decouple?
      Ptr<IRenderContext> gfxApiRenderContext = nullptr; 
      if(gfxApi == EGFXAPI::Vulkan)
        gfxApiRenderContext = MakeSharedPointerType<VulkanRenderContext>();

      Ptr<IFrameGraphRenderContext> frameGraphRenderContext = FrameGraphRenderContext::create(m_assetStorage, m_resourceManager, gfxApiRenderContext);

      m_renderer = MakeSharedPointerType<Renderer>();
      status = m_renderer->initialize(m_environment, rendererConfiguration, frameGraphRenderContext);
      if(!CheckEngineError(status)) {
        status = m_scene.initialize();
      }
    };

    try {
      fnCreatePlatformWindowSystem();
      fnCreateDefaultGFXAPI();
      fnCreatePlatformResourceSystem();
      fnCreatePlatformRenderer();

    }
    catch(WindowsException const we) {
      Log::Error(logTag(), we.message());
      return we.engineStatus();
    }
    catch(EngineException const e) {
      Log::Error(logTag(), e.message());
      return e.status();
    }
    catch(std::exception const stde) {
      Log::Error(logTag(), stde.what());
      return EEngineStatus::Error;
    }
    catch(...) {
      Log::Error(logTag(), "Unknown error occurred.");
      return EEngineStatus::Error;
    }

    return status;
  }

  EEngineStatus EngineInstance::deinitialize() {
    EEngineStatus status = EEngineStatus::Ok;

    if(m_resourceManager) {
      m_resourceManager->clear(); // Will implicitely clear all subsystems!
      m_resourceManager = nullptr;
    }

    if(m_proxyFactory) {
      m_proxyFactory = nullptr;
    }

    if(m_renderer) {
      status = m_renderer->deinitialize();
    }

    if(m_mainWindow) {
      m_mainWindow->hide();
      m_mainWindow->pause();
      // TODO: Handle errors
      m_mainWindow = nullptr;
    }

    m_windowManager = nullptr;

    return EEngineStatus::Ok;
  }

  EEngineStatus EngineInstance::update() {
    if(CheckWindowManagerError(m_windowManager->update())) {
      Log::Error(logTag(), "Failed to update window manager.");
      return EEngineStatus::EngineComponentUpdateError;
    }

    m_scene.update();

    m_renderer->renderScene();

    return EEngineStatus::Ok;
  }
}
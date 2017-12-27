#include "Core/Engine.h"

#include "Resources/System/Core/ProxyBasedResourceManager.h"

#include "GFXAPI/DirectX/DX11/DX11Renderer.h"
#include "GFXAPI/DirectX/DX11/DX11ResourceTaskBuilder.h"

namespace Engine {

	class TestDummy 
	    : public IWindow::IEventCallback {
	public:
		DeclareLogTag("TestDummy")

		void onResume(const IWindowPtr&) {
			//Log::Status(logTag(), "OnResume");
		}

		void onShow(const IWindowPtr&) {
			//Log::Status(logTag(), "onShow");
		}

		void onBoundsChanged(const IWindowPtr&,
							 const Rect& r) {
			//Log::Status(logTag(), String::format("onBoundsChanged: %0/%1/%2/%3", r._position.x(), r._position.y(), r._size.x(), r._size.y()));
		}

		void onHide(const IWindowPtr&) {
			//Log::Status(logTag(), "onHide");
		}

		void onPause(const IWindowPtr&) {
			//Log::Status(logTag(), "onPause");
		}

		void onClose(const IWindowPtr&) {
			//Log::Status(logTag(), "onClose");
			PostQuitMessage(0);
		}

	};

	EngineInstance::EngineInstance(const Platform::ApplicationEnvironment& environment)
		: _environment(environment)
		, _windowManager(nullptr)   // Do not initialize here, to avoid exceptions in constructor. Memory leaks!!!
	  , _mainWindow(nullptr)
    , _dx11Environment()
  {
	}

	EngineInstance::~EngineInstance() {
		// Fool-Proof redundant check
		if (_windowManager)
			_windowManager = nullptr;
	}

	EEngineStatus EngineInstance::initialize() {
		using namespace Engine::DX::_11;


		EEngineStatus status;

    unsigned long windowWidth  = _environment.osDisplays[0]._bounds._size.x;
    unsigned long windowHeight = _environment.osDisplays[0]._bounds._size.y;

    std::function<void()> fnCreatePlatformWindowSystem
      = [&, this] () -> void
    {
      EEngineStatus status = EEngineStatus::Ok;

      _windowManager = MakeSharedPointerType<WindowManager>();
      if(!(_windowManager && !CheckWindowManagerError(_windowManager->initialize(_environment)))) {
        status = EEngineStatus::EngineComponentInitializationError;
        HandleEngineStatusError(status, "Failed to create WindowManager.");
      }

      _mainWindow = _windowManager->createWindow("MainWindow", Rect(0, 0, windowWidth, windowHeight));
      if(!_mainWindow) {
        status = EEngineStatus::WindowCreationError;
        HandleEngineStatusError(status, "Failed to create main window in WindowManager.");
      }
      else {
        _environment.primaryWindowHandle = _mainWindow->handle();

        status = _mainWindow->resume();
        HandleEngineStatusError(status, "Failed to resume operation in main window.");

        if(!CheckEngineError(status)) {
          status = _mainWindow->show();
          HandleEngineStatusError(status, "Failed to show main window.");
        }
      }

      IWindow::IEventCallbackPtr dummy = MakeSharedPointerType<TestDummy>();
      _mainWindow->registerCallback(dummy);
    };

    RendererConfiguration rendererConfiguration;
    rendererConfiguration.enableVSync             = true;
    rendererConfiguration.frustum                 = Vec4Dd(windowWidth, windowHeight, 0.1f, 1000.0f);
    rendererConfiguration.preferredBackBufferSize = Vec2Dl(windowWidth, windowHeight);
    rendererConfiguration.preferredWindowSize     = rendererConfiguration.preferredBackBufferSize;
    rendererConfiguration.requestFullscreen       = false;

    std::function<void()> fnCreateDefaultGFXAPI
      = [this, &rendererConfiguration] () -> void
    {
      EEngineStatus status = _dx11Environment.initialize(_environment, rendererConfiguration);
      HandleEngineStatusError(status, "DirectX11 initialization failed.");
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
      EGFXAPI        gfxApi        = EGFXAPI::DirectX;
      EGFXAPIVersion gfxApiVersion = EGFXAPIVersion::DirectX_11_0;

      Ptr<BasicGFXAPIResourceBackend>              resourceBackend     = MakeSharedPointerType<BasicGFXAPIResourceBackend>();
      Ptr<IGFXAPIResourceTaskBackend<EngineTypes>> resourceTaskBackend = nullptr;

      if(gfxApi == EGFXAPI::DirectX && gfxApiVersion == EGFXAPIVersion::DirectX_11_0)
            resourceTaskBackend = MakeSharedPointerType<DX11ResourceTaskBuilder>();

      resourceBackend->setResourceTaskBackend(resourceTaskBackend);

      _proxyFactory = MakeSharedPointerType<ResourceProxyFactory>(resourceBackend);

      Ptr<ProxyBasedResourceManager> manager = MakeSharedPointerType<ProxyBasedResourceManager>(_proxyFactory);
      manager->setResourceBackend(resourceBackend);

      _resourceManager = manager;

      // Renderer will have access to resourceBackend!
    };

    std::function<void()> fnCreatePlatformRenderer 
      = [&, this] () -> void
    {
      _renderer = MakeSharedPointerType<DX11Renderer>();
      status = _renderer->initialize(_environment, rendererConfiguration, nullptr);
      if(!CheckEngineError(status)) {
        status = _scene.initialize();
      }
    };

    try {
      fnCreatePlatformWindowSystem();
      fnCreateDefaultGFXAPI();
      fnCreatePlatformResourceSystem();
      fnCreatePlatformRenderer();

    } catch(WindowsException const we) {
      Log::Error(logTag(), we.message());
      return we.engineStatus();
    } catch(EngineException const e) {
      Log::Error(logTag(), e.message());
      return e.status();
    } catch(std::exception const stde) {
      Log::Error(logTag(), stde.what());
      return EEngineStatus::Error;
    } catch(...) {
      Log::Error(logTag(), "Unknown error occurred.");
      return EEngineStatus::Error;
    }

		return status;
	}

	EEngineStatus EngineInstance::deinitialize() {
		EEngineStatus status = EEngineStatus::Ok;

		if( _resourceManager ) {
			_resourceManager->clear(); // Will implicitely clear all subsystems!
			_resourceManager = nullptr;
		}

		if( _proxyFactory ) {
			_proxyFactory = nullptr;
		}

		if (_renderer) {
			status = _renderer->deinitialize();
		}

		if (_mainWindow) {
			_mainWindow->hide();
			_mainWindow->pause();
			// TODO: Handle errors
			_mainWindow = nullptr;
		}

		_windowManager = nullptr;

		return EEngineStatus::Ok;
	}

	EEngineStatus EngineInstance::update() {

		if (CheckWindowManagerError(_windowManager->update())) {
			Log::Error(logTag(), "Failed to update window manager.");
			return EEngineStatus::EngineComponentUpdateError;
		}

		_scene.update();
		_renderer->render();

		return EEngineStatus::Ok;
	}
}
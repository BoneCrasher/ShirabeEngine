#include "Core/Engine.h"

#include "GAPI/DirectX/DX11/DX11Renderer.h"

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
		: _environment(environment),
		  _windowManager(nullptr) // Do not initialize here, to avoid exceptions in constructor. Memory leaks!!!
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

		unsigned long windowWidth  = 1920;
		unsigned long windowHeight = 1080;

		_windowManager = MakeSharedPointerType<WindowManager>();
		if (!(_windowManager && !CheckWindowManagerError(_windowManager->initialize(_environment)))) {
			// Log::Error(logTag(), "Failed to create window manager.");
			return EEngineStatus::EngineComponentInitializationError;
		}

		_mainWindow = _windowManager->createWindow("MainWindow", Rect(0, 0, windowWidth, windowHeight));
		if (!_mainWindow) {
			status = EEngineStatus::WindowCreationError;
		}
		else {
			_environment._primaryWindowHandle = _mainWindow->handle();

			status = _mainWindow->resume();
			if (!CheckEngineError(status))
				status = _mainWindow->show();
		}

		IWindow::IEventCallbackPtr dummy = MakeSharedPointerType<TestDummy>();
		_mainWindow->registerCallback(dummy);

		RendererConfiguration rendererConfiguration;
		rendererConfiguration._enableVSync             = true;
		rendererConfiguration._frustum                 = Vec4Dd(windowWidth, windowHeight, 0.1f, 1000.0f);
		rendererConfiguration._preferredBackBufferSize = Vec2Dl(windowWidth, windowHeight);
		rendererConfiguration._preferredWindowSize     = rendererConfiguration._preferredBackBufferSize;
		rendererConfiguration._requestFullscreen       = false;

		_renderer = MakeSharedPointerType<DX11Renderer>();
		status = _renderer->initialize(_environment, rendererConfiguration, nullptr);
		if (!CheckEngineError(status)) {
			status = _scene.initialize();
		}

		return status;
	}

	EEngineStatus EngineInstance::deinitialize() {
		EEngineStatus status = EEngineStatus::Ok;
		
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
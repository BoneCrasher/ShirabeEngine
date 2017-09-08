#include "Window/WindowManager.h"

#include <algorithm>
#include <functional>

#include "Platform/Platform.h"

#ifdef PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindowFactory.h"
#endif // PLATFORM_WINDOWS

namespace Engine {

	WindowManager::WindowManager()
		: _windows(),
		  _windowFactory(nullptr)
	{
		
	}

	WindowManager::~WindowManager() {
		if (!_windows.empty()) {
			for (const IWindowPtr& pWindow : _windows) {
				Log::Warning(logTag(), String::format("Pending, non-finalized window instance found with name '%0'", pWindow->name()));
			}
			_windows.clear();
		}
	}

	WindowManager::EWindowManagerError WindowManager::initialize(
		const Platform::ApplicationEnvironment& environment
	) {
#ifdef PLATFORM_WINDOWS
		_windowFactory = MakeSharedPointerType<Platform::Windows::WindowsWindowFactory>(environment._instanceHandle);
		// TODO: Create conditional "ApplicationEnvironment"-struct, which contains HINSTANCE as a member in csase  of MSC_VER
#endif // PLATFORM_WINDOWS

		if (!_windowFactory) {
			Log::Error(logTag(), "Failed to initialize the window factory.");
			return EWindowManagerError::InitializationFailed;
		}

		return EWindowManagerError::Ok;
	}

	WindowManager::EWindowManagerError WindowManager::deinitialize() {

		_windowFactory = nullptr;
		// if (!<condition>) {
		// 	Log::Error(logTag(), "Failed to initialize the window factory.");
		// 	return EWindowManagerError::InitializationFailed;
		// }

		return EWindowManagerError::Ok;

	}

	WindowManager::EWindowManagerError WindowManager::update() {
		EWindowManagerError error = EWindowManagerError::Ok;

		for (IWindowPtr pWindow : _windows)
			if (CheckEngineError(pWindow->update())) {
				Log::Warning(logTag(), String::format("Window '%0' failed to update.", pWindow->name()));
				error = EWindowManagerError::UpdateFailed;
				continue;
			}

		return error;
	}

	IWindowPtr WindowManager::createWindow(
		const std::string &name,
		const Rect        &initialBounds) 
	{
		IWindowPtr pNewWindow = _windowFactory->createWindow(name, initialBounds);
		if (!pNewWindow) {
			Log::Warning(logTag(), String::format("Failed to create window '%0' with bounds x/y/w/h --> %1/%2/%3/%4",
												  name.c_str(),
												  initialBounds._position.x(), initialBounds._position.y(),
												  initialBounds._size.x(), initialBounds._size.y())
			);
			return nullptr;
		}

		_windows.push_back(pNewWindow);

		return pNewWindow;
	}

	IWindowPtr WindowManager::getWindowByName(const std::string& name) {
		for (const IWindowPtr& pWindow : _windows) {
			if (pWindow->name().compare(name) == 0)
				return pWindow;
		}
		return nullptr;
	}

	IWindowPtr WindowManager::getWindowByHandle(
		const Platform::Window::WindowHandleWrapper::Handle& handle
	) {
		std::function<bool(const IWindowPtr&)> pred
			= [&](const IWindowPtr& cmp) -> bool { return cmp->handle() == handle; };

		IWindowList::iterator it 
			= std::find_if(_windows.begin(), _windows.end(), pred);

		return (it == _windows.end()) ? nullptr : (*it);

		// One line version:
		// return ((it = std::find_if(_windows.begin(), _windows.end(), [&](const IWindowPtr& cmp) -> bool { return cmp->handle() == handle; })) == _windows.end) ? nullptr : (*it);

		// Below code can also be used. But above is a educational demonstration of std::find_if.
		// for (const IWindowPtr& pWindow : _windows) {
		// 	if (pWindow->handle() == handle)
		// 		return pWindow;
		// }
		// return nullptr;
	}
}

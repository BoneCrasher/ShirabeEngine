#ifndef __SHIRABE_WINDOWMANAGER_H__
#define __SHIRABE_WINDOWMANAGER_H__

#include "Platform/ApplicationEnvironment.h"

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"

#include "Window/IWindowFactory.h" // Includes IWindow.h

namespace Engine {
	class WindowManager {
	public:
		enum class EWindowManagerError
		    : int8_t {
			Ok                     =    0,
			InitializationFailed   = -100,
			UpdateFailed           = -101,
			DeinitializationFailed = -102
		};

		WindowManager();
		~WindowManager();

		EWindowManagerError initialize(const Platform::ApplicationEnvironment& environment);
		EWindowManagerError deinitialize();

		EWindowManagerError update();

		IWindowPtr createWindow(const std::string& name, const Rect& initialBounds);

		IWindowPtr getWindowByName(const std::string& name);
		IWindowPtr getWindowByHandle(const Platform::Window::WindowHandleWrapper::Handle& handle);

	private:
		DeclareLogTag(WindowManager)

		IWindowList       m_windows;
		IWindowFactoryPtr m_windowFactory;
	};
	DeclareSharedPointerType(WindowManager)

#define CheckWindowManagerError(status) \
            (static_cast<std::underlying_type_t<WindowManager::EWindowManagerError>>(status) < 0)
}

#endif

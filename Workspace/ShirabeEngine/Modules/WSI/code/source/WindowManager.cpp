#include "WSI/WindowManager.h"

#include <algorithm>
#include <functional>

#include "Platform/Platform.h"

// #ifdef PLATFORM_WINDOWS
//     #include "Platform/Windows/WindowsWindowFactory.h"
// #endif // PLATFORM_WINDOWS

namespace Engine {
  namespace WSI {

    WindowManager::WindowManager()
      : m_windows(),
      m_windowFactory(nullptr)
    {

    }

    WindowManager::~WindowManager() {
      if(!m_windows.empty()) {
        for(const IWindowPtr& pWindow : m_windows) {
          Log::Warning(logTag(), String::format("Pending, non-finalized window instance found with name '%0'", pWindow->name()));
        }
        m_windows.clear();
      }
    }

    WindowManager::EWindowManagerError WindowManager::initialize(
      OS::ApplicationEnvironment const&environment)
    {
      //#ifdef PLATFORM_WINDOWS
      //		m_windowFactory = MakeSharedPointerType<Platform::Windows::WindowsWindowFactory>(environment.instanceHandle);
      //		// TODO: Create conditional "ApplicationEnvironment"-struct, which contains HINSTANCE as a member in csase  of MSC_VER
      //#endif // PLATFORM_WINDOWS

      if(!m_windowFactory) {
        Log::Error(logTag(), "Failed to initialize the window factory.");
        return EWindowManagerError::InitializationFailed;
      }

      return EWindowManagerError::Ok;
    }

    WindowManager::EWindowManagerError
      WindowManager::deinitialize()
    {
      m_windowFactory = nullptr;
      // if (!<condition>) {
      // 	Log::Error(logTag(), "Failed to initialize the window factory.");
      // 	return EWindowManagerError::InitializationFailed;
      // }

      return EWindowManagerError::Ok;
    }

    WindowManager::EWindowManagerError WindowManager::update() {
      EWindowManagerError error = EWindowManagerError::Ok;

      for(IWindowPtr pWindow : m_windows)
        if(CheckEngineError(pWindow->update())) {
          Log::Warning(logTag(), String::format("Window '%0' failed to update.", pWindow->name()));
          error = EWindowManagerError::UpdateFailed;
          continue;
        }

      return error;
    }

    Ptr<IWindow> WindowManager::createWindow(
      std::string const&name,
      Rect        const&initialBounds)
    {
      IWindowPtr pNewWindow = m_windowFactory->createWindow(name, initialBounds);
      if(!pNewWindow) {
        Log::Warning(logTag(), String::format("Failed to create window '%0' with bounds x/y/w/h --> %1/%2/%3/%4",
          name.c_str(),
          initialBounds.position.x(), initialBounds.position.y(),
          initialBounds.size.x(), initialBounds.size.y())
        );
        return nullptr;
      }

      m_windows.push_back(pNewWindow);

      return pNewWindow;
    }

    Ptr<IWindow> WindowManager::getWindowByName(std::string const&name) {
      for(const IWindowPtr& pWindow : m_windows) {
        if(pWindow->name().compare(name) == 0)
          return pWindow;
      }
      return nullptr;
    }

    Ptr<IWindow> WindowManager::getWindowByHandle(
      OS::WindowHandleWrapper::Handle const&handle
    ) {
      std::function<bool(const IWindowPtr&)> pred
        = [&] (const IWindowPtr& cmp) -> bool { return cmp->handle() == handle; };

      IWindowList::iterator it
        = std::find_if(m_windows.begin(), m_windows.end(), pred);

      return (it == m_windows.end()) ? nullptr : (*it);

      // One line version:
      // return ((it = std::find_if(_windows.begin(), m_windows.end(), [&](const IWindowPtr& cmp) -> bool { return cmp->handle() == handle; })) == m_windows.end) ? nullptr : (*it);

      // Below code can also be used. But above is a educational demonstration of std::find_if.
      // for (const IWindowPtr& pWindow : m_windows) {
      // 	if (pWindow->handle() == handle)
      // 		return pWindow;
      // }
      // return nullptr;
    }
  }
}

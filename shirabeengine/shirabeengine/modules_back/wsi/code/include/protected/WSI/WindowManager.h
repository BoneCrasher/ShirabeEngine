#ifndef __SHIRABE_WINDOWMANAGER_H__
#define __SHIRABE_WINDOWMANAGER_H__

#include "OS/ApplicationEnvironment.h"

#include "Core/EngineTypeHelper.h"
#include "Log/Log.h"

#include "WSI/IWindowFactory.h" // Includes IWindow.h

namespace Engine {
  namespace WSI {

    class WindowManager {
      DeclareLogTag(WindowManager);
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

      EWindowManagerError initialize(OS::ApplicationEnvironment const&environment);
      EWindowManagerError deinitialize();

      EWindowManagerError update();

      CStdSharedPtr_t<IWindow> createWindow(std::string const&name, Rect const&initialBounds);

      CStdSharedPtr_t<IWindow> getWindowByName(std::string const&name);
      CStdSharedPtr_t<IWindow> getWindowByHandle(OS::WindowHandleWrapper::Handle const&handle);

    private:
      IWindowList         m_windows;
      CStdSharedPtr_t<IWindowFactory> m_windowFactory;
    };
    DeclareSharedPointerType(WindowManager)

      #define CheckWindowManagerError(status) \
            (static_cast<std::underlying_type_t<WindowManager::EWindowManagerError>>(status) < 0)
  }
}

#endif

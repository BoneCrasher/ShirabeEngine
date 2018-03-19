#include "Platform/Windows/WindowsWindowFactory.h"

#include "Core/String.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Platform {
  namespace Windows {

    namespace WinAPIFunctions {

      template <typename TWindowType>
      HWND __CreateWindow(
        HINSTANCE               instanceHandle,
        const WindowsWindowPtr &engineWindow,
        const std::string      &name,
        const Rect             &initialBounds);
    }

    WindowsWindowFactory::WindowsWindowFactory(HINSTANCE instanceHandle)
      : m_instanceHandle(instanceHandle) {}

    WindowsWindowFactory::~WindowsWindowFactory() {}

    IWindowPtr WindowsWindowFactory::createWindow(
      const std::string &name,
      const Rect        &initialBounds) {

      IWindowPtr pWindow = MakeSharedPointerType<WindowsWindow>(name, initialBounds);

      HWND handle
        = WinAPIFunctions::__CreateWindow<WindowsWindow>(
          m_instanceHandle,
          std::static_pointer_cast<WindowsWindow>(pWindow),
          name,
          initialBounds);
      if(handle) {
        //
      }

      // Any further processing

      return pWindow;
    }


    namespace WinAPIFunctions {

      DeclareLogTag(WinAPIFunctions)

        template <typename TWindowType>
      HWND __CreateWindow(
        HINSTANCE               instanceHandle,
        const WindowsWindowPtr &engineWindow,
        const std::string      &name,
        const Rect             &initialBounds
        // TODO: Fullscreen
      ) {
        std::wstring className    = String::toWideString(TWindowType::ClassName);
        std::wstring instanceName = String::toWideString(name);

        HWND hwnd;

        WNDCLASSEX wc ={ };

        wc.style         = CS_OWNDC | CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW; // Global window class (available in all modules), redraw on horizonal or vertical movement.
        wc.cbClsExtra    = 0; // No extra bytes to be allocated after window class.
        wc.cbWndExtra    = 0; // No extra bytes to be allocated after window instance.
        wc.lpszClassName = className.c_str();
        wc.hInstance     = instanceHandle;
        wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
        wc.lpszMenuName  = NULL;
        wc.lpfnWndProc   = &TWindowType::WndProc;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wc.cbSize        = sizeof(WNDCLASSEX);

        DWORD registrationResult = RegisterClassEx(&wc);
        if(!registrationResult) {
          Log::Error(logTag(), String::format("Failed to register windows window class. WINAPI error code: %0", GetLastError()));
          return 0;
        }

        hwnd
          = CreateWindowEx(
            0,
            wc.lpszClassName,
            instanceName.c_str(),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            initialBounds.m_position.x(),  // Bounds::Location::X
            initialBounds.m_position.y(),  // Bounds::Location::Y
            initialBounds.size.x(),      // Bounds::Size::X
            initialBounds.size.y(),      // Bounds::Size::Y
            NULL,                        // Parent HWND
            NULL,                        // Menu
            instanceHandle,              // App-Instance
            static_cast<IWinAPIAdapter*>(engineWindow.get())     // Additional l-param
          );

        if(!hwnd) {
          DWORD err = GetLastError();
          Log::Error(logTag(), String::format("Failed to create window. WINAPI error code: %0", err));
          return 0;
        }

        return hwnd;
      }
    }
  }
}
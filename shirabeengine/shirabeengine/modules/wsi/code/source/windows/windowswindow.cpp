#include "Log/Log.h"
#include "WSI/Windows/WindowsWindow.h"

namespace engine {
  namespace wsi {
    namespace Windows {

      WindowsWindow::WindowsWindow(
        const std::string &name,
        const Rect        &initialBounds
      ): IWindow()
        , IWinAPIAdapter()
        , m_name(name)
        , m_bounds(initialBounds)
        , m_handleWrapper(0)
        , m_active(false)
      {
      }

      WindowsWindow::~WindowsWindow() {
        if(m_active.load()) {
          m_active.store(false);
        }
      }
      
      OS::WindowHandleWrapper::Handle const& WindowsWindow::handle() const { return m_handleWrapper.handle(); }

      // Properties
      std::string const& WindowsWindow::name()   const { return m_name; }
      Rect        const& WindowsWindow::bounds() const { return m_bounds; }

      EEngineStatus WindowsWindow::show() {
        try {
          ShowWindow((HWND)handle(), SW_SHOW);
          SetForegroundWindow((HWND)handle());
          SetFocus((HWND)handle());

          return EEngineStatus::Ok;
        }
        catch(...) {
          Log::Error(logTag(), String::format("Failed to show window '%0'", name()));
          return EEngineStatus::WindowEventError;
        }
      }

      EEngineStatus WindowsWindow::hide() {
        try {
          ShowWindow((HWND)handle(), SW_HIDE);

          return EEngineStatus::Ok;
        }
        catch(...) {
          Log::Error(logTag(), String::format("Failed to hide window '%0'", name()));
          return EEngineStatus::WindowEventError;
        }
      }

      EEngineStatus WindowsWindow::resume() {
        try {
          m_active.store(true);

          m_callbackAdapter.onResume(makeCStdSharedFromThis(this));
          return EEngineStatus::Ok;
        }
        catch(...) {
          Log::Error(logTag(), String::format("Failed to show window '%0'", name()));
          return EEngineStatus::WindowEventError;
        }
      }

      EEngineStatus WindowsWindow::update() {
        if(!m_active.load())
          return EEngineStatus::Ok;

        MSG winAPIMessage;

        try {
          if(PeekMessage(&winAPIMessage, (HWND)m_handleWrapper.handle(), 0, 0, PM_REMOVE)) {
            TranslateMessage(&winAPIMessage);
            DispatchMessage(&winAPIMessage);
          }

          return EEngineStatus::Ok;
        }
        catch(...) {
          Log::Error(logTag(), String::format("Failed to update message queue of window '%0'", name()));
          return EEngineStatus::WindowMessageHandlerError;
        }

      }

      EEngineStatus WindowsWindow::pause() {
        try {
          m_active.store(false);

          m_callbackAdapter.onPause(makeCStdSharedFromThis(this));
          return EEngineStatus::Ok;
        }
        catch(...) {
          Log::Error(logTag(), String::format("Failed to show window '%0'", name()));
          return EEngineStatus::WindowEventError;
        }
      }
      
      EEngineStatus WindowsWindow::registerCallback(const IWindow::IEventCallbackPtr& cb) {
        return m_callbackAdapter.registerCallback(cb);
      }
      EEngineStatus WindowsWindow::unregisterCallback(const IWindow::IEventCallbackPtr& cb) {
        return m_callbackAdapter.unregisterCallback(cb);
      }

      //
      // IWinAPIAdapter implementation
      //
      void WindowsWindow::onCreate(const HWND& handle) {
        m_handleWrapper = WindowHandleWrapper((OSHandle)handle);

      }

      void WindowsWindow::onEnabled() {

      }

      void WindowsWindow::onShow() {
        m_callbackAdapter.onShow(makeCStdSharedFromThis(this));
      }

      void WindowsWindow::onHide() {
        m_callbackAdapter.onHide(makeCStdSharedFromThis(this));
      }

      void WindowsWindow::onDisabled() {

      }

      void WindowsWindow::onClose() {
        m_callbackAdapter.onClose(makeCStdSharedFromThis(this));
      }


      void WindowsWindow::onMove(
        const long& x,
        const long& y) {
        m_bounds.position.x(x);
        m_bounds.position.y(y);

        m_callbackAdapter.onBoundsChanged(makeCStdSharedFromThis(this), m_bounds);
      }
      void WindowsWindow::onResize(const long& width,
        const long& height) {
        m_bounds.size.x(width);
        m_bounds.size.y(height);

        m_callbackAdapter.onBoundsChanged(makeCStdSharedFromThis(this), m_bounds);
      }

      //
      // Window procedure... 
      //
      LRESULT CALLBACK WindowsWindow::WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
        //
        // Store and retrieve the attached engine window WinAPI-adapter instance.
        //
        IWinAPIAdapter *adapter = nullptr;
        if(message == WM_NCCREATE) {
          LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
          adapter = reinterpret_cast<IWinAPIAdapter *>(lpcs->lpCreateParams);
          SetWindowLongPtr(handle,
            GWLP_USERDATA,
            reinterpret_cast<LPARAM>(adapter));
        }
        else {
          adapter = reinterpret_cast<IWinAPIAdapter *>(GetWindowLongPtr(handle, GWLP_USERDATA));
        }

        switch(message) {
        case WM_CREATE:
        {
          adapter->onCreate(handle);
          break;
        }
        case WM_ENABLE:
        {
          bool enabled = (bool)wparam;
          if(enabled)
            adapter->onEnabled();
          else
            adapter->onDisabled();

          break;
        }
        case WM_MOVING:
        {
          /*RECT *currentWindowRect = ((RECT *)lparam);

          adapter->onMove(currentWindowRect->left,
                  currentWindowRect->top);*/

          break;
        }
        case WM_MOVE:
        {
          // Sounds good, doesn't work... LOWORD and HIWORD return DWORD (=unsigned) 
          // long x = LOWORD(lparam);
          // long y = HIWORD(lparam);

          RECT currentWindowRect;
          GetWindowRect(handle, &currentWindowRect);

          adapter->onMove(currentWindowRect.left,
            currentWindowRect.top);

          break;
        }
        case WM_SIZING:
        {
          /* RECT *currentWindowRect = ((RECT *)lparam);

          adapter->onResize(
            (currentWindowRect->right - currentWindowRect->left),
            (currentWindowRect->bottom - currentWindowRect->top));*/

          break;
        }
        case WM_SIZE:
        {
          // Sounds good, doesn't work... LOWORD and HIWORD return DWORD (=unsigned)
          // long width  = LOWORD(lparam);
          // long height = HIWORD(lparam);

          RECT currentWindowRect;
          GetWindowRect(handle, &currentWindowRect);

          adapter->onResize(
            (currentWindowRect.right  - currentWindowRect.left),
            (currentWindowRect.bottom - currentWindowRect.top));

          break;
        }
        case WM_CLOSE:
        {
          adapter->onClose();
        }
        // DO NOT INCLUDE ANYTHING HERE to have the onClose-fallthrough work.
        // All other messages pass to the message handler in the system class.
        default:
          // Log::Verbose(logTag(), String::format("Unhandled message '%0'. Fwd to default handler.", message));
          goto unhandled;
          break;
        }

        return 0;

unhandled:
        return DefWindowProc(handle, message, wparam, lparam);
      }
    }
  }
}
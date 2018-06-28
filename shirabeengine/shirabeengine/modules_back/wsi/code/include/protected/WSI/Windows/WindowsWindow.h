#ifndef __SHIRABE_WSI_WINDOWSWINDOW_H__
#define __SHIRABE_WSI_WINDOWSWINDOW_H__

#include <atomic>

#include "Log/Log.h"

#include "OS/WindowHandleWrapper.h"
#include "WSI/IWindow.h"
#include "WSI/IWindowEventCallbackAdapter.h"

namespace Engine {
  namespace WSI {
    namespace Windows {
      using namespace Engine;
      using namespace OS;

      class IWinAPIAdapter {
      public:
        virtual ~IWinAPIAdapter() = default;

        IWinAPIAdapter(const IWinAPIAdapter&)             = delete;
        IWinAPIAdapter(IWinAPIAdapter&&)                  = delete;
        IWinAPIAdapter& operator =(const IWinAPIAdapter&) = delete;
        IWinAPIAdapter& operator =(IWinAPIAdapter&&)      = delete;

        //
        // API
        //

        // Lifecycle
        virtual void onCreate(const HWND&) = 0;
        virtual void onEnabled()           = 0;
        virtual void onShow()              = 0;
        virtual void onHide()              = 0;
        virtual void onDisabled()          = 0;
        virtual void onClose()             = 0;

        virtual void onMove(const long& x,
          const long& y) = 0;
        virtual void onResize(const long& width,
          const long& height) = 0;


      protected:
        IWinAPIAdapter() = default;
      };
      DeclareSharedPointerType(IWinAPIAdapter)

        class SHIRABE_LIBRARY_EXPORT WindowsWindow
        : public WSI::IWindow
        , public IWinAPIAdapter
      {
      public:
        constexpr static const char* ClassName = "WindowsWindow";

        //
        // Windows CALLBACK handler for windows events. Forward decl. to not pollute top entries.
        //
        static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

        WindowsWindow(
          const std::string                 &name,
          const Rect                        &initialBounds
        );
        ~WindowsWindow();

        //
        // IWindow implementation
        //

        // Platform
        OS::WindowHandleWrapper::Handle const& handle() const;

        // Properties
        std::string const&name()   const;
        Rect        const&bounds() const;

        // Lifecycle
        EEngineStatus show();
        EEngineStatus hide();

        EEngineStatus resume();
        EEngineStatus update();
        EEngineStatus pause();

        // Callbacks
        EEngineStatus registerCallback(const IWindow::IEventCallbackPtr& cb);
        EEngineStatus unregisterCallback(const IWindow::IEventCallbackPtr& cb);

        //
        // IWinAPIAdapter implementation
        //
        void onCreate(const HWND&);
        void onEnabled();
        void onShow();
        void onHide();
        void onDisabled();
        void onClose();

        void onMove(const long& x,
          const long& y);
        void onResize(const long& width,
          const long& height);

      private:
        DeclareLogTag(WindowsWindow);

        std::string m_name;
        Rect        m_bounds;

        WindowHandleWrapper        m_handleWrapper;
        WindowEventCallbackAdapter m_callbackAdapter;

        std::atomic_bool m_active;
      };
      DeclareSharedPointerType(WindowsWindow)
    }
  }
}
#endif

#ifndef __SHIRABE_WINDOWSWINDOW_H__
#define __SHIRABE_WINDOWSWINDOW_H__

#include <atomic>

#include "Log/Log.h"

#include "Platform/WindowHandleWrapper.h"
#include "Window/IWindow.h"
#include "Window/IWindowEventCallbackAdapter.h"

namespace Platform {
	namespace Windows {
		using namespace Engine;
		using namespace Platform::Window;

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
			: public Engine::IWindow,
			  public IWinAPIAdapter
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
			inline
				const Platform::Window::WindowHandleWrapper::Handle& handle() const { return m_handleWrapper.handle();  }

			// Properties
			inline const std::string& name()   const { return m_name;   }
			inline const Rect&        bounds() const { return m_bounds; }

			// Lifecycle
			EEngineStatus show();
			EEngineStatus hide();

			EEngineStatus resume();
			EEngineStatus update();
			EEngineStatus pause();

			// Callbacks
			inline EEngineStatus registerCallback(const IWindow::IEventCallbackPtr& cb) {
				return m_callbackAdapter.registerCallback(cb);
			}
			inline EEngineStatus unregisterCallback(const IWindow::IEventCallbackPtr& cb) {
				return m_callbackAdapter.unregisterCallback(cb);
			}

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
			DeclareLogTag(WindowsWindow)

			std::string m_name;
			Rect        m_bounds;

			WindowHandleWrapper        m_handleWrapper;
			WindowEventCallbackAdapter m_callbackAdapter;

			std::atomic_bool m_active;
		};
		DeclareSharedPointerType(WindowsWindow)
	}
}

#endif

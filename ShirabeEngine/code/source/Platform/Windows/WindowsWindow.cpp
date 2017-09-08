#include "Platform/Windows/WindowsWindow.h"
#include "Log/Log.h"

namespace Platform {
	namespace Windows {

		WindowsWindow::WindowsWindow(
			const std::string                 &name,
			const Rect                        &initialBounds
		) : IWindow(),
			IWinAPIAdapter(),
			_name(name),
			_bounds(initialBounds),
			_handleWrapper((HWND) 0),
			_active(false)
		{
		}

		WindowsWindow::~WindowsWindow() {
			if (_active.load()) {
				_active.store(false);
			}
		}
		
		EEngineStatus WindowsWindow::show() {
			try {
				ShowWindow(handle(), SW_SHOW);
				SetForegroundWindow(handle());
				SetFocus(handle());

				return EEngineStatus::Ok;
			} catch (...) {
				Log::Error(logTag(), String::format("Failed to show window '%0'", name()));
				return EEngineStatus::WindowEventError;
			}
		}

		EEngineStatus WindowsWindow::hide() {
			try {
				ShowWindow(handle(), SW_HIDE);

				return EEngineStatus::Ok;
			}
			catch (...) {
				Log::Error(logTag(), String::format("Failed to hide window '%0'", name()));
				return EEngineStatus::WindowEventError;
			}
		}

		EEngineStatus WindowsWindow::resume() {
			try {
				_active.store(true);

				_callbackAdapter.onResume(GetNonDeletingSelfPtrType(this));
				return EEngineStatus::Ok;
			}
			catch (...) {
				Log::Error(logTag(), String::format("Failed to show window '%0'", name()));
				return EEngineStatus::WindowEventError;
			}
		}

		EEngineStatus WindowsWindow::update() {
			if (!_active.load())
				return EEngineStatus::Ok;

			MSG winAPIMessage;

			try {
				if (PeekMessage(&winAPIMessage, _handleWrapper.handle(), 0, 0, PM_REMOVE)) {
					TranslateMessage(&winAPIMessage);
					DispatchMessage(&winAPIMessage);
				}

				return EEngineStatus::Ok;
			}
			catch (...) {
				Log::Error(logTag(), String::format("Failed to update message queue of window '%0'", name()));
				return EEngineStatus::WindowMessageHandlerError;
			}

		}

		EEngineStatus WindowsWindow::pause() {
			try {
				_active.store(false);

				_callbackAdapter.onPause(GetNonDeletingSelfPtrType(this));
				return EEngineStatus::Ok;
			}
			catch (...) {
				Log::Error(logTag(), String::format("Failed to show window '%0'", name()));
				return EEngineStatus::WindowEventError;
			}
		}

		//
		// IWinAPIAdapter implementation
		//
		void WindowsWindow::onCreate(const HWND& handle) {
			_handleWrapper = WindowHandleWrapper(handle);

		}

		void WindowsWindow::onEnabled() {

		}

		void WindowsWindow::onShow() {
			_callbackAdapter.onShow(GetNonDeletingSelfPtrType(this));
		}

		void WindowsWindow::onHide() {
			_callbackAdapter.onHide(GetNonDeletingSelfPtrType(this));
		}

		void WindowsWindow::onDisabled() {

		}

		void WindowsWindow::onClose() {
			_callbackAdapter.onClose(GetNonDeletingSelfPtrType(this));
		}


		void WindowsWindow::onMove(const long& x,
								   const long& y) {
			_bounds._position.x() = x;
			_bounds._position.y() = y;

			_callbackAdapter.onBoundsChanged(GetNonDeletingSelfPtrType(this), _bounds);
		}
		void WindowsWindow::onResize(const long& width,
									 const long& height) {
			_bounds._size.x() = width;
			_bounds._size.y() = height;

			_callbackAdapter.onBoundsChanged(GetNonDeletingSelfPtrType(this), _bounds);
		}
		
		//
		// Window procedure... 
		//
		LRESULT CALLBACK WindowsWindow::WndProc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam) {
			//
			// Store and retrieve the attached engine window WinAPI-adapter instance.
			//
			IWinAPIAdapter *adapter = nullptr;
			if (message == WM_NCCREATE) {
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
				adapter = reinterpret_cast<IWinAPIAdapter *>(lpcs->lpCreateParams);
				SetWindowLongPtr(handle, 
								 GWLP_USERDATA,
								 reinterpret_cast<LPARAM>(adapter));
			} else {
				adapter = reinterpret_cast<IWinAPIAdapter *>(GetWindowLongPtr(handle, GWLP_USERDATA));
			}

			switch (message) {
			case WM_CREATE:
			{
				adapter->onCreate(handle);
				break;
			}
			case WM_ENABLE:
			{
				bool enabled = (bool)wparam;
				if (enabled)
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
				Log::Verbose(logTag(), String::format("Unhandled message '%0'. Fwd to default handler.", message));
				goto unhandled;
				break;
			}

			return 0;

			unhandled:
				return DefWindowProc(handle, message, wparam, lparam);
		}
	}
}
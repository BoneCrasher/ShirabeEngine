#ifndef __SHIRABE_IWINDOW_H__
#define __SHIRABE_IWINDOW_H__

#include "Compiler/MSVCWarnings.h"

#include <string>

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Core/BasicTypes.h"

#include "Platform/WindowHandleWrapper.h"

namespace Engine {
	
	class IWindow;
	DeclareSharedPointerType(IWindow);

	DeclareInterface(IWindow);
		DeclareInterface(IEventCallback);
			//
			// API
			// 
			virtual void onResume(const IWindowPtr&)        = 0;
			virtual void onShow(const IWindowPtr&)          = 0;
			virtual void onBoundsChanged(const IWindowPtr&,
										 const Rect&)       = 0;
			virtual void onHide(const IWindowPtr&)          = 0;
			virtual void onPause(const IWindowPtr&)         = 0;
			virtual void onClose(const IWindowPtr&)         = 0;

		DeclareInterfaceEnd(IEventCallback);
		DeclareSharedPointerType(IEventCallback);        // IEventCallbackPtr
		DeclareListType(IEventCallbackPtr, IEventCallback); // IEventCallbackList


		// Platform
		virtual const Platform::Window::WindowHandleWrapper::Handle& handle() const = 0;

		// Properties
		virtual const std::string& name()   const = 0;
		virtual const Rect&        bounds() const = 0;

		// Lifecycle
		virtual EEngineStatus show() = 0;
		virtual EEngineStatus hide() = 0;

		virtual EEngineStatus resume() = 0;
		virtual EEngineStatus update() = 0;
		virtual EEngineStatus pause()  = 0;

		// Callbacks
		virtual EEngineStatus registerCallback(const IWindow::IEventCallbackPtr& cb)   = 0;
		virtual EEngineStatus unregisterCallback(const IWindow::IEventCallbackPtr& cb) = 0;

		DeclareInterfaceEnd(IWindow);
	// DeclareSharedPointerType(IWindow)    // Declare IWindowPtr 
    DeclareListType(IWindowPtr, IWindow) // Declare IWindowList
}

#endif
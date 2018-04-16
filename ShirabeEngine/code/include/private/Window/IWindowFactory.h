#ifndef __SHIRABE_IWINDOWFACTORY_H__
#define __SHIRABE_IWINDOWFACTORY_H__

#include "Compiler/MSVCWarnings.h"

#include <string>

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Window/IWindow.h"

namespace Engine {

	class IWindowFactory {
	public:
		//
		// Facilitate proper destruction
		//
		virtual ~IWindowFactory() = default;

		//
		// Deny Copy & Move
		// 
		IWindowFactory(const IWindowFactory&)             = delete;
		IWindowFactory(IWindowFactory&&)                  = delete;
		IWindowFactory& operator =(const IWindowFactory&) = delete;
		IWindowFactory& operator =(IWindowFactory&)       = delete;

		//
		// Interface API
		//
		virtual IWindowPtr createWindow(const std::string &name, 
	                                    const Rect        &initialBounds) = 0;

	protected:
		// Prevent direct instantiation
		IWindowFactory() = default;
	};
	DeclareSharedPointerType(IWindowFactory) // Declare IWindowFactoryPtr 
}

#endif
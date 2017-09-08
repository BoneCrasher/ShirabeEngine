#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include "Platform/ApplicationEnvironment.h"

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Window/WindowManager.h"

#include "BuildingBlocks/Scene.h"

#include "GAPI/IRenderer.h"

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace Engine {
	using namespace Renderer;

	class EngineInstance
	{
	public:
		EngineInstance(const Platform::ApplicationEnvironment& environment);
		~EngineInstance();

		EEngineStatus initialize();
		EEngineStatus deinitialize();

		EEngineStatus update();

	private:
		DeclareLogTag(EngineInstance)

		Platform::ApplicationEnvironment _environment;
		WindowManagerPtr                 _windowManager;
		IWindowPtr                       _mainWindow;

		IRendererPtr _renderer;
		Scene        _scene;
	};

	DeclareSharedPointerType(EngineInstance)
}

#endif 


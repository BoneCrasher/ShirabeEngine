#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include "Platform/ApplicationEnvironment.h"
#include "Platform/Platform.h"

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Window/WindowManager.h"

#include "Resources/System/Core/IResourceManager.h"
#include "Resources/System/Core/ResourceProxyFactory.h"

#include "BuildingBlocks/Scene.h"

#include "GFXAPI/IRenderer.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsError.h"
#include "GFXAPI/DirectX/DX11/DX11Environment.h"

using namespace Platform::Windows;
using namespace Engine::DX::_11;
#endif

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace Engine {
	using namespace Renderer;
	using namespace Resources;

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

#ifdef PLATFORM_WINDOWS
    DX11Environment _dx11Environment;
#endif


		Ptr<ResourceProxyFactory> _proxyFactory;
		Ptr<IResourceManager>     _resourceManager;

		IRendererPtr _renderer;
		Scene        _scene;
	};

	DeclareSharedPointerType(EngineInstance)
}

#endif 


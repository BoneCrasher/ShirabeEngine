#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include "Platform/ApplicationEnvironment.h"
#include "Platform/Platform.h"

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Window/WindowManager.h"

#include "Resources/Core/IResourceManager.h"
#include "Resources/Core/ResourceProxyFactory.h"

#include "BuildingBlocks/Scene.h"

#include "Renderer/IRenderer.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsError.h"
#include "GFXAPI/DirectX/DX11/Environment.h"

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

		Platform::ApplicationEnvironment m_environment;
		WindowManagerPtr                 m_windowManager;
		IWindowPtr                       m_mainWindow;

#ifdef PLATFORM_WINDOWS
    Ptr<DX11Environment> m_dx11Environment;
#endif

		Ptr<ResourceProxyFactory> m_proxyFactory;
		Ptr<IResourceManager>     m_resourceManager;

		IRendererPtr m_renderer;
		Scene        m_scene;
	};

	DeclareSharedPointerType(EngineInstance)
}

#endif 


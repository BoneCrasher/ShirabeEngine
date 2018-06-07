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
#include "GFXAPI/Vulkan/Environment.h"

using namespace Platform::Windows;
using namespace Engine::Vulkan;
#endif

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace Engine {
	using namespace Renderer;
	using namespace Resources;

	class SHIRABE_LIBRARY_EXPORT EngineInstance
	{
	public:
		EngineInstance(Platform::ApplicationEnvironment const&environment);
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
    Ptr<VulkanEnvironment> m_vulkanEnvironment;
#endif

		Ptr<ResourceProxyFactory> m_proxyFactory;
		Ptr<IResourceManager>     m_resourceManager;

		IRendererPtr m_renderer;
		Scene        m_scene;
	};

	DeclareSharedPointerType(EngineInstance)
}

#endif 


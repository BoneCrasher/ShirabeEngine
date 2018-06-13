#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include "Platform/ApplicationEnvironment.h"
#include "Platform/Platform.h"

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "Window/WindowManager.h"

#include "Asset/AssetStorage.h"
#include "Resources/Core/ResourceProxyFactory.h"

#include "Renderer/IRenderer.h"
#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsError.h"
#include "GFXAPI/Vulkan/Environment.h"

using namespace Platform::Windows;
using namespace Engine::Vulkan;
#endif

#include "BuildingBlocks/Scene.h"

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace Engine {
  using namespace Asset;
	using namespace Rendering;
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

    // Application 
		Platform::ApplicationEnvironment m_environment;
		
    // WSI
    WindowManagerPtr m_windowManager;
		IWindowPtr       m_mainWindow;

    // Assets & Resources
    Ptr<AssetStorage>         m_assetStorage;
    Ptr<ResourceProxyFactory> m_proxyFactory;
    Ptr<ResourceManager>      m_resourceManager;

    // Rendering
#ifdef PLATFORM_WINDOWS
    Ptr<VulkanEnvironment> m_vulkanEnvironment;
#endif
		IRendererPtr m_renderer;
		
    // Internals
    Scene m_scene;
	};

	DeclareSharedPointerType(EngineInstance)
}

#endif 


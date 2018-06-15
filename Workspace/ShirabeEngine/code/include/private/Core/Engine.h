#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include "Platform/Platform.h"
#include "OS/ApplicationEnvironment.h"

#include "Core/EngineStatus.h"
#include "Core/EngineTypeHelper.h"

#include "WSI/WindowManager.h"

#include "Asset/AssetStorage.h"
#include "Resources/Core/ResourceProxyFactory.h"

#include "Renderer/IRenderer.h"
#ifdef PLATFORM_WINDOWS
#include "WSI/Windows/WindowsError.h"
#include "Vulkan/VulkanEnvironment.h"

#endif

#include "BuildingBlocks/Scene.h"

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace Engine {
  using namespace Asset;
	using namespace Rendering;
	using namespace Resources;
  using namespace Engine::OS;
  using namespace Engine::WSI;
  using namespace Engine::Vulkan;

	class SHIRABE_LIBRARY_EXPORT EngineInstance
	{
	public:
		EngineInstance(Ptr<OS::ApplicationEnvironment> const&environment);
		~EngineInstance();

		EEngineStatus initialize();
		EEngineStatus deinitialize();

		EEngineStatus update();

	private:
		DeclareLogTag(EngineInstance)

    // Application 
		Ptr<OS::ApplicationEnvironment> m_environment;
		
    // WSI
    Ptr<WindowManager> m_windowManager;
		Ptr<IWindow>       m_mainWindow;

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


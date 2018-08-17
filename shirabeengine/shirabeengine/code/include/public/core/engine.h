#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include "Platform/Platform.h"
#include "OS/ApplicationEnvironment.h"

#include "core/enginestatus.h"
#include "core/enginetypehelper.h"

#include "WSI/WindowManager.h"

#include "Asset/AssetStorage.h"
#include "Resources/Core/ResourceProxyFactory.h"

#include "Renderer/IRenderer.h"
#ifdef SHIRABE_PLATFORM_WINDOWS
#include "WSI/Windows/WindowsError.h"
#include "Vulkan/VulkanEnvironment.h"

#endif

#include "BuildingBlocks/Scene.h"

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace engine {
  using namespace Asset;
	using namespace Rendering;
	using namespace Resources;
  using namespace engine::OS;
  using namespace engine::WSI;
  using namespace engine::Vulkan;

	class SHIRABE_LIBRARY_EXPORT EngineInstance
	{
	public:
		EngineInstance(CStdSharedPtr_t<OS::ApplicationEnvironment> const&environment);
		~EngineInstance();

		EEngineStatus initialize();
		EEngineStatus deinitialize();

		EEngineStatus update();

	private:
		SHIRABE_DECLARE_LOG_TAG(EngineInstance)

    // Application 
		CStdSharedPtr_t<OS::ApplicationEnvironment> m_environment;
		
    // WSI
    CStdSharedPtr_t<WindowManager> m_windowManager;
		CStdSharedPtr_t<IWindow>       m_mainWindow;

    // Assets & Resources
    CStdSharedPtr_t<AssetStorage>         m_assetStorage;
    CStdSharedPtr_t<ResourceProxyFactory> m_proxyFactory;
    CStdSharedPtr_t<ResourceManager>      m_resourceManager;

    // Rendering
#ifdef SHIRABE_PLATFORM_WINDOWS
    CStdSharedPtr_t<VulkanEnvironment> m_vulkanEnvironment;
#endif
		IRendererPtr m_renderer;
		
    // Internals
    Scene m_scene;
	};

	DeclareSharedPointerType(EngineInstance)
}

#endif 


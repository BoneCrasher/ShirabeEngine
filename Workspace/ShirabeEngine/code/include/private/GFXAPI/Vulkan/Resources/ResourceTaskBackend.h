#ifndef __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__
#define __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__

#include <map>

#include "Resources/Subsystems/GFXAPI/GFXAPIResourceTaskBackend.h"
#include "Resources/Subsystems/GFXAPI/Types/Texture.h"

#include "Platform/Vulkan/VulkanImport.h"
#include "GFXAPI/Vulkan/Environment.h"

namespace Engine {
  namespace Vulkan {
    using namespace Engine::Resources;
    using namespace Engine::GFXAPI;

    /**********************************************************************************************//**
     * \def	DxTypes__
     *
     * \brief	A macro that defines all supported DirectX11 types being used!
     **************************************************************************************************/
    #define VkTypes 																			    		                                      /*                    */\
			  VkSwapchainKHR, 							     														                                    /* Components         */\
     	  VkImage,                                        											                            /* Texture-Types      */\
        VkBuffer,                                                                                         /* Generic-Buffers    */\
			  VkImageView, VkBufferView,                                                                       	/* View-Types         */\
			  VkShaderModule                                                                                    /* Shader-Types       */

  }

  #define DeclareTaskBuilderModule(Type)                                                                                                                  \
          EEngineStatus fn##Type##CreationTask   (Type::CreationRequest    const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus fn##Type##UpdateTask     (Type::UpdateRequest      const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus fn##Type##DestructionTask(Type::DestructionRequest const&request, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus fn##Type##QueryTask      (Type::Query              const&request, ResourceTaskFn_t &outTask);                                               

  namespace Vulkan {

    /**********************************************************************************************//**
     * \class	DX11ResourceTaskBuilder
     *
     * \brief	Glue...
     **************************************************************************************************/
    class VulkanResourceTaskBackend
      : public GFXAPIResourceTaskBackend
    {
    public:
      VulkanResourceTaskBackend(Ptr<VulkanEnvironment> const&);

      void initialize();

    private:
      DeclareTaskBuilderModule(Texture);

      Ptr<VulkanEnvironment> m_vulkanEnvironment;
    };


  }
}

#endif
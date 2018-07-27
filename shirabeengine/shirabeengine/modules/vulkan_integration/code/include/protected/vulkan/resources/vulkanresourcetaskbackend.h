#ifndef __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__
#define __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__

#include <map>

#include "GraphicsAPI/Resources/GFXAPIResourceTaskBackend.h"
#include "GraphicsAPI/Resources/Types/Texture.h"
#include "GraphicsAPI/Resources/Types/TextureView.h"

#include "Vulkan/VulkanImport.h"
#include "Vulkan/VulkanEnvironment.h"

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
          EEngineStatus fn##Type##UpdateTask     (Type::UpdateRequest      const&request, GFXAPIResourceHandleAssignment const&, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus fn##Type##DestructionTask(Type::DestructionRequest const&request, GFXAPIResourceHandleAssignment const&, ResolvedDependencyCollection const&depencies, ResourceTaskFn_t &outTask); \
          EEngineStatus fn##Type##QueryTask      (Type::Query              const&request, GFXAPIResourceHandleAssignment const&, ResourceTaskFn_t &outTask);                                               

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
      VulkanResourceTaskBackend(CStdSharedPtr_t<VulkanEnvironment> const&);

      void initialize();

    private:
      DeclareTaskBuilderModule(Texture);
      DeclareTaskBuilderModule(TextureView);
      //DeclareTaskBuilderModule(Buffer);
      //DeclareTaskBuilderModule(BufferView);

      CStdSharedPtr_t<VulkanEnvironment> m_vulkanEnvironment;
    };


  }
}

#endif
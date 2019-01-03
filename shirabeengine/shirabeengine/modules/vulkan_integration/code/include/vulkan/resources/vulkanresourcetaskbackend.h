#ifndef __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__
#define __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__

#include <map>

#include <graphicsapi/resources/gfxapiresourcetaskbackend.h>
#include <graphicsapi/resources/types/texture.h>
#include <graphicsapi/resources/types/textureview.h>
#include <graphicsapi/resources/types/renderpass.h>
#include <graphicsapi/resources/types/framebuffer.h>
#include "vulkan/vulkanimport.h"
#include "vulkan/vulkanenvironment.h"

namespace engine
{
    namespace vulkan
    {
        using namespace engine::resources;
        using namespace engine::gfxapi;

        #define VkTypes 	\
            VkSwapchainKHR, \
            VkRenderPass,   \
            VkFrameBuffer,  \
            VkImage,        \
            VkBuffer,       \
            VkImageView,    \
            VkBufferView,   \
            VkShaderModule
    }

    #define SHIRABE_DECLARE_TASK_BUILDER_MODULE(type)                          \
        EEngineStatus fn##type##CreationTask(                                  \
                            C##type::CCreationRequest      const &aRequest,    \
                            ResolvedDependencyCollection_t const &aDepencies,  \
                            ResourceTaskFn_t                     &aOutTask);   \
        EEngineStatus fn##type##UpdateTask(                                    \
                            C##type::CUpdateRequest         const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResolvedDependencyCollection_t  const &aDepencies, \
                            ResourceTaskFn_t                      &aOutTask);  \
        EEngineStatus fn##type##DestructionTask(                               \
                            C##type::CDestructionRequest    const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResolvedDependencyCollection_t  const &aDepencies, \
                            ResourceTaskFn_t                      &aOutTask);  \
        EEngineStatus fn##type##QueryTask(                                     \
                            C##type::CQuery                 const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResourceTaskFn_t                      &aOutTask);

    namespace vulkan
    {
        /**
         * The CVulkanResourceTaskBackend class implements the graphics API resource task
         * backend for the vulkan API.
         */
        class CVulkanResourceTaskBackend
                : public CGFXAPIResourceTaskBackend
        {
        public_constructors:
            /**
             * Construct a vulkan resource task backend from a vulkan environment.
             *
             * @param aVulkanEnvironment The vulkan environment to attach the backend to.
             */
            CVulkanResourceTaskBackend(CStdSharedPtr_t<CVulkanEnvironment> const &aVulkanEnvironment);

        public_methods:
            /**
             * Initialize the backend.
             */
            EEngineStatus initialize();

            /**
             * Deinitialize the backend.
             */
            EEngineStatus deinitialize();

        private_methods:
            SHIRABE_DECLARE_TASK_BUILDER_MODULE(Texture);
            SHIRABE_DECLARE_TASK_BUILDER_MODULE(TextureView);
            //DeclareTaskBuilderModule(Buffer);
            //DeclareTaskBuilderModule(BufferView);
            SHIRABE_DECLARE_TASK_BUILDER_MODULE(RenderPass);
            SHIRABE_DECLARE_TASK_BUILDER_MODULE(FrameBuffer);

        private_members:
            CStdSharedPtr_t<CVulkanEnvironment> mVulkanEnvironment;
        };


    }
}

#endif
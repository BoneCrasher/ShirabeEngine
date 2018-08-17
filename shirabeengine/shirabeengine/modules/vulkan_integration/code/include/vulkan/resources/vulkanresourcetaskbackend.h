#ifndef __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__
#define __SHIRABE_RESOURCES_VULKAN_TASKBUILDER_H__

#include <map>

#include <graphicsapi/resources/gfxapiresourcetaskbackend.h>
#include <graphicsapi/resources/types/texture.h>
#include <graphicsapi/resources/types/textureview.h>
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
            VkImage,        \
            VkBuffer,       \
            VkImageView,    \
            VkBufferView,   \
            VkShaderModule
    }

    #define DeclareTaskBuilderModule(Type)                                     \
        EEngineStatus fn##Type##CreationTask(                                  \
                            Type::CCreationRequest         const &aRequest,    \
                            ResolvedDependencyCollection_t const &aDepencies,  \
                            ResourceTaskFn_t                     &aOutTask);   \
        EEngineStatus fn##Type##UpdateTask(                                    \
                            Type::CUpdateRequest            const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResolvedDependencyCollection_t  const &aDepencies, \
                            ResourceTaskFn_t                      &aOutTask);  \
        EEngineStatus fn##Type##DestructionTask(                               \
                            Type::CDestructionRequest       const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResolvedDependencyCollection_t  const &aDepencies, \
                            ResourceTaskFn_t                      &aOutTask);  \
        EEngineStatus fn##Type##QueryTask(                                     \
                            Type::CQuery                    const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResourceTaskFn_t                      &aOutTask);

    namespace vulkan
    {
        /**
         * The VulkanResourceTaskBackend class implements the graphics API resource task
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
            void initialize();

            /**
             * Deinitialize the backend.
             */
            void deinitialize();

        private_methods:
            DeclareTaskBuilderModule(CTexture);
            DeclareTaskBuilderModule(CTextureView);
            //DeclareTaskBuilderModule(Buffer);
            //DeclareTaskBuilderModule(BufferView);

        private_members:
            CStdSharedPtr_t<CVulkanEnvironment> mVulkanEnvironment;
        };


    }
}

#endif

#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <log/log.h>
#include <renderer/irenderer.h>
#include <renderer/renderertypes.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include "vulkan/vulkanenvironment.h"

namespace engine
{
    namespace vulkan
    {
        using namespace engine::rendering;

        /**
         * The CVulkanRenderContext class implements the IRenderContext for
         * the vulkan API.
         */
        class CVulkanRenderContext
                : public IRenderContext
        {
            SHIRABE_DECLARE_LOG_TAG(CVulkanRenderContext);

        public_methods:
            /**
             * Initialize this render context.
             *
             * @param aGraphicsAPIResourceBackend The resource backend to access and use for graphics resources.
             * @param aVulkanEnvironment          The current vulkan environment initialized for this application.
             * @return                            True, if successful. False otherwise.
             */
            bool initialize(
                    CStdSharedPtr_t<CVulkanEnvironment>             const &aVulkanEnvironment,
                    CStdSharedPtr_t<gfxapi::CGFXAPIResourceBackend> const &aGraphicsAPIResourceBackend);

            /**
             * Destroy and run...
             *
             * @return See brief.
             */
            bool deinitialize();


            EEngineStatus bindGraphicsCommandBuffer();

            EEngineStatus commitGraphicsCommandBuffer();

            /**
             * Bind the graphics API swapchain to the pipeline (if any...)
             *
             * @param  aSwapChainResourceId The resource id by which the swapchain should be inserted into the resource backend.
             * @return                      EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus bindSwapChain(PublicResourceId_t const &aSwapChainResourceId);

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus present();

            /**
             * Bind a resource to the pipeline.
             *
             * @param aResourceId The id of the resourcer to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus bindResource(PublicResourceId_t const &aResourceId);

            /**
             * Undbind a resource from the pipeline.
             *
             * @param aResourceId The id of the resourcer to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unbindResource(PublicResourceId_t const &aResourceId);

            /**
             * Render a renderable entity using vulkan.
             *
             * @param aRenderable The renderable entity to process.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus render(SRenderable const &aRenderable);

        private_members:
            CStdSharedPtr_t<CVulkanEnvironment>             mVulkanEnvironment;
            CStdSharedPtr_t<gfxapi::CGFXAPIResourceBackend> mGraphicsAPIResourceBackend;
        };
    }
}

#endif

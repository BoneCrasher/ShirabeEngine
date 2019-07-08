#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <log/log.h>
#include <renderer/irendercontext.h>
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

            //
            // IRenderContext implementation
            //

            /**
             * Begin a subpass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            EEngineStatus beginSubpass();

            /**
             * End a subpass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            EEngineStatus endSubpass();

            /**
             * Copy one image to another.
             * This will incorporate explicit image memory barriers.
             *
             * @param aSourceImageId The id of the image to copy from.
             * @param aTargetImageId The id of the image to copy to.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error on any error.
             */
            EEngineStatus copyImage(PublicResourceId_t const &aSourceImageId,
                                    PublicResourceId_t const &aTargetImageId);

            /**
             * Copy one image to the current backbuffer.
             * This will incorporate explicit image memory barriers.
             *
             * @param aImageId
             * @return
             */
            EEngineStatus copyToBackBuffer(PublicResourceId_t const &aImageId);
            /**
             * Put the current internal command buffer into recording mode.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            EEngineStatus beginGraphicsCommandBuffer();

            /**
             * Stop recording in the current internal command buffer.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            EEngineStatus commitGraphicsCommandBuffer();

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly begin the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to bind.
             * @param aRenderPassId  The id of the render pass to begin with.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            EEngineStatus bindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                       std::string const &aRenderPassId);

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly end the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to unbind.
             * @param aRenderPassId  The id of the render pass to end.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            EEngineStatus unbindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                         std::string const &aRenderPassId);

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
             * Bind a pipeline instance  in the GPU.
             *
             * @param aPipelineUID The uid of the pipeline instance to bind.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::Error, if failed.
             */
            EEngineStatus bindPipeline(PublicResourceId_t const &aPipelineUID);

            /**
             * Unbind a pipeline instance from the GPU.
             *
             * @param aPipelineUID The uid of the pipeline instance to unbind.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::Error, if failed.
             */
            EEngineStatus unbindPipeline(PublicResourceId_t const &aPipelineUID);

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

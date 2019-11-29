#ifndef __SHIRABE_VULKAN_RENDERCONTEXT_H__
#define __SHIRABE_VULKAN_RENDERCONTEXT_H__

#include <log/log.h>
#include <resources/resourcetypes.h>
#include <renderer/irendercontext.h>
#include <renderer/renderertypes.h>
#include "vulkan_integration/vulkanenvironment.h"
#include "vulkan_integration/resources/vulkanresourceoperations.h"

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
            bool initialize(Shared<CVulkanEnvironment>     const &aVulkanEnvironment
                          , Shared<CGpuApiResourceStorage> const &aResourceStorage);

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
            EEngineStatus beginSubpass() final;

            /**
             * End a subpass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            EEngineStatus endSubpass() final;

            /**
             * Copy one image to another.
             * This will incorporate explicit image memory barriers.
             *
             * @param aSourceImageId The id of the image to copy from.
             * @param aTargetImageId The id of the image to copy to.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error on any error.
             */
            EEngineStatus copyImage(GpuApiHandle_t const &aSourceImageId,
                                    GpuApiHandle_t const &aTargetImageId) final;

            /**
             * Copy one image to the current backbuffer.
             * This will incorporate explicit image memory barriers.
             *
             * @param aImageId
             * @return
             */
            EEngineStatus copyToBackBuffer(GpuApiHandle_t const &aImageId) final;

            EEngineStatus transferBufferData(ByteBuffer const &aDataSource, GpuApiHandle_t const &aGpuBufferHandle);

            /**
             * Put the current internal command buffer into recording mode.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            EEngineStatus beginGraphicsCommandBuffer() final;

            /**
             * Stop recording in the current internal command buffer.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            EEngineStatus commitGraphicsCommandBuffer() final;

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly begin the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to bind.
             * @param aRenderPassId  The id of the render pass to begin with.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            EEngineStatus bindRenderPass(GpuApiHandle_t const &aRenderPassId,
                                         GpuApiHandle_t const &aFrameBufferId) final;

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly end the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to unbind.
             * @param aRenderPassId  The id of the render pass to end.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            EEngineStatus unbindRenderPass(GpuApiHandle_t const &aRenderPassId,
                                           GpuApiHandle_t const &aFrameBufferId) final;

            /**
             * Bind the graphics API swapchain to the pipeline (if any...)
             *
             * @param  aSwapChainResourceId The resource id by which the swapchain should be inserted into the resource backend.
             * @return                      EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus bindSwapChain(GpuApiHandle_t const &aSwapChainResourceId) final;

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus present() final;

            /**
             * Bind a pipeline instance  in the GPU.
             *
             * @param aPipelineUID The uid of the pipeline instance to bind.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::Error, if failed.
             */
            EEngineStatus bindPipeline(GpuApiHandle_t const &aPipelineUID) final;

            /**
             * Unbind a pipeline instance from the GPU.
             *
             * @param aPipelineUID The uid of the pipeline instance to unbind.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::Error, if failed.
             */
            EEngineStatus unbindPipeline(GpuApiHandle_t const &aPipelineUID) final;

            /**
             * Bind a resource to the pipeline.
             *
             * @param aResourceId The id of the resourcer to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus bindResource(GpuApiHandle_t const &aResourceId) final;

            /**
             * Undbind a resource from the pipeline.
             *
             * @param aResourceId The id of the resourcer to bind.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus unbindResource(GpuApiHandle_t const &aResourceId) final;

            /**
             * Render a renderable entity using vulkan.
             *
             * @param aRenderable The renderable entity to process.
             * @return            EEngineStatus::Ok if successful. False otherwise.
             */
            EEngineStatus render(SRenderable const &aRenderable) final;

        private_members:
            Shared<CVulkanEnvironment>     mVulkanEnvironment;
            Shared<CGpuApiResourceStorage> mResourceStorage;
        };
    }
}

#endif

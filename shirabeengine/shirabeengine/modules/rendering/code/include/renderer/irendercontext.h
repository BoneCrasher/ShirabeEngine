#ifndef __SHIRABE_IRENDERCONTEXT_H__
#define __SHIRABE_IRENDERCONTEXT_H__

#include <core/basictypes.h>
#include <core/enginestatus.h>
#include <os/applicationenvironment.h>
#include <resources/igpuapiresourceobject.h>
#include <wsi/display.h>
#include "renderer/rendererconfiguration.h"
#include "renderer/renderertypes.h"

namespace engine
{
    namespace rendering
    {
        using namespace os;
        using resources::GpuApiHandle_t;
        // using namespace engine::resources;

        /**
         * The IRenderContext interface describes rendering operations supported in
         * order to generate a command buffer.
         */
        class IRenderContext
        {
            SHIRABE_DECLARE_INTERFACE(IRenderContext);

        public_api:

            virtual EEngineStatus clearAttachments(GpuApiHandle_t const &aRenderPassId, uint32_t const &aCurrentSubpassIndex) = 0;

            /**
             * Begin a subpass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            virtual EEngineStatus beginSubpass() = 0;

            /**
             * End a subpass.
             *
             * @return EEngineStatus::Ok, if successful.
             */
            virtual EEngineStatus endSubpass() = 0;

            /**
             * Copy one image to another.
             * This will incorporate explicit image memory barriers.
             *
             * @param aSourceImageId The id of the image to copy from.
             * @param aTargetImageId The id of the image to copy to.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error on any error.
             */
            virtual EEngineStatus copyImage(GpuApiHandle_t const &aSourceImageId,
                                            GpuApiHandle_t const &aTargetImageId) = 0;

            virtual EEngineStatus performImageLayoutTransfer(  GpuApiHandle_t     const &aImageHandle
                                                             , CRange             const &aArrayRange
                                                             , CRange             const &aMipRange
                                                             , VkImageAspectFlags const &aAspectFlags
                                                             , VkImageLayout      const &aSourceLayout
                                                             , VkImageLayout      const &aTargetLayout) = 0;

            /**
             * Copy one image to the current backbuffer.
             * This will incorporate explicit image memory barriers.
             *
             * @param aImageId
             * @return
             */
            virtual EEngineStatus copyToBackBuffer(GpuApiHandle_t const &aImageId) = 0;

            virtual EEngineStatus transferBufferData(ByteBuffer const &aDataSource, GpuApiHandle_t const &aGpuBufferHandle) = 0;

            virtual EEngineStatus transferImageData(GpuApiHandle_t const &aTextureResourceHandle) = 0;

            virtual EEngineStatus updateResourceBindings(  GpuApiHandle_t                    const &aGpuMaterialHandle
                                                         , std::vector<GpuApiHandle_t>       const &aGpuBufferHandles
                                                         , std::vector<GpuApiHandle_t>       const &aGpuInputAttachmentTextureViewHandles
                                                         , std::vector<SSampledImageBinding> const &aGpuTextureViewHandles) = 0;

            /**
             * Put the current internal command buffer into recording mode.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus beginFrameCommandBuffers() = 0;

            /**
             * Stop recording in the current internal command buffer.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus commitFrameCommandBuffers() = 0;

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly begin the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to bind.
             * @param aRenderPassId  The id of the render pass to begin with.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus bindRenderPass(GpuApiHandle_t const &aRenderPassId,
                                                 GpuApiHandle_t const &aFrameBufferId) = 0;

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly end the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to unbind.
             * @param aRenderPassId  The id of the render pass to end.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus unbindRenderPass(GpuApiHandle_t const &aRenderPassId,
                                                   GpuApiHandle_t const &aFrameBufferId) = 0;

            /**
             * Bind the graphics API swapchain to the pipeline (if any...)
             *
             * @param  aSwapChainResourceId The resource id by which the swapchain should be inserted into the resource backend.
             * @return                      EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus beginGraphicsFrame() = 0;
            virtual EEngineStatus endGraphicsFrame() = 0;

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus present() = 0;

            virtual EEngineStatus bindAttributeAndIndexBuffers(GpuApiHandle_t const &aAttributeBufferId, GpuApiHandle_t const &aIndexBufferId, Vector<VkDeviceSize> aOffsets) = 0;

            /**
             * Bind a pipeline instance  in the GPU.
             *
             * @param aPipelineUID The uid of the pipeline instance to bind.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::Error, if failed.
             */
            virtual EEngineStatus bindPipeline(GpuApiHandle_t const &aPipelineUID) = 0;

            /**
             * Unbind a pipeline instance from the GPU.
             *
             * @param aPipelineUID The uid of the pipeline instance to unbind.
             * @return             EEngineStatus::Ok, if successful.
             * @return             EEngineStatus::Error, if failed.
             */
            virtual EEngineStatus unbindPipeline(GpuApiHandle_t const &aPipelineUID) = 0;

            /**
             * Bind a resource to the pipeline of the respective graphics API used.
             *
             * @param aResourceUID The UID of the resource to be bound.
             * @return             EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus bindResource(GpuApiHandle_t const &aResourceUID)   = 0;
            /**
             * Unbind a resource from the pipeline of the respective graphics API used.
             *
             * @param aResourceUID The UID of the resource to be bound.
             * @return             EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus unbindResource(GpuApiHandle_t const &aResourceUID) = 0;

            /**
             * Render a renderable entity using the pipeline of the respective graphics API used.
             *
             * @param aRenderable The renderable entity to be processed.
             * @return            EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus render(SRenderable const &aRenderable) = 0;

            virtual EEngineStatus drawIndex(uint32_t const aIndexCount) = 0;

            virtual EEngineStatus drawQuad() = 0;


        };
    }
}

#endif

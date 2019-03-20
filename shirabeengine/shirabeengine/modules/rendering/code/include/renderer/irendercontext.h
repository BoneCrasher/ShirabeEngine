#ifndef __SHIRABE_IRENDERER_H__
#define __SHIRABE_IRENDERER_H__

#include <core/basictypes.h>
#include <core/enginestatus.h>
#include <os/applicationenvironment.h>
#include <resources/core/resourcemanagerbase.h>
#include <wsi/display.h>
#include "renderer/rendererconfiguration.h"
#include "renderer/renderertypes.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine
{
    namespace rendering
    {
        using namespace os;
        using namespace engine::resources;

        using engine::framegraph::IFrameGraphRenderContext;

        /**
         * The IRenderContext interface describes rendering operations supported in
         * order to generate a command buffer.
         */
        class IRenderContext
        {
            SHIRABE_DECLARE_INTERFACE(IRenderContext);

        public_api:

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
            virtual EEngineStatus copyImage(PublicResourceId_t const &aSourceImageId,
                                            PublicResourceId_t const &aTargetImageId) = 0;

            /**
             * Copy one image to the current backbuffer.
             * This will incorporate explicit image memory barriers.
             *
             * @param aImageId
             * @return
             */
            virtual EEngineStatus copyToBackBuffer(PublicResourceId_t const &aImageId) = 0;

            /**
             * Put the current internal command buffer into recording mode.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus beginGraphicsCommandBuffer() = 0;

            /**
             * Stop recording in the current internal command buffer.
             *
             * @return EEngineStatus::Ok, if successful.
             * @return EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus commitGraphicsCommandBuffer() = 0;

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly begin the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to bind.
             * @param aRenderPassId  The id of the render pass to begin with.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus bindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                               std::string const &aRenderPassId) = 0;

            /**
             * Bind the framebuffer and render pass with the provided ids, if found.
             * Will implicitly end the render pass.
             *
             * @param aFrameBufferId The id of the framebuffer to unbind.
             * @param aRenderPassId  The id of the render pass to end.
             * @return               EEngineStatus::Ok, if successful.
             * @return               EEngineStatus::Error, on any error.
             */
            virtual EEngineStatus unbindFrameBufferAndRenderPass(std::string const &aFrameBufferId,
                                                                 std::string const &aRenderPassId) = 0;

            /**
             * Bind the graphics API swapchain to the pipeline (if any...)
             *
             * @param  aSwapChainResourceId The resource id by which the swapchain should be inserted into the resource backend.
             * @return                      EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus bindSwapChain(PublicResourceId_t const &aSwapChainResourceId) = 0;

            /**
             * Commit all changes and present the rendered content in the backbuffer to screen.
             *
             * @return EEngineStatus::Ok if successful. False otherwise.
             */
            virtual EEngineStatus present() = 0;

            /**
             * Bind a resource to the pipeline of the respective graphics API used.
             *
             * @param aResourceUID The UID of the resource to be bound.
             * @return             EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus bindResource(PublicResourceId_t const &aResourceUID)   = 0;
            /**
             * Unbind a resource from the pipeline of the respective graphics API used.
             *
             * @param aResourceUID The UID of the resource to be bound.
             * @return             EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus unbindResource(PublicResourceId_t const &aResourceUID) = 0;

            /**
             * Render a renderable entity using the pipeline of the respective graphics API used.
             *
             * @param aRenderable The renderable entity to be processed.
             * @return            EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus render(SRenderable const &aRenderable) = 0;
        };
    }
}

#endif

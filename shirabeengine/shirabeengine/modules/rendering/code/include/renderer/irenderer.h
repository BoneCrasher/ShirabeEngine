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

            virtual EEngineStatus bindGraphicsCommandBuffer() = 0;

            virtual EEngineStatus commitGraphicsCommandBuffer() = 0;

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

        /**
         * The IRenderer interface describes the basic capabilities of an engine renderer required
         * in order to use it for rendering output.
         */
        class IRenderer
        {
            SHIRABE_DECLARE_INTERFACE(IRenderer);

        public_api:
            /**
             * Initialize the renderer.
             *
             * @param aApplicationEnvironment  Hardware and software environment of the running application.
             * @param aDisplay                 WSI display to bind against the current hardware monitor config.
             * @param aConfiguration           Renderer configuration as provided from the engine default config
             *                                 and dynamic engine configuration.
             * @param aFrameGraphRenderContext Rendercontext for the framegraph.
             * @return                         EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus initialize(
                    CStdSharedPtr_t<SApplicationEnvironment>  const &aApplicationEnvironment,
                    CStdSharedPtr_t<wsi::CWSIDisplay>         const &aDisplay,
                    SRendererConfiguration                    const &aConfiguration,
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aFrameGraphRenderContext) = 0;

            /**
             * Deinitialize and clear the renderer.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus deinitialize() = 0;
            /**
             * Try to reinitialize the renderer, e.g. due to a configuration change.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus reinitialize() = 0;

            /**
             * Pause the renderer, stopping and rejecting all upcoming render requests.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus pause() = 0;
            /**
             * Resume the renderer.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            virtual EEngineStatus resume() = 0;
            /**
             * Check, whether the renderer is paused.
             *
             * @return True, if paused. False otherwise.
             */
            virtual bool isPaused() const = 0;

            /**
             * Render an entire scene... (format not specified yet..)
             * @return
             */
            virtual EEngineStatus renderScene() = 0;
        };
    }
}

#endif

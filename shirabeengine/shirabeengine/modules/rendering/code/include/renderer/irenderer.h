#ifndef __SHIRABE_IRENDERER_H__
#define __SHIRABE_IRENDERER_H__

#include <core/basictypes.h>
#include <core/enginestatus.h>
#include <os/applicationenvironment.h>
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
                    Shared<SApplicationEnvironment>  const &aApplicationEnvironment,
                    Shared<wsi::CWSIDisplay>         const &aDisplay,
                    SRendererConfiguration                    const &aConfiguration,
                    Shared<IFrameGraphRenderContext>       &aFrameGraphRenderContext) = 0;

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
            virtual EEngineStatus renderScene(RenderableList const &) = 0;
        };
    }
}

#endif

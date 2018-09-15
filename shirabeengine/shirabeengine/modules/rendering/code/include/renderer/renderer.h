#ifndef __SHIRABE_RENDERER_H__
#define __SHIRABE_RENDERER_H__

#include <atomic>
#include "renderer/irenderer.h"

namespace engine
{
    namespace rendering
    {
        using engine::framegraph::IFrameGraphRenderContext;

        /**
         * @brief The CRenderer class
         */
        class CRenderer
                : public IRenderer
        {
        public_constructors:
            CRenderer();

        public_destructors:
            ~CRenderer();

        public_methods:
            /**
             * Initialize the renderer.
             *
             * @param aApplicationEnvironment  Hardware and software environment of the running application.
             * @param aDisplay                 WSI display instance to bind against the currently selected monitor config.
             * @param aConfiguration           Renderer configuration as provided from the engine default config
             *                                 and dynamic engine configuration.
             * @param aFrameGraphRenderContext Rendercontext for the framegraph.
             * @return                         EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus initialize(
                    CStdSharedPtr_t<SApplicationEnvironment>  const &aApplicationEnvironment,
                    CStdSharedPtr_t<wsi::CWSIDisplay>         const &aDisplay,
                    SRendererConfiguration                    const &aConfiguration,
                    CStdSharedPtr_t<IFrameGraphRenderContext>       &aFrameGraphRenderContext) ;

            /**
             * Deinitialize and clear the renderer.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus deinitialize();

            /**
             * Try to reinitialize the renderer, e.g. due to a configuration change.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus reinitialize();

            /**
             * Pause the renderer, stopping and rejecting all upcoming render requests.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus pause();

            /**
             * Resume the renderer.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus resume();

            /**
             * Check, whether the renderer is paused.
             *
             * @return True, if paused. False otherwise.
             */
            bool isPaused() const;

            /**
             * Render an entire scene... (format not specified yet..)
             * @return
             */
            EEngineStatus renderScene();

        private_members:
            SRendererConfiguration                    mConfiguration;
            CStdSharedPtr_t<SApplicationEnvironment>  mAppEnvironment;
            CStdSharedPtr_t<wsi::CWSIDisplay>         mDisplay;
            CStdSharedPtr_t<IFrameGraphRenderContext> mFrameGraphRenderContext;
            std::atomic<bool>                         mPaused;

        };

    }
}

#endif

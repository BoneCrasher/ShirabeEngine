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
            SHIRABE_DECLARE_LOG_TAG(CRenderer);

        public_constructors:
            CRenderer();

        public_destructors:
            ~CRenderer() final;

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
                    Shared<SApplicationEnvironment>  const &aApplicationEnvironment,
                    Shared<wsi::CWSIDisplay>         const &aDisplay,
                    SRendererConfiguration           const &aConfiguration,
                    Shared<IFrameGraphRenderContext>       &aFrameGraphRenderContext) final;

            /**
             * Deinitialize and clear the renderer.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus deinitialize() final;

            /**
             * Try to reinitialize the renderer, e.g. due to a configuration change.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus reinitialize() final;

            /**
             * Pause the renderer, stopping and rejecting all upcoming render requests.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus pause() final;

            /**
             * Resume the renderer.
             *
             * @return EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus resume() final;

            /**
             * Check, whether the renderer is paused.
             *
             * @return True, if paused. False otherwise.
             */
             [[nodiscard]]
            bool isPaused() const final;

            /**
             * Render an entire scene... (format not specified yet..)
             * @return
             */
            EEngineStatus renderScene(RenderableList const &aRenderableCollection) final;

        private_members:
            SRendererConfiguration           mConfiguration;
            Shared<SApplicationEnvironment>  mAppEnvironment;
            Shared<wsi::CWSIDisplay>         mDisplay;
            Shared<IFrameGraphRenderContext> mFrameGraphRenderContext;
            std::atomic<bool>                mPaused;

        };

    }
}

#endif

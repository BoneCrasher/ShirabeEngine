#ifndef __SHIRABE_RENDERER_H__
#define __SHIRABE_RENDERER_H__

#include <atomic>
#include <os/applicationenvironment.h>
#include <wsi/display.h>
#include "renderer/rendererconfiguration.h"
#include "renderer/rendergraph/rendergraphdata.h"
#include "renderer/rendergraph/rendergraph.h"

namespace engine
{
    namespace rendering
    {
        using engine::framegraph::SRenderGraphPlatformContext;
        using engine::framegraph::SRenderGraphResourceContext;
        using engine::framegraph::SRenderGraphRenderContext;
        using engine::framegraph::SRenderGraphDataSource;
        using engine::framegraph::CGraph;

        /**
         * @brief The CRenderer class
         */
        class CRenderer
        {
            SHIRABE_DECLARE_LOG_TAG(CRenderer);

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
             * @param aRenderGraphRenderContext Rendercontext for the rendergraph.
             * @return                         EEngineStatus::Ok, if successful. An error code otherwise.
             */
            EEngineStatus initialize(
                    Shared<os::SApplicationEnvironment> const &aApplicationEnvironment,
                    Shared<wsi::CWSIDisplay>            const &aDisplay,
                    SRendererConfiguration              const &aConfig);

            EEngineStatus createDeferredPipeline(rhi::CRHIResourceManager const            &aResourceManager
                                                 , framegraph::SRenderGraphResourceContext &aResourceContext);

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
             [[nodiscard]]
            bool isPaused() const;

            /**
             * Render an entire scene... (format not specified yet..)
             * @return
             */
            EEngineStatus renderSceneDeferred(SRenderGraphPlatformContext   &aPlatformContext
                                              , SRenderGraphDataSource      &aDataSource
                                              , SRenderGraphResourceContext &aResourceContext
                                              , SRenderGraphRenderContext   &aRenderContext);

        private_members:
            Shared<os::SApplicationEnvironment> mAppEnvironment;
            Shared<wsi::CWSIDisplay>            mDisplay;
            SRendererConfiguration              mConfiguration;
            std::atomic<bool>                   mPaused;

            Unique<engine::framegraph::CGraph> mDeferredGraph;
        };

    }
}

#endif

#ifndef __SHIRABE_FRAMEGRAPH_MODULE_GFXAPICOMMON_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_GFXAPICOMMON_H__

#include <platform/platform.h>
#include <log/log.h>
#include <resources/core/resourcemanagerbase.h>
#include <graphicsapi/resources/types/all.h>
#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/passbuilder.h"
#include "renderer/framegraph/modules/module.h"

namespace engine
{
    namespace framegraph
    {
        /**
         * Template specialization selector for the gbuffer module.
         */
        struct SGraphicsAPICommonModuleTag_t {};

        /**
         * The FrameGraphModule<SGBufferModuleTag_t> class implements all gbuffer generation
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CFrameGraphModule<SGraphicsAPICommonModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphModule<SGraphicsAPICommonModuleTag_t>);

        public_structs:
            /**
             * The SSwapChainPassImportData struct describes all imported data for the
             * SwapChain pass.
             */
            struct SSwapChainPassImportData
            {
                SFrameGraphResource backBufferInput;
            };

            /**
             * The SSwapChainPassExportData struct describes all exported data for the
             * SwapChain pass.
             */
            struct SSwapChainPassExportData
            {
                SFrameGraphResource backbuffer;
            };

            /**
             * The SPresentPassImportData struct describes all imported data for the
             * Present pass.
             */
            struct SPresentPassImportData
            {
            };

            /**
             * The SPresentPassExportData struct describes all exported data for the
             * Present pass.
             */
            struct SPresentPassExportData
            {
            };

        public_methods:
            /**
             * Add a swapchain pass to the framegraph, which will import a swapchain backbuffer image
             * and bind it to the underlying graphics API.
             *
             * @param aGraphBuilder    The graph builder to source from.
             * @param aWidth           The width of the backbuffer images.
             * @param aHeight          The height of the backbuffer images.
             * @param aFormat          The format of the backbuffer images.
             * @return                 Export data of this pass to chain it with other passes' inputs.
             */
            SSwapChainPassExportData addSwapChainPass(
                    CGraphBuilder            &aGraphBuilder,
                    uint32_t           const &aWidth,
                    uint32_t           const &aHeight,
                    FrameGraphFormat_t const &aFormat);

            /**
             * Add a present pass to commit the rendering result to screen and free up the swap chain image acquired.
             *
             * @param aGraphBuilder    The graph builder to source from.
             * @param aRenderingResult "No-Op" forwarded resource just to abuse the framegraph system to add this pass...
             *                         TODO: Think of a clean solution...
             * @return                 Export data of this pass. Will be empty as this pass is the end of execution.
             */
            SPresentPassExportData addPresentPass(
                    CGraphBuilder             &aGraphBuilder,
                    SFrameGraphResource const &aRenderingResult);
        };

    }
}

#endif

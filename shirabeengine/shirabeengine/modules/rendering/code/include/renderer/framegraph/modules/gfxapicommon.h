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
             * The SGBufferGenerationImportData struct describes all imported data for the
             * GBufferGeneration pass.
             */
            struct SSwapChainPassImportData
            {
                SFrameGraphResource backBufferInput;
            };

            /**
             * The SGBufferGenerationExportData struct describes all exported data for the
             * GBufferGeneration pass.
             */
            struct SSwapChainPassExportData
            {
                SFrameGraphResource backbuffer;
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
        };

    }
}

#endif

#ifndef __SHIRABE_FRAMEGRAPH_MODULE_PREPASS_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_PREPASS_H__

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
        struct SPrepassModuleTag_t {};

        /**
         * The FrameGraphModule<SGBufferModuleTag_t> class implements all gbuffer generation
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CFrameGraphModule<SPrepassModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphModule<SPrepassModuleTag_t>);

        public_structs:
            /**
             * The SGBufferGenerationImportData struct describes all imported data for the
             * GBufferGeneration pass.
             */
            struct SPrepassImportData
            {
                SFrameGraphResource backBufferInput;
            };

            /**
             * The SGBufferGenerationExportData struct describes all exported data for the
             * GBufferGeneration pass.
             */
            struct SPrepassExportData
            {
                SFrameGraphResource backbuffer;
            };

        public_methods:
            /**
             * Add a gbuffer generation pass to the render graph.
             *
             * @param aGraphBuilder    The graph builder to source from.
             * @param aRenderableInput The renderables for which gbuffer data should be generated.
             * @return                 Export data of this pass to chain it with other passes' inputs.
             */
            SPrepassExportData addPrepass(
                    CGraphBuilder             &aGraphBuilder,
                    SFrameGraphResource const &aBackBuffer);
        };

    }
}

#endif

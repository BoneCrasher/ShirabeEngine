#ifndef __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__

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
         * Template specialization selector for the lighting module.
         */
        struct SLightingModuleTag_t {};

        /**
         * The FrameGraphModule<SLightingModuleTag_t> class implements all lighting
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CFrameGraphModule<SLightingModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(FrameGraphModule<SLightingModuleTag_t>);

        public_structs:
            /**
             * The SLightingImportData struct describes all imported data for the
             * lighting pass.
             */
            struct SLightingImportData
            {
                SFrameGraphResource gbuffer0;
                SFrameGraphResource gbuffer1;
                SFrameGraphResource gbuffer2;
                SFrameGraphResource gbuffer3;
            };

            /**
             * The SLightingExportData struct describes all exported data for the
             * lighting pass.
             */
            struct SLightingExportData
            {
                SFrameGraphResource lightAccumulationBuffer;
            };

        public_methods:
            /**
             * Add a lighting pass to the render graph.
             *
             * @param aPassName     The unique name of the pass to add.
             * @param aGraphBuilder The graph builder to source from.
             * @param aGbuffer0     GBuffer source containing relevant information for lighting.
             * @param aGbuffer1     GBuffer source containing relevant information for lighting.
             * @param aGbuffer2     GBuffer source containing relevant information for lighting.
             * @param aGbuffer3     GBuffer source containing relevant information for lighting.
             * @return              Export data of this pass to chain it with other passes' inputs.
             */
            CEngineResult<SLightingExportData> addLightingPass(
                    std::string         const &aPassName,
                    CGraphBuilder             &aGraphBuilder,
                    SFrameGraphResource const &aGbuffer0,
                    SFrameGraphResource const &aGbuffer1,
                    SFrameGraphResource const &aGbuffer2,
                    SFrameGraphResource const &aGbuffer3);

        };

    }
}

#endif

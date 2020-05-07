#ifndef __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__

#include <log/log.h>
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
                SFrameGraphTextureView gbuffer0;
                SFrameGraphTextureView gbuffer1;
                SFrameGraphTextureView gbuffer2;
                SFrameGraphTextureView gbuffer3;
                SFrameGraphTextureView depth;

                SFrameGraphMaterial material;
            };

            /**
             * The SLightingExportData struct describes all exported data for the
             * lighting pass.
             */
            struct SLightingExportData
            {
                SFrameGraphTextureView lightAccumulationBuffer;
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
                    std::string const      &aPassName,
                    CGraphBuilder          &aGraphBuilder,
                    SFrameGraphTextureView &aGbuffer0,
                    SFrameGraphTextureView &aGbuffer1,
                    SFrameGraphTextureView &aGbuffer2,
                    SFrameGraphTextureView &aGbuffer4,
                    SFrameGraphTextureView &aDepthStencil);

        };

    }
}

#endif

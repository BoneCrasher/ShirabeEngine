#ifndef __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_LIGHTING_H__

#include <log/log.h>
#include "renderer/rendergraph/graphbuilder.h"
#include "renderer/rendergraph/passbuilder.h"
#include "renderer/rendergraph/modules/module.h"

namespace engine
{
    namespace framegraph
    {
        /**
         * Template specialization selector for the lighting module.
         */
        struct SLightingModuleTag_t {};

        /**
         * The RenderGraphModule<SLightingModuleTag_t> class implements all lighting
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CRenderGraphModule<SLightingModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(RenderGraphModule<SLightingModuleTag_t>);

        public_structs:

            struct SLightingPassInputData
            {
                SRenderGraphImageView gbuffer0
                                      , gbuffer1
                                      , gbuffer2
                                      , gbuffer3
                                      , depthStencil;
            };

            /**
             * The SLightingImportData struct describes all imported data for the
             * lighting pass.
             */
            struct SLightingImportData
            {
                SRenderGraphImageView gbuffer0;
                SRenderGraphImageView gbuffer1;
                SRenderGraphImageView gbuffer2;
                SRenderGraphImageView gbuffer3;
                SRenderGraphImageView depth;

                SRenderGraphMaterial material;
            };

            /**
             * The SLightingExportData struct describes all exported data for the
             * lighting pass.
             */
            struct SLightingExportData
            {
                SRenderGraphImageView lightAccumulationBuffer;
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
                std::string const        &aPassName
                , CGraphBuilder          &aGraphBuilder
                , SLightingPassInputData &aInputData);

        };

    }
}

#endif

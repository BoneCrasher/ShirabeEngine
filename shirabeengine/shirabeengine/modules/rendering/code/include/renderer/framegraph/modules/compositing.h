#ifndef __SHIRABE_FRAMEGRAPH_MODULE_COMPOSITING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_COMPOSITING_H__

#include <log/log.h>
#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/passbuilder.h"
#include "renderer/framegraph/modules/module.h"

namespace engine
{
    namespace framegraph
    {

        /**
         * Template specialization selector for the compositing module.
         */
        struct SCompositingModuleTag_t {};

        /**
         * The RenderGraphModule<SGBufferModuleTag_t> class implements all compositing
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CRenderGraphModule<SCompositingModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(CRenderGraphModule<SCompositingModuleTag_t>);

        public_structs:
            struct SInputData
            {
                SRenderGraphImageView gbuffer0
                                      , gbuffer1
                                      , gbuffer2
                                      , gbuffer3
                                      , depthStencil
                                      , lightAccumulationBuffer;

            };

            /**
             * The SImportData struct describes all imported data for the
             * compositing pass.
             */
            struct SImportData
            {
                SRenderGraphImageView gbuffer0;
                SRenderGraphImageView gbuffer1;
                SRenderGraphImageView gbuffer2;
                SRenderGraphImageView gbuffer3;
                SRenderGraphImageView depth;
                SRenderGraphImageView lightAccumulationBuffer;

                SRenderGraphMaterial material;
            };

            /**
             * The SExportData struct describes all exported data for the
             * compositing pass.
             */
            struct SExportData
            {
                SRenderGraphImageView output;
            };

        public_methods:
            /**
             * Add a compositing pass to the render graph.
             *
             * @param aPassName                The unique name of the pass to add.
             * @param aGraphBuilder            The graph builder to source from.
             * @param aGbuffer0                GBuffer source containing relevant information for lighting.
             * @param aGbuffer1                GBuffer source containing relevant information for lighting.
             * @param aGbuffer2                GBuffer source containing relevant information for lighting.
             * @param aGbuffer3                GBuffer source containing relevant information for lighting.
             * @param aLightAccumulationBuffer Light Accumulation data for compositing.
             * @return                         Export data of this pass to chain it with other passes' inputs.
             */
            CEngineResult<SExportData> addDefaultCompositingPass(
                std::string const &aPassName,
                CGraphBuilder     &aGraphBuilder,
                SInputData        &aInputData);
        };

    }
}

#endif

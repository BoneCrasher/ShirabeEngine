#ifndef __SHIRABE_FRAMEGRAPH_MODULE_GBUFFERGENERATION_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_GBUFFERGENERATION_H__

#include <platform/platform.h>
#include <log/log.h>
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
        struct SGBufferModuleTag_t {};

        /**
         * The RenderGraphModule<SGBufferModuleTag_t> class implements all gbuffer generation
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CRenderGraphModule<SGBufferModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(CRenderGraphModule<SGBufferModuleTag_t>);

        public_structs:
            struct SBufferGenerationInputData
            {};

            /**
             * The SGBufferGenerationImportData struct describes all imported data for the
             * GBufferGeneration pass.
             */
            struct SGBufferGenerationImportData
            { };

            /**
             * The SGBufferGenerationExportData struct describes all exported data for the
             * GBufferGeneration pass.
             */
            struct SGBufferGenerationExportData
            {
                SRenderGraphImageView gbuffer0;
                SRenderGraphImageView gbuffer1;
                SRenderGraphImageView gbuffer2;
                SRenderGraphImageView gbuffer3;
                SRenderGraphImageView depthStencil;
            };

        public_methods:
            /**
             * Add a gbuffer generation pass to the render graph.
             *
             * @param aPassName        The unique name of the pass to add.
             * @param aGraphBuilder    The graph builder to source from.
             * @param aRenderableInput The renderables for which gbuffer data should be generated.
             * @return                 Export data of this pass to chain it with other passes' inputs.
             */
            CEngineResult<SGBufferGenerationExportData> addGBufferGenerationPass(
                    std::string const                  &aPassName
                    , CGraphBuilder                    &aGraphBuilder
                    , SBufferGenerationInputData const &aInputData);
        };

    }
}

#endif

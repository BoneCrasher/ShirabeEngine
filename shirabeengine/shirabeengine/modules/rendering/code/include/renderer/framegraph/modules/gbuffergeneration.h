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
         * The FrameGraphModule<SGBufferModuleTag_t> class implements all gbuffer generation
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CFrameGraphModule<SGBufferModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphModule<SGBufferModuleTag_t>);

        public_structs:

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
                SFrameGraphTextureView gbuffer0;
                SFrameGraphTextureView gbuffer1;
                SFrameGraphTextureView gbuffer2;
                SFrameGraphTextureView gbuffer3;
                SFrameGraphTextureView depthStencil;
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
                    std::string               const &aPassName,
                    CGraphBuilder                   &aGraphBuilder);
        };

    }
}

#endif

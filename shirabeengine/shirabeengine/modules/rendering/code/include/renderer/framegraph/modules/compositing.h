#ifndef __SHIRABE_FRAMEGRAPH_MODULE_COMPOSITING_H__
#define __SHIRABE_FRAMEGRAPH_MODULE_COMPOSITING_H__

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
         * Template specialization selector for the compositing module.
         */
        struct SCompositingModuleTag_t {};

        /**
         * The FrameGraphModule<SGBufferModuleTag_t> class implements all compositing
         * related data structs and passes.
         */
        template<>
        class SHIRABE_TEST_EXPORT CFrameGraphModule<SCompositingModuleTag_t>
        {
            SHIRABE_DECLARE_LOG_TAG(CFrameGraphModule<SCompositingModuleTag_t>);

        public_structs:
            /**
             * The SImportData struct describes all imported data for the
             * compositing pass.
             */
            struct SImportData
            {
                SFrameGraphResource gbuffer0;
                SFrameGraphResource gbuffer1;
                SFrameGraphResource gbuffer2;
                SFrameGraphResource gbuffer3;
                SFrameGraphResource lightAccumulationBuffer;
            };

            /**
             * The SExportData struct describes all exported data for the
             * compositing pass.
             */
            struct SExportData
            {
                SFrameGraphResource output;
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
                    std::string         const &aPassName,
                    CGraphBuilder             &aGraphBuilder,
                    SFrameGraphResource const &aGbuffer0,
                    SFrameGraphResource const &aGbuffer1,
                    SFrameGraphResource const &aGbuffer2,
                    SFrameGraphResource const &aGbuffer3,
                    SFrameGraphResource const &aLightAccumulationBuffer);
        };

    }
}

#endif

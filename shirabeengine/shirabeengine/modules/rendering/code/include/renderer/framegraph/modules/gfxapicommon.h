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

        public_static_fields:
            static constexpr char const *sRenderPassResourceId  = "DefaultRenderPass";
            static constexpr char const *sFrameBufferResourceId = "DefaultFrameBuffer";

        public_structs:
            /**
             * The SPrePassImportData struct describes all imported data for the
             * Pre pass.
             */
            struct SPrePassImportData
            {
                SFrameGraphResource backBufferInput;
            };

            /**
             * The SPrePassExportData struct describes all exported data for the
             * Pre pass.
             */
            struct SPrePassExportData
            {
                SFrameGraphResource backbuffer;
            };

            /**
             * The SPresentPassImportData struct describes all imported data for the
             * Present pass.
             */
            struct SPresentPassImportData
            {
                SFrameGraphResource finalOutputId;
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
             * Add a pre pass to the framegraph, which will perform all preparation steps to render into
             * a graphics pipeline.
             *
             * @param aPassName        The unique name of the pass to add.
             * @param aGraphBuilder    The graph builder to source from.
             * @param aWidth           The width of the backbuffer images.
             * @param aHeight          The height of the backbuffer images.
             * @param aFormat          The format of the backbuffer images.
             * @return                 Export data of this pass to chain it with other passes' inputs.
             */
            CEngineResult<SPrePassExportData> addPrePass(
                    std::string        const &aPassName,
                    CGraphBuilder            &aGraphBuilder,
                    uint32_t           const &aWidth,
                    uint32_t           const &aHeight,
                    FrameGraphFormat_t const &aFormat);

            /**
             * Add a present pass to commit the rendering result to screen and free up the swap chain image acquired.
             *
             * @param aPassName        The unique name of the pass to add.
             * @param aGraphBuilder    The graph builder to source from.
             * @param aOutput          The final output of the framegraph to commit.
             * @return                 Export data of this pass. Will be empty as this pass is the end of execution.
             */
            CEngineResult<SPresentPassExportData> addPresentPass(
                    std::string         const &aPassName,
                    CGraphBuilder             &aGraphBuilder,
                    SFrameGraphResource const &aOutput);
        };

    }
}

#endif

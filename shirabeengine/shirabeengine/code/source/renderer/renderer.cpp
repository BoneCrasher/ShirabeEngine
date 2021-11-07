#include "renderer/rendergraph/rendergraphdata.h"
#include "renderer/rendergraph/graphbuilder.h"
#include "renderer/rendergraph/passbuilder.h"
#include "renderer/rendergraph/modules/gbuffergeneration.h"
#include "renderer/rendergraph/modules/lighting.h"
#include "renderer/rendergraph/modules/compositing.h"
#include "renderer/rendergraph/framegraphcontexts.h"
#include "renderer/rendergraph/platformcontext.h"
#include "renderer/rendergraph/rendergraphserialization.h"

#include "renderer/renderer.h"

namespace engine
{
    namespace rendering
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CRenderer::CRenderer()
            : mAppEnvironment(nullptr)
            , mDisplay(nullptr)
            , mPaused(true)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CRenderer::~CRenderer()
        {
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::initialize(
                Shared<os::SApplicationEnvironment> const &aApplicationEnvironment,
                Shared<wsi::CWSIDisplay>            const &aDisplay,
                SRendererConfiguration              const &aConfig)
        {
            assert(nullptr != aApplicationEnvironment);
            assert(nullptr != aDisplay);

            mAppEnvironment = aApplicationEnvironment;
            mDisplay        = aDisplay;
            mConfiguration  = aConfig;

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::createDeferredPipeline(resources::CRHIResourceManager const         &aResourceManager
                                                        , framegraph::SRenderGraphResourceContext &aResourceContext)
        {
            using namespace engine;
            using namespace engine::framegraph;

            os::SOSDisplayDescriptor const&displayDesc = mDisplay->screenInfo()[mDisplay->primaryScreenIndex()];

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            CGraphBuilder graphBuilder(aResourceManager);
            graphBuilder.setGraphMode(CGraph::EGraphMode::Graphics);
            graphBuilder.setRenderToBackBuffer(true);

            auto const &[initResult] = graphBuilder.initialize(mAppEnvironment, mDisplay);
            if(CheckEngineError(initResult))
            {
                CLog::Error(logTag(), "GraphBuilder initialization failed");
                return initResult;
            }

            CRenderGraphModule<SGBufferModuleTag_t>     gbufferModule          { };
            CRenderGraphModule<SLightingModuleTag_t>    lightingModule         { };
            CRenderGraphModule<SCompositingModuleTag_t> compositingModule      { };

            static std::string const sGBufferGenerationPassID = "GBufferGenerationPass";
            static std::string const sLightingPassID          = "LightingPass";
            static std::string const sCompositingPassID       = "CompositingPass";

            static constexpr char const* sDeferredPipelineId = "DeferredPipeline";

            auto const &[renderPassBeginResult, renderPassPtr] = graphBuilder.beginRenderPass("DeferredPipeline");
            if(CheckEngineError(renderPassBeginResult))
            {
                CLog::Error(logTag(), "Failed to begin render pass w/ name %s", sDeferredPipelineId);
                return EEngineStatus::Error;
            }

            // GBuffer

            CRenderGraphModule<SGBufferModuleTag_t>::SBufferGenerationInputData   gbufferInput { };
            CRenderGraphModule<SGBufferModuleTag_t>::SGBufferGenerationExportData gbufferExportData{ };
            gbufferExportData =
                    gbufferModule.addGBufferGenerationPass(
                                         sGBufferGenerationPassID,
                                         graphBuilder,
                                         gbufferInput).data();

            // Link SwapChain pass and GBuffer
            // graphBuilder.createPassDependency(sPrePassID, sGBufferGenerationPassID);

            // Lighting
            CRenderGraphModule<SLightingModuleTag_t>::SLightingPassInputData lightingInput { };
             lightingInput.gbuffer0     = gbufferExportData.gbuffer0;
             lightingInput.gbuffer1     = gbufferExportData.gbuffer1;
             lightingInput.gbuffer2     = gbufferExportData.gbuffer2;
             lightingInput.gbuffer3     = gbufferExportData.gbuffer3;
             lightingInput.depthStencil = gbufferExportData.depthStencil;

            CRenderGraphModule<SLightingModuleTag_t>::SLightingExportData lightingExportData { };
            lightingExportData =
                    lightingModule.addLightingPass(
                                          sLightingPassID,
                                          graphBuilder,
                                          lightingInput).data();

            // Compositing
            CRenderGraphModule<SCompositingModuleTag_t>::SInputData compositingInput { };
            compositingInput.gbuffer0                = gbufferExportData.gbuffer0;
            compositingInput.gbuffer1                = gbufferExportData.gbuffer1;
            compositingInput.gbuffer2                = gbufferExportData.gbuffer2;
            compositingInput.gbuffer3                = gbufferExportData.gbuffer3;
            compositingInput.depthStencil            = gbufferExportData.depthStencil;
            compositingInput.lightAccumulationBuffer = lightingExportData.lightAccumulationBuffer;

            CRenderGraphModule<SCompositingModuleTag_t>::SExportData compositingExportData{ };
            compositingExportData =
                    compositingModule.addDefaultCompositingPass(
                                             sCompositingPassID,
                                             graphBuilder,
                                         compositingInput).data();

            auto const &[renderPassEndResult] = graphBuilder.endRenderPass();
            if(CheckEngineError(renderPassEndResult))
            {
                CLog::Error(logTag(), "Failed to end render pass w/ name %s", sDeferredPipelineId);
                return EEngineStatus::Error;
            }

            graphBuilder.setOutputTextureResourceId(compositingExportData.output.resourceId);

            // Present
            // CRenderGraphModule<SGraphicsAPICommonModuleTag_t>::SPresentPassExportData presentPassExportData{};
            // presentPassExportData =
            //         graphicsAPICommonModule.addPresentPass(
            //             sPresentPassID,
            //             graphBuilder,
            //             compositingExportData.output).data();
            //
            // Link Compositing and Present
            // graphBuilder.createPassDependency(sCompositingPassID, sPresentPassID);

            // Compile the whole thing :)
            CEngineResult<Unique<engine::framegraph::CGraph>> compilation = graphBuilder.compile();
            if(not compilation.successful())
            {
                CLog::Error(logTag(), "Failed to compile the rendergraph.");
                return EEngineStatus::Error;
            }

            mDeferredGraph = std::move(compilation.data());

            #if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            static bool serializedOnce = false;
            if(!serializedOnce)
                {
                    serializedOnce = true;

                    Shared<CRenderGraphGraphVizSerializer::IResult> result     = nullptr;
                    Shared<CRenderGraphGraphVizSerializer>          serializer = std::make_shared<CRenderGraphGraphVizSerializer>();
                    bool const initialized  = serializer>initialize();
                    bool const serialized   = serializer>serialize(*mDeferredGraph, result);


                    Shared<CRenderGraphGraphVizSerializer::CRenderGraphSerializationResult> typedResult =
                            std::static_pointer_cast<CRenderGraphGraphVizSerializer::CRenderGraphSerializationResult>(result);

                    std::string serializedData {};
                    bool const dataFetched = typedResult>asString(serializedData);
                    writeFile("RenderGraphTest.gv", serializedData);

                    bool const deserialized = serializer>deinitialize();
                    serializer = nullptr;

                    system("tools/makeRenderGraphPNG.sah");
                }
            #endif

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::deinitialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::reinitialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::pause()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::resume()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool CRenderer::isPaused() const
        {
            return mPaused.load();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------

        /*!
         * Helper-Macro: If '_aCondition' is valid and evaluates to true, return '_aValue'.
         */
        #define SR_RETURN_IF( _aCondition, _aValue ) if( ( _aCondition) ) { return _aValue; }

        /*!
         * Write a string to a file.
         *
         * @param [in] aFilename The output filename of the file to write.
         * @param [in] aData     The string data to write into the file.
         * @return               True, if successful. False, otherwise.
         */
        bool writeFile(const std::string& aFilename, const std::string& aData)
        {
            SR_RETURN_IF(aFilename.empty(), false);
            SR_RETURN_IF(aData.empty(),     false);

            std::ofstream file(aFilename, std::ios::out);

            SR_RETURN_IF(file.bad(), false);

            file << aData;

            // File will be closed when leaving scope.
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::renderSceneDeferred(SRenderGraphPlatformContext   &aPlatformContext
                                                     , SRenderGraphDataSource      &aDataSource
                                                     , SRenderGraphResourceContext &aResourceContext
                                                     , SRenderGraphRenderContext   &aRenderContext)
        {
            if(mDeferredGraph)
            {
                CEngineResult<> const execution = mDeferredGraph->execute(aPlatformContext, aDataSource, aResourceContext, aRenderContext);
                if(not execution.successful())
                {
                    // TODO: Log
                    return execution.result();
                }
            }
        }
        //<-----------------------------------------------------------------------------
    }
}

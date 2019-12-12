#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/passbuilder.h"
#include "renderer/framegraph/modules/gfxapicommon.h"
#include "renderer/framegraph/modules/gbuffergeneration.h"
#include "renderer/framegraph/modules/lighting.h"
#include "renderer/framegraph/modules/compositing.h"
#include "renderer/framegraph/framegraphrendercontext.h"
#include "renderer/framegraph/framegraphserialization.h"

#include "renderer/renderer.h"

namespace engine
{
    namespace rendering
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CRenderer::CRenderer()
            : mConfiguration()
            , mAppEnvironment(nullptr)
            , mDisplay(nullptr)
            , mFrameGraphRenderContext(nullptr)
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
                Shared<SApplicationEnvironment> const &aApplicationEnvironment,
                Shared<wsi::CWSIDisplay>        const &aDisplay,
                SRendererConfiguration          const &aConfiguration,
                Shared<IFrameGraphRenderContext>      &aFrameGraphRenderContext,
                Shared<IRenderContext>                &aGpuApiRenderContext)
        {
            assert(nullptr != aApplicationEnvironment);
            assert(nullptr != aDisplay);
            assert(nullptr != aFrameGraphRenderContext);
            assert(nullptr != aGpuApiRenderContext);

            mConfiguration           = aConfiguration;
            mAppEnvironment          = aApplicationEnvironment;
            mDisplay                 = aDisplay;
            mFrameGraphRenderContext = aFrameGraphRenderContext;
            mGpuApiRenderContext     = aGpuApiRenderContext;

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
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::updateBuffer(GpuApiHandle_t const &aBufferId, ByteBuffer aData)
        {
            mGpuApiRenderContext->transferBufferData(aData, aBufferId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CRenderer::renderScene(RenderableList const &aRenderableCollection)
        {
            using namespace engine;
            using namespace engine::framegraph;

            SOSDisplayDescriptor const&displayDesc = mDisplay->screenInfo()[mDisplay->primaryScreenIndex()];

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            CGraphBuilder graphBuilder{ };
            graphBuilder.initialize(mAppEnvironment, mDisplay);
            graphBuilder.setGraphMode(CGraph::EGraphMode::Graphics);
            graphBuilder.setRenderToBackBuffer(true);


            SFrameGraphRenderableList renderables{ };
            renderables = graphBuilder.registerRenderables("SceneRenderables", aRenderableCollection);

            CFrameGraphModule<SGraphicsAPICommonModuleTag_t>                     graphicsAPICommonModule{ };
            CFrameGraphModule<SGBufferModuleTag_t>                               gbufferModule          { };
            CFrameGraphModule<SLightingModuleTag_t>                              lightingModule         { };
            CFrameGraphModule<SCompositingModuleTag_t>                           compositingModule      { };
            CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPrePassExportData prePassExportData{ };

            // static std::string const sPrePassID               = "PrePass";
            static std::string const sGBufferGenerationPassID = "GBufferGenerationPass";
            static std::string const sLightingPassID          = "LightingPass";
            static std::string const sCompositingPassID       = "CompositingPass";
            // static std::string const sPresentPassID           = "PresentPass";

            // Prepass
            // prePassExportData =
            //         graphicsAPICommonModule.addPrePass(
            //             sPrePassID,
            //             graphBuilder,
            //             width,
            //             height,
            //             FrameGraphFormat_t::R8G8B8A8_UNORM).data();

            // GBuffer
            CFrameGraphModule<SGBufferModuleTag_t>::SGBufferGenerationExportData gbufferExportData{ };
            gbufferExportData =
                    gbufferModule.addGBufferGenerationPass(
                        sGBufferGenerationPassID,
                        graphBuilder,
                        renderables).data();

            // Link SwapChain pass and GBuffer
            // graphBuilder.createPassDependency(sPrePassID, sGBufferGenerationPassID);

            // Lighting
            CFrameGraphModule<SLightingModuleTag_t>::SLightingExportData lightingExportData{ };
            lightingExportData =
                    lightingModule.addLightingPass(
                        sLightingPassID,
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3).data();

            // Compositing
            CFrameGraphModule<SCompositingModuleTag_t>::SExportData compositingExportData{ };
            compositingExportData =
                    compositingModule.addDefaultCompositingPass(
                        sCompositingPassID,
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3,
                        lightingExportData.lightAccumulationBuffer).data();

            graphBuilder.setOutputTextureResourceId(compositingExportData.output.resourceId);

            // Present
            // CFrameGraphModule<SGraphicsAPICommonModuleTag_t>::SPresentPassExportData presentPassExportData{};
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
                CLog::Error(logTag(), "Failed to compile the framegraph.");
                return EEngineStatus::Ok;
            }

            Unique<engine::framegraph::CGraph> frameGraph = std::move(compilation.data());

#if defined SHIRABE_FRAMEGRAPH_ENABLE_SERIALIZATION
            static bool serializedOnce = false;
            if(!serializedOnce)
            {
                serializedOnce = true;

                Shared<CFrameGraphGraphVizSerializer::IResult> result     = nullptr;
                Shared<CFrameGraphGraphVizSerializer>          serializer = std::make_shared<CFrameGraphGraphVizSerializer>();
                bool const initialized  = serializer->initialize();
                bool const serialized   = serializer->serialize(*frameGraph, result);


                Shared<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult> typedResult =
                        std::static_pointer_cast<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult>(result);

                std::string serializedData {};
                bool const dataFetched = typedResult->asString(serializedData);
                writeFile("FrameGraphTest.gv", serializedData);

                bool const deserialized = serializer->deinitialize();
                serializer = nullptr;

                system("tools/makeFrameGraphPNG.sh");
            }
#endif
            if(frameGraph)
            {
                frameGraph->execute(mFrameGraphRenderContext);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

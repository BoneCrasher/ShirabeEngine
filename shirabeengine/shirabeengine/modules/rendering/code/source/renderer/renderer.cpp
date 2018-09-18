#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/passbuilder.h"
#include "renderer/framegraph/modules/prepass.h"
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
                CStdSharedPtr_t<SApplicationEnvironment> const &aApplicationEnvironment,
                CStdSharedPtr_t<wsi::CWSIDisplay>        const &aDisplay,
                SRendererConfiguration                   const &aConfiguration,
                CStdSharedPtr_t<IFrameGraphRenderContext>      &aFrameGraphRenderContext)
        {
            assert(nullptr != aApplicationEnvironment);
            assert(nullptr != aDisplay);
            assert(nullptr != aFrameGraphRenderContext);

            mConfiguration           = aConfiguration;
            mAppEnvironment          = aApplicationEnvironment;
            mDisplay                 = aDisplay;
            mFrameGraphRenderContext = aFrameGraphRenderContext;

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
        EEngineStatus CRenderer::renderScene()
        {
            using namespace engine;
            using namespace engine::framegraph;

            SOSDisplayDescriptor const&displayDesc = mDisplay->screenInfo()[mDisplay->primaryScreenIndex()];

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            CGraphBuilder graphBuilder{ };
            graphBuilder.initialize(mAppEnvironment, mDisplay);

            SFrameGraphTexture backBufferTextureDesc{ };
            backBufferTextureDesc.width          = width;
            backBufferTextureDesc.height         = height;
            backBufferTextureDesc.depth          = 1;
            backBufferTextureDesc.format         = FrameGraphFormat_t::R8G8B8A8_UNORM;
            backBufferTextureDesc.initialState   = EFrameGraphResourceInitState::Clear;
            backBufferTextureDesc.arraySize      = 1;
            backBufferTextureDesc.mipLevels      = 1;
            backBufferTextureDesc.permittedUsage = EFrameGraphResourceUsage::RenderTarget | EFrameGraphResourceUsage::Unused;

            SFrameGraphResource backBuffer{ };
            backBuffer = graphBuilder.registerTexture("BackBuffer", backBufferTextureDesc);

            RenderableList renderableCollection ={
                { "Cube",    0, 0 },
                { "Sphere",  0, 0 },
                { "Pyramid", 0, 0 }
            };
            SFrameGraphResource renderables{ };
            renderables = graphBuilder.registerRenderables("SceneRenderables", renderableCollection);

            // Prepass
            CFrameGraphModule<SPrepassModuleTag_t>                     prepassModule    { };
            CFrameGraphModule<SPrepassModuleTag_t>::SPrepassExportData prepassExportData{ };
            prepassExportData = prepassModule.addPrepass(
                        graphBuilder,
                        backBuffer);

            SFrameGraphTexture const &tex = *graphBuilder.getResources().get<SFrameGraphTexture>(backBuffer.resourceId);

            // GBuffer
            CFrameGraphModule<SGBufferModuleTag_t>                               gbufferModule    { };
            CFrameGraphModule<SGBufferModuleTag_t>::SGBufferGenerationExportData gbufferExportData{ };
            gbufferExportData = gbufferModule.addGBufferGenerationPass(
                        graphBuilder,
                        renderables);

            // Lighting
            CFrameGraphModule<SLightingModuleTag_t>                      lightingModule    { };
            CFrameGraphModule<SLightingModuleTag_t>::SLightingExportData lightingExportData{ };
            lightingExportData = lightingModule.addLightingPass(
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3);

            // Compositing
            CFrameGraphModule<SCompositingModuleTag_t>              compositingModule    { };
            CFrameGraphModule<SCompositingModuleTag_t>::SExportData compositingExportData{ };
            compositingExportData = compositingModule.addDefaultCompositingPass(
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3,
                        lightingExportData.lightAccumulationBuffer,
                        prepassExportData.backbuffer);

            CStdUniquePtr_t<engine::framegraph::CGraph> frameGraph = graphBuilder.compile();

            static bool serializedOnce = false;
            if(!serializedOnce)
            {
                serializedOnce = true;

                CStdSharedPtr_t<CFrameGraphGraphVizSerializer::IResult> result     = nullptr;
                CStdSharedPtr_t<CFrameGraphGraphVizSerializer>          serializer = std::make_shared<CFrameGraphGraphVizSerializer>();
                bool const initialized  = serializer->initialize();
                bool const serialized   = serializer->serialize(*frameGraph, result);


                CStdSharedPtr_t<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult> typedResult =
                        std::static_pointer_cast<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult>(result);

                std::string serializedData {};
                bool const dataFetched = typedResult->asString(serializedData);
                writeFile("FrameGraphTest.gv", serializedData);

                bool const deserialized = serializer->deinitialize();
                serializer = nullptr;

                system("tools/makeFrameGraphPNG.sh");
            }

            // CRenderer will call.
            if(frameGraph)
                frameGraph->execute(mFrameGraphRenderContext);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

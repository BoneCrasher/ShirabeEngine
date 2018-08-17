#include "renderer/framegraph/graphbuilder.h"
#include "renderer/framegraph/passbuilder.h"
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
            , mAppEnvironment()
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
                SRendererConfiguration                    const &aConfiguration,
                CStdSharedPtr_t<IFrameGraphRenderContext>      &aFrameGraphRenderContext)
        {
            assert(nullptr != aApplicationEnvironment);
            assert(nullptr != aFrameGraphRenderContext);

            mConfiguration           = aConfiguration;
            mAppEnvironment          = aApplicationEnvironment;
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
        EEngineStatus CRenderer::renderScene()
        {
            using namespace engine;
            using namespace engine::framegraph;
            
            SOSDisplayDescriptor const &displayDesc = mAppEnvironment->primaryDisplay();

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            CGraphBuilder graphBuilder{ };
            graphBuilder.initialize(mAppEnvironment);

            SFrameGraphTexture backBufferTextureDesc{ };
            backBufferTextureDesc.width          = width;
            backBufferTextureDesc.height         = height;
            backBufferTextureDesc.depth          = 1;
            backBufferTextureDesc.format         = FrameGraphFormat_t::R8G8B8A8_UNORM;
            backBufferTextureDesc.initialState   = EFrameGraphResourceInitState::Clear;
            backBufferTextureDesc.arraySize      = 1;
            backBufferTextureDesc.mipLevels      = 1;
            backBufferTextureDesc.permittedUsage = EFrameGraphResourceUsage::RenderTarget;

            SFrameGraphResource backBuffer{ };
            backBuffer = graphBuilder.registerTexture("BackBuffer", backBufferTextureDesc);

            RenderableList renderableCollection ={
                { "Cube",    0, 0 },
                { "Sphere",  0, 0 },
                { "Pyramid", 0, 0 }
            };
            SFrameGraphResource renderables{ };
            renderables = graphBuilder.registerRenderables("SceneRenderables", renderableCollection);

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
                        backBuffer);

            CStdUniquePtr_t<engine::framegraph::CGraph> frameGraph = graphBuilder.compile();

            CStdSharedPtr_t<CFrameGraphGraphVizSerializer::IResult> result     = nullptr;
            CStdSharedPtr_t<CFrameGraphGraphVizSerializer>          serializer = std::make_shared<CFrameGraphGraphVizSerializer>();
            bool const initialized  = serializer->initialize();
            bool const serialized   = serializer->serialize(*frameGraph, result);
            bool const deserialized = serializer->deinitialize();
            serializer = nullptr;

            system("makeGraphPNG.bat");

            // CRenderer will call.
            if(frameGraph)
                frameGraph->execute(mFrameGraphRenderContext);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------
    }
}

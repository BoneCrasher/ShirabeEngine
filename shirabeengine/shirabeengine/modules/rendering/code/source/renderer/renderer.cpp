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
        EEngineStatus Renderer::deinitialize()
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
            using namespace engine::FrameGraph;
            
            OSDisplayDescriptor const &displayDesc = mAppEnvironment->primaryDisplay();

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            GraphBuilder graphBuilder{ };
            graphBuilder.initialize(mAppEnvironment);

            FrameGraphTexture backBufferTextureDesc{ };
            backBufferTextureDesc.width          = width;
            backBufferTextureDesc.height         = height;
            backBufferTextureDesc.depth          = 1;
            backBufferTextureDesc.format         = FrameGraphFormat::R8G8B8A8_UNORM;
            backBufferTextureDesc.initialState   = FrameGraphResourceInitState::Clear;
            backBufferTextureDesc.arraySize      = 1;
            backBufferTextureDesc.mipLevels      = 1;
            backBufferTextureDesc.permittedUsage = FrameGraphResourceUsage::RenderTarget;

            FrameGraphResource backBuffer{ };
            backBuffer = graphBuilder.registerTexture("BackBuffer", backBufferTextureDesc);

            RenderableList renderableCollection ={
                { "Cube",    0, 0 },
                { "Sphere",  0, 0 },
                { "Pyramid", 0, 0 }
            };
            FrameGraphResource renderables{ };
            renderables = graphBuilder.registerRenderables("SceneRenderables", renderableCollection);

            // GBuffer
            FrameGraphModule<GBufferModuleTag_t> gbufferModule{ };
            FrameGraphModule<GBufferModuleTag_t>::GBufferGenerationExportData gbufferExportData{ };
            gbufferExportData = gbufferModule.addGBufferGenerationPass(
                        graphBuilder,
                        renderables);

            // Lighting
            FrameGraphModule<LightingModuleTag_t> lightingModule{ };
            FrameGraphModule<LightingModuleTag_t>::LightingExportData lightingExportData{ };
            lightingExportData = lightingModule.addLightingPass(
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3);

            // Compositing
            FrameGraphModule<CompositingModuleTag_t> compositingModule{ };
            FrameGraphModule<CompositingModuleTag_t>::ExportData compositingExportData{ };
            compositingExportData = compositingModule.addDefaultCompositingPass(
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3,
                        lightingExportData.lightAccumulationBuffer,
                        backBuffer);

            UniqueCStdSharedPtr_t<engine::FrameGraph::Graph> frameGraph = graphBuilder.compile();

            CStdSharedPtr_t<FrameGraphGraphVizSerializer> serializer = std::make_shared<FrameGraphGraphVizSerializer>();
            serializer->initialize();

            frameGraph->acceptSerializer(serializer);
            serializer->writeToFile("FrameGraphTest");

            serializer->deinitialize();
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

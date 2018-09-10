#include <vector>

#include <core/enginetypehelper.h>
#include <asset/assetindex.h>
#include <asset/assetstorage.h>
#include <graphicsapi/resources/types/all.h>
#include <resources/core/resourcemanager.h>
#include <resources/core/resourceproxyfactory.h>


#include <renderer/irenderer.h>
#include <renderer/framegraph/graphbuilder.h>
#include <renderer/framegraph/passbuilder.h>
#include <renderer/framegraph/modules/gbuffergeneration.h>
#include <renderer/framegraph/modules/lighting.h>
#include <renderer/framegraph/modules/compositing.h>
#include <renderer/framegraph/framegraphrendercontext.h>
#include <renderer/framegraph/framegraphserialization.h>

#include "tests/test_framegraph.h"
#include "tests/test_framegraph_mocks.h"

namespace Test
{
    namespace FrameGraph
    {
        using namespace engine;
        using namespace engine::rendering;
        using namespace engine::framegraph;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool Test__FrameGraph::testAll()
        {
            bool ok = true;

            ok |= testGraphBuilder();

            return ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool Test__FrameGraph::testGraphBuilder()
        {
            using namespace engine;
            using namespace engine::core;
            using namespace engine::resources;
            using namespace engine::gfxapi;
            using namespace engine::framegraph;

            namespace asset = engine::asset;

            CStdSharedPtr_t<os::SApplicationEnvironment> appEnvironment = makeCStdSharedPtr<os::SApplicationEnvironment>();
            // appEnvironment->osDisplays = os::SOSDisplay::GetDisplays();

            //
            // ASSET STORAGE
            //
            asset::CAssetRegistry<asset::SAsset> registry = asset::CAssetIndex::loadIndexById("Default");

            CStdSharedPtr_t<asset::CAssetStorage> assetStorage = makeCStdSharedPtr<asset::CAssetStorage>();
            assetStorage->readIndex(registry);

            //
            // RESOURCE MANAGEMENT
            //
            CStdSharedPtr_t<CGFXAPIResourceBackend::ResourceTaskBackend_t> gfxApiResourceTaskBackend = makeCStdSharedPtr<CGFXAPIResourceTaskBackend>();

            CStdSharedPtr_t<CGFXAPIResourceBackend> gfxApiResourceBackend = makeCStdSharedPtr<CGFXAPIResourceBackend>();
            gfxApiResourceBackend->setResourceTaskBackend(gfxApiResourceTaskBackend);

            CStdSharedPtr_t<CResourceProxyFactory> resourceProxyFactory = makeCStdSharedPtr<CResourceProxyFactory>(gfxApiResourceBackend);
            CStdSharedPtr_t<CResourceManager>      proxyResourceManager = makeCStdSharedPtr<CResourceManager>(resourceProxyFactory);
            proxyResourceManager->setResourceBackend(gfxApiResourceBackend);

            CStdSharedPtr_t<IResourceManager> resourceManager = std::static_pointer_cast<IResourceManager>(proxyResourceManager);

            gfxApiResourceBackend->initialize();

            //
            // RENDERING
            //
            RendererConfiguration rendererConfiguration{};
            Ptr<IRenderContext> renderer = MakeSharedPointerType<MockRenderContext>();
            // renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
            //
            Ptr<IFrameGraphRenderContext> renderContext = FrameGraphRenderContext::create(assetStorage, resourceManager, renderer);

            OSDisplayDescriptor const&displayDesc = appEnvironment->primaryDisplay();

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            GraphBuilder graphBuilder{};
            graphBuilder.initialize(appEnvironment);

            FrameGraphTexture backBufferTextureDesc{};
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
            FrameGraphModule<GBufferModuleTag_t> gbufferModule{};
            FrameGraphModule<GBufferModuleTag_t>::GBufferGenerationExportData gbufferExportData{};
            gbufferExportData = gbufferModule.addGBufferGenerationPass(
                        graphBuilder,
                        renderables);

            // Lighting
            FrameGraphModule<LightingModuleTag_t> lightingModule{};
            FrameGraphModule<LightingModuleTag_t>::LightingExportData lightingExportData{};
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

            UniquePtr<Engine::FrameGraph::Graph> frameGraph = graphBuilder.compile();

            Ptr<FrameGraphGraphVizSerializer> serializer = std::make_shared<FrameGraphGraphVizSerializer>();
            serializer->initialize();

            frameGraph->acceptSerializer(serializer);
            serializer->writeToFile("FrameGraphTest");

            serializer->deinitialize();
            serializer = nullptr;

            system("makeGraphPNG.bat");

            // Renderer will call.
            if(frameGraph)
                frameGraph->execute(renderContext);

            gfxApiResourceBackend->deinitialize();

            return true;
        }

    }
}

#include <vector>

#include <Core/EngineTypeHelper.h>

#include <Asset/AssetIndex.h>
#include <Asset/AssetStorage.h>

#include <OS/OSDisplay.h>
#ifdef PLATFORM_WINDOWS
#include <WSI/Windows/WindowsDisplay.h>
using OSDisplayType = Engine::OS::OSDisplay<Engine::WSI::WinAPIDisplay>;
#endif


#include <Resources/Core/ResourceManager.h>
#include <Resources/Core/ResourceProxyFactory.h>
#include <GraphicsAPI/Resources/Types/All.h>
#include <GraphicsAPI/Resources/GFXAPIResourceProxy.h>

#include <Renderer/IRenderer.h>
#include <Renderer/FrameGraph/GraphBuilder.h>
#include <Renderer/FrameGraph/PassBuilder.h>
#include <Renderer/FrameGraph/Modules/GBufferGeneration.h>
#include <Renderer/FrameGraph/Modules/Lighting.h>
#include <Renderer/FrameGraph/Modules/Compositing.h>

#include <Renderer/FrameGraph/FrameGraphRenderContext.h>
#include <Renderer/FrameGraph/FrameGraphSerialization.h>

#include "Tests/Test_FrameGraph.h"
#include "Tests/Test_FrameGraph_Mocks.h"

namespace Test {
  namespace FrameGraph {
    using namespace Engine;
    using namespace Engine::OS;
    using namespace Engine::Rendering;
    using namespace Engine::FrameGraph;

    bool
      Test__FrameGraph::testAll()
    {
      bool ok = true;

      ok |= testGraphBuilder();

      return ok;
    }

    template <typename TResource>
    struct SpawnProxy {
      static ResourceProxyFactory::CreatorFn_t<TResource> forGFXAPIBackend(
        Ptr<GFXAPIResourceBackend> backend)
      {
        return
          [=]()
          -> ResourceProxyFactory::CreatorFn_t<TResource>
        {
          return
            [=](EProxyType const&type, typename TResource::CreationRequest const&request)
            -> Ptr<IResourceProxy<TResource>>
          {
            return MakeSharedPointerType<GFXAPIResourceProxy<TResource>>(type, backend, request);
          };
        }();
      }
    };

    bool
      Test__FrameGraph::testGraphBuilder()
    {
      using namespace Engine;
      using namespace Engine::Core;
      using namespace Engine::Resources;
      using namespace Engine::GFXAPI;
      using namespace Engine::FrameGraph;

      Ptr<OS::ApplicationEnvironment> appEnvironment = MakeSharedPointerType<OS::ApplicationEnvironment>();
      appEnvironment->instanceHandle         = 0;
      appEnvironment->previousInstanceHandle = 0;
      appEnvironment->osDisplays = OSDisplayType::GetDisplays(appEnvironment->primaryDisplayIndex);

      //
      // ASSET STORAGE
      // 
      namespace asset = Engine::Asset;
      asset::AssetRegistry<asset::Asset> registry = asset::AssetIndex::loadIndexById("Default");

      Ptr<asset::AssetStorage> assetStorage = MakeSharedPointerType<asset::AssetStorage>();
      assetStorage->readIndex(registry);

      //
      // RESOURCE MANAGEMENT
      // 
      Ptr<MockGFXAPITaskBackend> gfxApiResourceTaskBackend = MakeSharedPointerType<MockGFXAPITaskBackend>();
      Ptr<GFXAPIResourceBackend> gfxApiResourceBackend     = MakeSharedPointerType<GFXAPIResourceBackend>();
      Ptr<ResourceProxyFactory>  resourceProxyFactory      = MakeSharedPointerType<ResourceProxyFactory>();
      Ptr<ResourceManager>       proxyResourceManager      = MakeSharedPointerType<ResourceManager>(resourceProxyFactory);

      resourceProxyFactory->addCreator<Texture>(EResourceType::TEXTURE, SpawnProxy<Texture>::forGFXAPIBackend(gfxApiResourceBackend));
      resourceProxyFactory->addCreator<TextureView>(EResourceType::GAPI_VIEW, SpawnProxy<TextureView>::forGFXAPIBackend(gfxApiResourceBackend));

      gfxApiResourceTaskBackend->initialize();
      gfxApiResourceBackend->setResourceTaskBackend(gfxApiResourceTaskBackend);
      gfxApiResourceBackend->initialize();

      //
      // RENDERING
      // 
      RendererConfiguration rendererConfiguration{};
      Ptr<IRenderContext> renderer = MakeSharedPointerType<MockRenderContext>();
      // renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
      // 
      Ptr<IFrameGraphRenderContext> renderContext = FrameGraphRenderContext::create(assetStorage, proxyResourceManager, renderer);

      OSDisplayDescriptor const&displayDesc = appEnvironment->primaryDisplay();

      uint32_t
        width  = displayDesc.bounds.size.x(),
        height = displayDesc.bounds.size.y();

      std::function<void()> constructAndRunFrameGraph =
        [&] () -> void
      {
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

        gfxApiResourceBackend->registerResource("BackBuffer", Ptr<void>(new int()));

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
      };

      constructAndRunFrameGraph();
      constructAndRunFrameGraph(); // Run twice to check for resource persistency

      gfxApiResourceBackend->deinitialize();

      return true;
    }

  }
}
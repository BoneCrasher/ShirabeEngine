#include <vector>

#include <core/enginetypehelper.h>
#include <asset/assetindex.h>
#include <asset/assetstorage.h>
#include <graphicsapi/resources/types/all.h>
#include <graphicsapi/resources/gfxapiresourceproxy.h>
#include <resources/core/resourcemanagerbase.h>
#include <resources/core/resourceproxyfactory.h>


#include <renderer/irenderer.h>
#include <renderer/framegraph/framegraph.h>
#include <renderer/framegraph/graphbuilder.h>
#include <renderer/framegraph/passbuilder.h>
#include <renderer/framegraph/modules/gbuffergeneration.h>
#include <renderer/framegraph/modules/lighting.h>
#include <renderer/framegraph/modules/compositing.h>
#include <renderer/framegraph/framegraphrendercontext.h>
#include <renderer/framegraph/framegraphserialization.h>

#include <resource_management/resourcemanager.h>

#include <wsi/display.h>
#if defined SHIRABE_PLATFORM_LINUX
#include <wsi/x11/x11display.h>
#elif defined SHIRABE_PLATFORM_WINDOWS
#include <wsi/windows/windowsdisplay.h>
#endif

#include "tests/test_framegraph.h"
#include "tests/test_framegraph_mocks.h"

namespace Test
{
    namespace FrameGraph
    {
        using namespace engine;
        using namespace engine::rendering;
        using namespace engine::framegraph;
        using namespace engine::wsi;

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
        /**
         * Templated helper struct to spawn a typed gfxapi backend resource proxy creator functor.
         */
        template <typename TResource>
        struct SSpawnProxy
        {
            /**
             * Create and return a gfxapi resource backend proxy creation functor from TResource.
             *
             * @param aBackend The backend to create a proxy for.
             * @return         A valid creation functor for a proxy of type TResource.
             */
            static CResourceProxyFactory::CreatorFn_t<TResource> forGFXAPIBackend(CStdSharedPtr_t<CGFXAPIResourceBackend> aBackend)
            {
                auto const creator = [=]() -> CResourceProxyFactory::CreatorFn_t<TResource>
                {
                    return
                            [=](resources::EProxyType const &aType, typename TResource::CCreationRequest const &aRequest)
                            -> CStdSharedPtr_t<IResourceProxy<TResource>>
                    {
                        return makeCStdSharedPtr<CGFXAPIResourceProxy<TResource>>(aType, aRequest, aBackend);
                    };
                };

                return creator();
            }
        };
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
            // DISPLAY
            //
            CStdSharedPtr_t<CWSIDisplay> display = nullptr;
#if defined SHIRABE_PLATFORM_LINUX
            display = makeCStdSharedPtr<x11::CX11Display>();
#elif defined SHIRABE_PLATFORM_WINDOWS
#endif
            EEngineStatus status = display->initialize();

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

            CStdSharedPtr_t<CResourceProxyFactory> resourceProxyFactory = makeCStdSharedPtr<CResourceProxyFactory>();
            resourceProxyFactory->addCreator<CTexture>    (EResourceSubType::TEXTURE_2D,   SSpawnProxy<CTexture>::forGFXAPIBackend(gfxApiResourceBackend));
            resourceProxyFactory->addCreator<CTextureView>(EResourceSubType::TEXTURE_VIEW, SSpawnProxy<CTextureView>::forGFXAPIBackend(gfxApiResourceBackend));

            CStdSharedPtr_t<CResourceManagerBase>  proxyResourceManager = makeCStdSharedPtr<CResourceManager>(resourceProxyFactory);
            proxyResourceManager->initialize();
            gfxApiResourceBackend->initialize();

            //
            // RENDERING
            //
            SRendererConfiguration rendererConfiguration{};
            CStdSharedPtr_t<IRenderContext> renderer = makeCStdSharedPtr<CMockRenderContext>();
            // renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
            //
            CStdSharedPtr_t<IFrameGraphRenderContext> renderContext = CFrameGraphRenderContext::create(assetStorage, proxyResourceManager, renderer);

            SOSDisplayDescriptor const&displayDesc = display->screenInfo()[display->primaryScreenIndex()];

            uint32_t
                    width  = displayDesc.bounds.size.x(),
                    height = displayDesc.bounds.size.y();

            CGraphBuilder graphBuilder{};
            graphBuilder.initialize(appEnvironment, display);

            SFrameGraphTexture backBufferTextureDesc{};
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
            CFrameGraphModule<SGBufferModuleTag_t> gbufferModule{};
            CFrameGraphModule<SGBufferModuleTag_t>::SGBufferGenerationExportData gbufferExportData{};
            gbufferExportData = gbufferModule.addGBufferGenerationPass(
                        graphBuilder,
                        renderables);

            // Lighting
            CFrameGraphModule<SLightingModuleTag_t> lightingModule{};
            CFrameGraphModule<SLightingModuleTag_t>::SLightingExportData lightingExportData{};
            lightingExportData = lightingModule.addLightingPass(
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3);

            // Compositing
            CFrameGraphModule<SCompositingModuleTag_t> compositingModule{ };
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

            CStdSharedPtr_t<IFrameGraphSerializer> serializer = makeCStdSharedPtr<CFrameGraphGraphVizSerializer>();
            serializer->initialize();

            CStdSharedPtr_t<ISerializer<CGraph>::IResult> result = nullptr;

            bool const serialized = serializer->serialize(*frameGraph, result);

            CStdSharedPtr_t<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult> typedResult =
                    std::static_pointer_cast<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult>(result);



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

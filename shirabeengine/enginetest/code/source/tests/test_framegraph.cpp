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
            static CResourceProxyFactory::CreatorFn_t<TResource> forGFXAPIBackend(Shared<CGFXAPIResourceBackend> aBackend)
            {
                auto const creator = [=]() -> CResourceProxyFactory::CreatorFn_t<TResource>
                {
                    return
                            [=](resources::EProxyType const &aType, typename TResource::CCreationRequest const &aRequest)
                            -> Shared<IResourceProxy<TResource>>
                    {
                        return makeShared<CGFXAPIResourceProxy<TResource>>(aType, aRequest, aBackend);
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

            Shared<os::SApplicationEnvironment> appEnvironment = makeShared<os::SApplicationEnvironment>();
            // appEnvironment->osDisplays = os::SOSDisplay::GetDisplays();

            //
            // DISPLAY
            //
            Shared<CWSIDisplay> display = nullptr;
#if defined SHIRABE_PLATFORM_LINUX
            display = makeShared<x11::CX11Display>();
#elif defined SHIRABE_PLATFORM_WINDOWS
#endif
            EEngineStatus status = display->initialize();

            //
            // ASSET STORAGE
            //
            asset::CAssetRegistry<asset::SAsset> registry = asset::CAssetIndex::loadIndexById("Default");

            Shared<asset::CAssetStorage> assetStorage = makeShared<asset::CAssetStorage>();
            assetStorage->readIndex(registry);

            //
            // RESOURCE MANAGEMENT
            //
            Shared<CGFXAPIResourceBackend::ResourceTaskBackend_t> gfxApiResourceTaskBackend = makeShared<CMockGFXAPIResourceTaskBackend>();
            gfxApiResourceTaskBackend->initialize();

            Shared<CGFXAPIResourceBackend> gfxApiResourceBackend = makeShared<CGFXAPIResourceBackend>();
            gfxApiResourceBackend->setResourceTaskBackend(gfxApiResourceTaskBackend);

            Shared<CResourceProxyFactory> resourceProxyFactory = makeShared<CResourceProxyFactory>();
            resourceProxyFactory->addCreator<CTexture>    (EResourceSubType::TEXTURE_2D,   SSpawnProxy<CTexture>::forGFXAPIBackend(gfxApiResourceBackend));
            resourceProxyFactory->addCreator<CTextureView>(EResourceSubType::TEXTURE_VIEW, SSpawnProxy<CTextureView>::forGFXAPIBackend(gfxApiResourceBackend));

            Shared<CResourceManagerBase>  proxyResourceManager = makeShared<CResourceManager>(resourceProxyFactory);
            proxyResourceManager->initialize();
            gfxApiResourceBackend->initialize();

            //
            // RENDERING
            //
            SRendererConfiguration rendererConfiguration{};
            Shared<IRenderContext> renderer = makeShared<CMockRenderContext>();
            // renderer->initialize(*appEnvironment, rendererConfiguration, nullptr);
            //
            Shared<IFrameGraphRenderContext> renderContext = CFrameGraphRenderContext::create(assetStorage, proxyResourceManager, renderer).data();

            SOSDisplayDescriptor const &displayDesc = display->screenInfo()[display->primaryScreenIndex()];

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
            backBufferTextureDesc.permittedUsage = EFrameGraphResourceUsage::ColorAttachment;

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
                        "GBufferGeneration",
                        graphBuilder,
                        renderables).data();

            // Lighting
            CFrameGraphModule<SLightingModuleTag_t> lightingModule{};
            CFrameGraphModule<SLightingModuleTag_t>::SLightingExportData lightingExportData{};
            lightingExportData = lightingModule.addLightingPass(
                        "Lighting",
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3).data();

            // Compositing
            CFrameGraphModule<SCompositingModuleTag_t> compositingModule{ };
            CFrameGraphModule<SCompositingModuleTag_t>::SExportData compositingExportData{ };
            compositingExportData = compositingModule.addDefaultCompositingPass(
                        "Compositing",
                        graphBuilder,
                        gbufferExportData.gbuffer0,
                        gbufferExportData.gbuffer1,
                        gbufferExportData.gbuffer2,
                        gbufferExportData.gbuffer3,
                        lightingExportData.lightAccumulationBuffer).data();

            CEngineResult<Unique<engine::framegraph::CGraph>> frameGraph = graphBuilder.compile();

            // Shared<IFrameGraphSerializer> serializer = makeShared<CFrameGraphGraphVizSerializer>();
            // serializer->initialize();

            // Shared<ISerializer<CGraph>::IResult> result = nullptr;
            //
            // bool const serialized = serializer->serialize(*frameGraph, result);
            //
            // Shared<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult> typedResult =
            //         std::static_pointer_cast<CFrameGraphGraphVizSerializer::CFrameGraphSerializationResult>(result);
            //
            // std::string serializedData {};
            // bool const dataFetched = typedResult->asString(serializedData);
            // writeFile("FrameGraphTest.gv", serializedData);
            //
            // serializer->deinitialize();
            // serializer = nullptr;
            //
            // system("tools/makeFrameGraphPNG.sh");
            //
            // // Renderer will call.
            // if(frameGraph)
            //     frameGraph->execute(renderContext);

            gfxApiResourceTaskBackend->deinitialize();
            proxyResourceManager->deinitialize();
            gfxApiResourceBackend->deinitialize();

            return true;
        }

    }
}

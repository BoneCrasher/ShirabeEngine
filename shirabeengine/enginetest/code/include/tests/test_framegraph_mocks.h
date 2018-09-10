#ifndef __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__
#define __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__

#include <log/log.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include <graphicsapi/resources/types/all.h>
#include <graphicsapi/resources/gfxapiresourcebackend.h>
#include <graphicsapi/resources/gfxapiresourcetaskbackend.h>
#include <resources/core/resourcemanagerbase.h>
#include <resources/core/resourceproxyfactory.h>
#include <renderer/irenderer.h>
#include <renderer/framegraph/graphbuilder.h>
#include <renderer/framegraph/passbuilder.h>
#include <renderer/framegraph/modules/gbuffergeneration.h>
#include <renderer/framegraph/modules/lighting.h>
#include <renderer/framegraph/modules/compositing.h>
#include <renderer/framegraph/framegraphrendercontext.h>
#include <renderer/framegraph/framegraphserialization.h>

namespace Test
{
    namespace FrameGraph
    {
        using namespace engine;
        using namespace engine::rendering;
        using namespace engine::framegraph;

        /**
         * Mock implementation of a render context.
         */
        class CMockRenderContext
                : public IRenderContext
        {
            SHIRABE_DECLARE_LOG_TAG(MockRenderContext)

        public_methods:
            /**
             * Bind a resource to the pipeline.
             *
             * @param aResourceId The id of the resource to bind.
             * @return            EEngineStatus::Ok, if successful. Error code otherwise.
             */
            EEngineStatus bindResource(PublicResourceId_t const &aResourceId);

            /**
             * Unbind a resource from the pipeline.
             *
             * @param aResourceId The id of the resource to unbind.
             * @return            EEngineStatus::Ok, if successful. Error code otherwise.
             */
            EEngineStatus unbindResource(PublicResourceId_t const &aResourceId);

            /**
             * Render a renderable using the currently configured pipeline.
             *
             * @param aRenderable The renderable to render.
             * @return            EEngineStatus::Ok, if successful. Error code otherwise.
             */
            EEngineStatus render(SRenderable const &aRenderable);
        };

        /**
         * Mock implementation of a render context to interface with a dummy render context.
         */
        class CMockFrameGraphRenderContext
                : public IFrameGraphRenderContext
        {
            SHIRABE_DECLARE_LOG_TAG(CMockFrameGraphRenderContext);
        public:
            static CStdSharedPtr_t<IFrameGraphRenderContext> fromRenderer(CStdSharedPtr_t<IRenderContext> renderer);

            EEngineStatus importTexture(SFrameGraphTexture const &aTexture);

            EEngineStatus createTexture(SFrameGraphTexture const &aTexture);

            EEngineStatus createTextureView(
                    SFrameGraphTexture      const &aTexture,
                    SFrameGraphTextureView  const &aView);

            EEngineStatus createBuffer(
                    FrameGraphResourceId_t const &aResourceId,
                    SFrameGraphResource    const &aResource,
                    SFrameGraphBuffer      const &aBuffer);

            EEngineStatus createBufferView(
                    FrameGraphResourceId_t const &aResourceId,
                    SFrameGraphResource    const &aResource,
                    SFrameGraphBufferView  const &aView);

            EEngineStatus loadTextureAsset(AssetId_t const &aAssetId);

            EEngineStatus loadBufferAsset(AssetId_t const &aAssetId);

            EEngineStatus loadMeshAsset(AssetId_t const &aAssetId);

            EEngineStatus bindTextureView(SFrameGraphTextureView const &aView);

            EEngineStatus bindBufferView(FrameGraphResourceId_t const &aResourceId);

            EEngineStatus bindMesh(AssetId_t const &aMesh);

            EEngineStatus unbindTextureView(SFrameGraphTextureView const &aView);

            EEngineStatus unbindBufferView(FrameGraphResourceId_t const &aResourceId);

            EEngineStatus unbindMesh(AssetId_t const &aAssetId);

            EEngineStatus unloadTextureAsset(AssetId_t const &aAssetId);

            EEngineStatus unloadBufferAsset(AssetId_t const &aAssetId);

            EEngineStatus unloadMeshAsset(AssetId_t  const &aAssetId);

            EEngineStatus destroyTexture(SFrameGraphTexture const &aTexture);

            EEngineStatus destroyTextureView(SFrameGraphTextureView const &aView);

            EEngineStatus destroyBuffer(FrameGraphResourceId_t      const &aResourceId);

            EEngineStatus destroyBufferView(FrameGraphResourceId_t  const &aResourceId);

            EEngineStatus render(SRenderable const&renderable);

        private:
            CMockFrameGraphRenderContext(CStdSharedPtr_t<IRenderContext> aRenderer);

            CStdSharedPtr_t<IRenderContext> mRenderer;
        };
    }
}

#endif

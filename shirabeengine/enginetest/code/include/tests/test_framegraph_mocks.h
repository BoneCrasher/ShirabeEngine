#ifndef __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__
#define __SHIRABE_TEST_FRAMEGRAPH_MOCKS_H__

#include <log/log.h>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
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
    namespace RenderGraph
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
        class CMockRenderGraphRenderContext
                : public IRenderGraphRenderContext
        {
            SHIRABE_DECLARE_LOG_TAG(CMockRenderGraphRenderContext);
        public:
            static Shared<IRenderGraphRenderContext> fromRenderer(Shared<IRenderContext> renderer);

            CEngineResult<> importTexture(SRenderGraphTexture const &aTexture);

            CEngineResult<> createTexture(SRenderGraphTexture const &aTexture);

            CEngineResult<> createTextureView(
                    SRenderGraphTexture      const &aTexture,
                    SRenderGraphTextureView  const &aView);

            CEngineResult<> createBuffer(
                    RenderGraphResourceId_t const &aResourceId,
                    SRenderGraphResource    const &aResource,
                    SRenderGraphBuffer      const &aBuffer);

            CEngineResult<> createBufferView(
                    RenderGraphResourceId_t const &aResourceId,
                    SRenderGraphResource    const &aResource,
                    SRenderGraphBufferView  const &aView);

            CEngineResult<> loadTextureAsset(AssetId_t const &aAssetId);

            CEngineResult<> loadBufferAsset(AssetId_t const &aAssetId);

            CEngineResult<> loadMeshAsset(AssetId_t const &aAssetId);

            CEngineResult<> bindTextureView(SRenderGraphTextureView const &aView);

            CEngineResult<> bindBufferView(RenderGraphResourceId_t const &aResourceId);

            CEngineResult<> bindMesh(AssetId_t const &aMesh);

            CEngineResult<> unbindTextureView(SRenderGraphTextureView const &aView);

            CEngineResult<> unbindBufferView(RenderGraphResourceId_t const &aResourceId);

            CEngineResult<> unbindMesh(AssetId_t const &aAssetId);

            CEngineResult<> unloadTextureAsset(AssetId_t const &aAssetId);

            CEngineResult<> unloadBufferAsset(AssetId_t const &aAssetId);

            CEngineResult<> unloadMeshAsset(AssetId_t  const &aAssetId);

            CEngineResult<> destroyTexture(SRenderGraphTexture const &aTexture);

            CEngineResult<> destroyTextureView(SRenderGraphTextureView const &aView);

            CEngineResult<> destroyBuffer(RenderGraphResourceId_t      const &aResourceId);

            CEngineResult<> destroyBufferView(RenderGraphResourceId_t  const &aResourceId);

            CEngineResult<> render(SRenderable const&renderable);

        private:
            CMockRenderGraphRenderContext(Shared<IRenderContext> aRenderer);

            Shared<IRenderContext> mRenderer;
        };

#define SHIRABE_DECLARE_MOCK_TASK_BUILDER_MODULE(type)                         \
        EEngineStatus fn##type##CreationTask(                                  \
                            C##type::CCreationRequest      const &aRequest,    \
                            ResolvedDependencyCollection_t const &aDepencies,  \
                            ResourceTaskFn_t                     &aOutTask);   \
        EEngineStatus fn##type##UpdateTask(                                    \
                            C##type::CUpdateRequest         const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResolvedDependencyCollection_t  const &aDepencies, \
                            ResourceTaskFn_t                      &aOutTask);  \
        EEngineStatus fn##type##DestructionTask(                               \
                            C##type::CDestructionRequest    const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResolvedDependencyCollection_t  const &aDepencies, \
                            ResourceTaskFn_t                      &aOutTask);  \
        EEngineStatus fn##type##QueryTask(                                     \
                            C##type::CQuery                 const &aRequest,   \
                            SGFXAPIResourceHandleAssignment const &aAssignment,\
                            ResourceTaskFn_t                      &aOutTask);


        /**
         * Mock implementation of a resource task backend.
         */
        class CMockGFXAPIResourceTaskBackend
                : public CGFXAPIResourceTaskBackend
        {
            SHIRABE_DECLARE_LOG_TAG(CMockGFXAPIResourceTaskBackend)

        public_methods:
            CEngineResult<> initialize();

            CEngineResult<> deinitialize();

            SHIRABE_DECLARE_MOCK_TASK_BUILDER_MODULE(Texture)
            SHIRABE_DECLARE_MOCK_TASK_BUILDER_MODULE(TextureView)
        };
    }
}

#endif

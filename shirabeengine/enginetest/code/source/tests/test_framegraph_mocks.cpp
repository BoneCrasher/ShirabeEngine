#include "tests/test_framegraph_mocks.h"

namespace Test
{
    namespace RenderGraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderContext::bindResource(PublicResourceId_t const &aResourceId)
        {
            CLog::Verbose(logTag(), CString::format("bindResource(ID: {});", aResourceId));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderContext::unbindResource(PublicResourceId_t const &aResourceId)
        {
            CLog::Verbose(logTag(), CString::format("unbindResource(ID: {});", aResourceId));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> MockRenderContext::render(SRenderable const &aRenderable)
        {
            std::string message =
                    CString::format(
                        "operation -> render(Renderable const&):\n"
                        "Renderable: {}\n"
                        "  MeshId:     {}\n"
                        "  MaterialId: {}\n",
                        aRenderable.name,
                        aRenderable.meshId,
                        aRenderable.materialId);
            CLog::Verbose(logTag(), message);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMockRenderGraphRenderContext::CMockRenderGraphRenderContext(Shared<IRenderContext> aRenderer)
            : mRenderer(aRenderer)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Shared<IRenderGraphRenderContext> CMockRenderGraphRenderContext::fromRenderer(Shared<IRenderContext> aRenderer)
        {
            assert(nullptr != aRenderer);

            Shared<RenderGraph::IRenderGraphRenderContext> context =
                    Shared<CMockRenderGraphRenderContext>(new CMockRenderGraphRenderContext(aRenderer));
            if(!context)
            {
                CLog::Error(logTag(), "Failed to create render context from renderer.");
            }

            return context;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::importTexture(SRenderGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("ImportTexture(...):\n{}", convert_to_string(aTexture)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::createTexture(SRenderGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("CreateTexture(...):\n{}", convert_to_string(aTexture)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::createTextureView(
                SRenderGraphTexture      const &aTexture,
                SRenderGraphTextureView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("CreateTextureView(...):\n{}", convert_to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::createBuffer(
                RenderGraphResourceId_t const &aResourceId,
                SRenderGraphResource    const &aResource,
                SRenderGraphBuffer      const &aBuffer)
        {
            CLog::Verbose(logTag(), CString::format("CreateBuffer(...):\n{}", convert_to_string(aBuffer)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::createBufferView(
                RenderGraphResourceId_t const &aResourceId,
                SRenderGraphResource    const &aResource,
                SRenderGraphBufferView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("CreateBufferView(...):\n{}", convert_to_string(aView)));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::loadTextureAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::loadBufferAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::loadMeshAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::bindTextureView(SRenderGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("BindTextureView(...):\n{}", convert_to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::bindBufferView(RenderGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::bindMesh(AssetId_t const &aMesh)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::unbindTextureView(SRenderGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("UnbindTextureView(...):\n{}", convert_to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::unbindBufferView(RenderGraphResourceId_t const &aResourceid)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::unbindMesh(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::unloadTextureAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::unloadBufferAsset(AssetId_t  const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::unloadMeshAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::destroyTexture(SRenderGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("DestroyTexture(...):\n{}", convert_to_string(aTexture)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::destroyTextureView(SRenderGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("DestroyTextureView(...):\n{}", convert_to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::destroyBuffer(RenderGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::destroyBufferView(RenderGraphResourceId_t  const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockRenderGraphRenderContext::render(SRenderable const &aRenderable)
        {
            CLog::Verbose(logTag(), CString::format("Render(...):\n", convert_to_string(aRenderable)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
#define SHIRABE_IMPLEMENT_TASKS_FOR(type)                                   \
        addCreator<C##type>(                                                    \
                std::bind(                                                      \
                        &CMockGFXAPIResourceTaskBackend::fn##type##CreationTask,    \
                        this,                                                   \
                        std::placeholders::_1,                                  \
                        std::placeholders::_2,                                  \
                        std::placeholders::_3));                                \
        addUpdater<C##type>(                                                    \
                std::bind(                                                      \
                        &CMockGFXAPIResourceTaskBackend::fn##type##UpdateTask,      \
                        this,                                                   \
                        std::placeholders::_1,                                  \
                        std::placeholders::_2,                                  \
                        std::placeholders::_3,                                  \
                        std::placeholders::_4));                                \
        addDestructor<C##type>(                                                 \
                std::bind(                                                      \
                        &CMockGFXAPIResourceTaskBackend::fn##type##DestructionTask, \
                        this,                                                   \
                        std::placeholders::_1,                                  \
                        std::placeholders::_2,                                  \
                        std::placeholders::_3,                                  \
                        std::placeholders::_4));                                \
        addQuery<C##type>(                                                      \
                std::bind(                                                      \
                        &CMockGFXAPIResourceTaskBackend::fn##type##QueryTask,       \
                        this,                                                   \
                        std::placeholders::_1,                                  \
                        std::placeholders::_2,                                  \
                        std::placeholders::_3));
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::initialize()
        {
            SHIRABE_IMPLEMENT_TASKS_FOR(Texture);
            SHIRABE_IMPLEMENT_TASKS_FOR(TextureView);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::deinitialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureCreationTask(
                CTexture::CCreationRequest     const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CTexture::SDescriptor const &descriptor = aRequest.resourceDescriptor();
                CLog::Debug(logTag(), CString::format("Creating texture:\n{}", descriptor.toString()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureUpdateTask(
                CTexture::CUpdateRequest        const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Updating texture:\n{}", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureDestructionTask(
                CTexture::CDestructionRequest   const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Destroying texture:\n{}", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureQueryTask(
                CTexture::CQuery                const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Querying texture:\n{}", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureViewCreationTask(
                CTextureView::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CTextureView::SDescriptor const &descriptor = aRequest.resourceDescriptor();
                CLog::Debug(logTag(), CString::format("Creating textureview:\n{}", descriptor.toString()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureViewUpdateTask(
                CTextureView::CUpdateRequest    const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Updating textureview:\n{}", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureViewDestructionTask(
                CTextureView::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment   const &aAssignment,
                ResolvedDependencyCollection_t    const &aDepencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Destroying textureview:\n{}", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CEngineResult<> CMockGFXAPIResourceTaskBackend::fnTextureViewQueryTask(
                CTextureView::CQuery            const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Querying textureview:\n{}", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeShared<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}

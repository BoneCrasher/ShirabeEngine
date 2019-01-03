#include "tests/test_framegraph_mocks.h"

namespace Test
{
    namespace FrameGraph
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockRenderContext::bindResource(PublicResourceId_t const &aResourceId)
        {
            CLog::Verbose(logTag(), CString::format("bindResource(ID: %0);", aResourceId));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockRenderContext::unbindResource(PublicResourceId_t const &aResourceId)
        {
            CLog::Verbose(logTag(), CString::format("unbindResource(ID: %0);", aResourceId));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockRenderContext::render(SRenderable const &aRenderable)
        {
            std::string message =
                    CString::format(
                        "operation -> render(Renderable const&):\n"
                        "Renderable: %0\n"
                        "  MeshId:     %1\n"
                        "  MaterialId: %2\n",
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
        CMockFrameGraphRenderContext::CMockFrameGraphRenderContext(CStdSharedPtr_t<IRenderContext> aRenderer)
            : mRenderer(aRenderer)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<IFrameGraphRenderContext> CMockFrameGraphRenderContext::fromRenderer(CStdSharedPtr_t<IRenderContext> aRenderer)
        {
            assert(nullptr != aRenderer);

            CStdSharedPtr_t<FrameGraph::IFrameGraphRenderContext> context =
                    CStdSharedPtr_t<CMockFrameGraphRenderContext>(new CMockFrameGraphRenderContext(aRenderer));
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
        EEngineStatus CMockFrameGraphRenderContext::importTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("ImportTexture(...):\n%0", to_string(aTexture)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::createTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("CreateTexture(...):\n%0", to_string(aTexture)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::createTextureView(
                SFrameGraphTexture      const &aTexture,
                SFrameGraphTextureView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("CreateTextureView(...):\n%0", to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::createBuffer(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBuffer      const &aBuffer)
        {
            CLog::Verbose(logTag(), CString::format("CreateBuffer(...):\n%0", to_string(aBuffer)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::createBufferView(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBufferView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("CreateBufferView(...):\n%0", to_string(aView)));
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::loadTextureAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::loadBufferAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::loadMeshAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::bindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("BindTextureView(...):\n%0", to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::bindMesh(AssetId_t const &aMesh)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::unbindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("UnbindTextureView(...):\n%0", to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t const &aResourceid)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::unbindMesh(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::unloadTextureAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::unloadBufferAsset(AssetId_t  const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::unloadMeshAsset(AssetId_t const &aAssetId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::destroyTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("DestroyTexture(...):\n%0", to_string(aTexture)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::destroyTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("DestroyTextureView(...):\n%0", to_string(aView)));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t  const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockFrameGraphRenderContext::render(SRenderable const &aRenderable)
        {
            CLog::Verbose(logTag(), CString::format("Render(...):\n", to_string(aRenderable)));

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
        EEngineStatus CMockGFXAPIResourceTaskBackend::initialize()
        {
            SHIRABE_IMPLEMENT_TASKS_FOR(Texture);
            SHIRABE_IMPLEMENT_TASKS_FOR(TextureView);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::deinitialize()
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
                CLog::Debug(logTag(), CString::format("Creating texture:\n%0", descriptor.toString()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureUpdateTask(
                CTexture::CUpdateRequest        const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Updating texture:\n%0", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureDestructionTask(
                CTexture::CDestructionRequest   const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Destroying texture:\n%0", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureQueryTask(
                CTexture::CQuery                const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Querying texture:\n%0", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureViewCreationTask(
                CTextureView::CCreationRequest const &aRequest,
                ResolvedDependencyCollection_t const &aDepencies,
                ResourceTaskFn_t                     &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CTextureView::SDescriptor const &descriptor = aRequest.resourceDescriptor();
                CLog::Debug(logTag(), CString::format("Creating textureview:\n%0", descriptor.toString()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureViewUpdateTask(
                CTextureView::CUpdateRequest    const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResolvedDependencyCollection_t  const &aDepencies,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Updating textureview:\n%0", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureViewDestructionTask(
                CTextureView::CDestructionRequest const &aRequest,
                SGFXAPIResourceHandleAssignment   const &aAssignment,
                ResolvedDependencyCollection_t    const &aDepencies,
                ResourceTaskFn_t                        &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Destroying textureview:\n%0", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CMockGFXAPIResourceTaskBackend::fnTextureViewQueryTask(
                CTextureView::CQuery            const &aRequest,
                SGFXAPIResourceHandleAssignment const &aAssignment,
                ResourceTaskFn_t                      &aOutTask)
        {
            EEngineStatus status = EEngineStatus::Ok;

            aOutTask = [=] () -> SGFXAPIResourceHandleAssignment
            {
                CLog::Debug(logTag(), CString::format("Querying textureview:\n%0", aRequest.publicResourceId()));

                SGFXAPIResourceHandleAssignment assignment ={ };
                assignment.publicResourceHandle   = 1;
                assignment.internalResourceHandle = std::static_pointer_cast<void>(makeCStdSharedPtr<int>());
                return assignment;
            };

            return status;
        }
        //<-----------------------------------------------------------------------------
    }
}
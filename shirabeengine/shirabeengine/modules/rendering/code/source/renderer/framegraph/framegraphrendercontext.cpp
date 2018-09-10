#include <assert.h>

#include "renderer/irenderer.h"
#include "renderer/framegraph/framegraphrendercontext.h"

namespace engine
{
    namespace framegraph
    {
        using namespace engine::rendering;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<CFrameGraphRenderContext> CFrameGraphRenderContext::create(
                CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
        {
            assert(aAssetStorage    != nullptr);
            assert(aResourceManager != nullptr);
            assert(aRenderer        != nullptr);

            CStdSharedPtr_t<CFrameGraphRenderContext> context = nullptr;
            context = CStdSharedPtr_t<CFrameGraphRenderContext>(new CFrameGraphRenderContext(aAssetStorage, aResourceManager, aRenderer));
            if(!context)
                CLog::Error(logTag(), "Failed to create render context from renderer and resourcemanager.");

            return context;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CFrameGraphRenderContext::CFrameGraphRenderContext(
                CStdSharedPtr_t<IAssetStorage>        aAssetStorage,
                CStdSharedPtr_t<CResourceManagerBase> aResourceManager,
                CStdSharedPtr_t<IRenderContext>       aRenderer)
            : mAssetStorage(aAssetStorage)
            , mResourceManager(aResourceManager)
            , mGraphicsAPIRenderContext(aRenderer)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        void CFrameGraphRenderContext::mapFrameGraphToInternalResource(
                std::string        const &aName,
                PublicResourceId_t const &aResourceId)
        {
            mResourceMap[aName].push_back(aResourceId);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        Vector<PublicResourceId_t> CFrameGraphRenderContext::getMappedInternalResourceIds(std::string const &aName) const
        {
            if(mResourceMap.find(aName) == mResourceMap.end())
                return {};

            return mResourceMap.at(aName);
        }

        void CFrameGraphRenderContext::removeMappedInternalResourceIds(std::string const &aName)
        {
            mResourceMap.erase(aName);
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::importTexture(SFrameGraphTexture const &aTexture)
        {
            PublicResourceId_t const pid = "";

            mapFrameGraphToInternalResource(aTexture.readableName, pid);

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createTexture(SFrameGraphTexture const &aTexture)
        {
            CTexture::SDescriptor desc = {};
            desc.name        = aTexture.readableName;
            desc.textureInfo = aTexture;

            if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::RenderTarget))
                desc.gpuBinding.set(EBufferBinding::ColorAttachement);
            if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::DepthTarget))
                desc.gpuBinding.set(EBufferBinding::DepthAttachement);
            if(aTexture.requestedUsage.check(EFrameGraphResourceUsage::ImageResource))
                desc.gpuBinding.set(EBufferBinding::InputAttachement);

            desc.cpuGpuUsage = EResourceUsage::CPU_None_GPU_ReadWrite;

            CTexture::CCreationRequest const request(desc);

            CLog::Verbose(logTag(), CString::format("Texture:\n%0", to_string(aTexture)));

            EEngineStatus status = mResourceManager->createResource<CTexture>(request, aTexture.readableName, false);
            if(EEngineStatus::ResourceManager_ResourceAlreadyCreated == status)
                return EEngineStatus::Ok;
            else
                HandleEngineStatusError(status, "Failed to create texture.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createTextureView(
                SFrameGraphTexture     const &aTexture,
                SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            CTextureView::SDescriptor desc = { };
            desc.name             = aView.readableName;
            desc.textureFormat    = aView.format;
            desc.subjacentTexture = aTexture;
            desc.arraySlices      = aView.arraySliceRange;
            desc.mipMapSlices     = aView.mipSliceRange;
            desc.dependencies.push_back(aTexture.readableName);

            CTextureView::CCreationRequest const request(desc, aTexture.readableName);

            EEngineStatus status = mResourceManager->createResource<CTextureView>(request, aView.readableName, false);
            HandleEngineStatusError(status, "Failed to create texture.");

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createBuffer(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBuffer      const &aBuffer)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::createBufferView(
                FrameGraphResourceId_t const &aResourceId,
                SFrameGraphResource    const &aResource,
                SFrameGraphBufferView  const &aBufferView)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::loadTextureAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::loadBufferAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::loadMeshAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            Vector<PublicResourceId_t> const &subjacentResources = getMappedInternalResourceIds(aView.readableName);

            for(PublicResourceId_t const&pid : subjacentResources)
            {
                mGraphicsAPIRenderContext->bindResource(pid);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindBufferView(FrameGraphResourceId_t  const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::bindMesh(AssetId_t const&aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unbindTextureView(SFrameGraphTextureView const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            Vector<PublicResourceId_t> const&subjacentResources = getMappedInternalResourceIds(aView.readableName);

            for(PublicResourceId_t const&pid : subjacentResources)
            {
                mGraphicsAPIRenderContext->unbindResource(pid);
            }

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unbindBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unbindMesh(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unloadTextureAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unloadBufferAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::unloadMeshAsset(AssetId_t const &aAssetUID)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyTexture(SFrameGraphTexture const &aTexture)
        {
            CLog::Verbose(logTag(), CString::format("Texture:\n%0", to_string(aTexture)));

            EEngineStatus status = mResourceManager->destroyResource<CTexture>(aTexture.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyTextureView(SFrameGraphTextureView  const &aView)
        {
            CLog::Verbose(logTag(), CString::format("TextureView:\n%0", to_string(aView)));

            EEngineStatus status = EEngineStatus::Ok;
            status = mResourceManager->destroyResource<CTextureView>(aView.readableName);

            return status;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyBuffer(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::destroyBufferView(FrameGraphResourceId_t const &aResourceId)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CFrameGraphRenderContext::render(SRenderable const &aRenderable)
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}

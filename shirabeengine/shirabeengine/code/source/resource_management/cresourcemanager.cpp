#include "resource_management/resourcemanager.h"
#include "graphicsapi/resources/types/all.h"

namespace engine
{
    using engine::gfxapi::CTexture;
    using engine::gfxapi::CTextureView;
    using engine::gfxapi::CRenderPass;
    using engine::gfxapi::CFrameBuffer;
    using engine::gfxapi::CPipeline;
    using engine::gfxapi::CMesh;

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CResourceManager::CResourceManager(CStdSharedPtr_t<CResourceProxyFactory> const &aProxyFactory)
        : CResourceManagerBase(aProxyFactory)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CResourceManager::~CResourceManager()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CResourceManager::initialize()
    {
        CResourceManagerBase *resourceManager = static_cast<CResourceManagerBase *>(this);

        bool success = true;

        success = success && addCreator   <CTexture>    (resourceManager);
        success = success && addLoader    <CTexture>    (resourceManager);
        success = success && addUpdater   <CTexture>    (resourceManager);
        success = success && addUnloader  <CTexture>    (resourceManager);
        success = success && addDestructor<CTexture>    (resourceManager);

        success = success && addCreator   <CTextureView>(resourceManager);
        success = success && addLoader    <CTextureView>(resourceManager);
        success = success && addUnloader  <CTextureView>(resourceManager);
        success = success && addDestructor<CTextureView>(resourceManager);

        success = success && addCreator   <CRenderPass> (resourceManager);
        success = success && addLoader    <CRenderPass> (resourceManager);
        success = success && addUnloader  <CRenderPass> (resourceManager);
        success = success && addDestructor<CRenderPass> (resourceManager);

        success = success && addCreator   <CFrameBuffer>(resourceManager);
        success = success && addLoader    <CFrameBuffer>(resourceManager);
        success = success && addUnloader  <CFrameBuffer>(resourceManager);
        success = success && addDestructor<CFrameBuffer>(resourceManager);

        success = success && addCreator   <CPipeline>   (resourceManager);
        success = success && addLoader    <CPipeline>   (resourceManager);
        success = success && addUnloader  <CPipeline>   (resourceManager);
        success = success && addDestructor<CPipeline>   (resourceManager);

        success = success && addCreator   <CMesh>       (resourceManager);
        success = success && addLoader    <CMesh>       (resourceManager);
        success = success && addUnloader  <CMesh>       (resourceManager);
        success = success && addDestructor<CMesh>       (resourceManager);

        return (not success)
                ? EEngineStatus::ResourceManager_Setup_Failed
                : EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CResourceManager::deinitialize()
    {
        clear();

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

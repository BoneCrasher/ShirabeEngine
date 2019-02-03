#include "resource_management/resourcemanager.h"
#include "graphicsapi/resources/types/all.h"

namespace engine
{
    using engine::gfxapi::CTexture;
    using engine::gfxapi::CTextureView;
    using engine::gfxapi::CRenderPass;
    using engine::gfxapi::CFrameBuffer;

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
        bool success = true;

        success = success &&
                addCreator<CTexture>(std::bind(
                                 &CResourceManager::createResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3)).successful();
        success = success &&
                addCreator<CTextureView>(std::bind(
                                 &CResourceManager::createResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3)).successful();

        success = success &&
                addCreator<CRenderPass>(std::bind(
                                 &CResourceManager::createResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3)).successful();

        success = success &&
                addCreator<CFrameBuffer>(std::bind(
                                 &CResourceManager::createResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3)).successful();

        success = success &&
                addLoader<CTexture>(std::bind(
                                 &CResourceManager::loadResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addLoader<CTextureView>(std::bind(
                                 &CResourceManager::loadResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addLoader<CRenderPass>(std::bind(
                                 &CResourceManager::loadResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addLoader<CFrameBuffer>(std::bind(
                                 &CResourceManager::loadResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addUpdater<CTexture>(std::bind(
                                 &CResourceManager::updateResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2)).successful();

        success = success &&
                addUpdater<CTextureView>(std::bind(
                                 &CResourceManager::updateResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2)).successful();

        success = success &&
                addUnloader<CTexture>(std::bind(
                                 &CResourceManager::unloadResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addUnloader<CTextureView>(std::bind(
                                 &CResourceManager::unloadResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addUnloader<CRenderPass>(std::bind(
                                 &CResourceManager::unloadResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addUnloader<CFrameBuffer>(std::bind(
                                 &CResourceManager::unloadResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addDestructor<CTexture>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addDestructor<CTextureView>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addDestructor<CRenderPass>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        success = success &&
                addDestructor<CFrameBuffer>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1)).successful();

        if(!success)
        {
            return EEngineStatus::Error;
        }
        else
        {
            return EEngineStatus::Ok;
        }
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

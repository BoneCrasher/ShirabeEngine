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
    EEngineStatus CResourceManager::initialize()
    {
        bool success = true;

        success = success &&
                addCreator<CTexture>(std::bind(
                                 &CResourceManager::createResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3));
        success = success &&
                addCreator<CTextureView>(std::bind(
                                 &CResourceManager::createResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3));        

        success = success &&
                addCreator<CRenderPass>(std::bind(
                                 &CResourceManager::createResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3));

        success = success &&
                addCreator<CFrameBuffer>(std::bind(
                                 &CResourceManager::createResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2,
                                 std::placeholders::_3));

        success = success &&
                addLoader<CTexture>(std::bind(
                                 &CResourceManager::loadResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addLoader<CTextureView>(std::bind(
                                 &CResourceManager::loadResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addLoader<CRenderPass>(std::bind(
                                 &CResourceManager::loadResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addLoader<CFrameBuffer>(std::bind(
                                 &CResourceManager::loadResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addUpdater<CTexture>(std::bind(
                                 &CResourceManager::updateResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2));

        success = success &&
                addUpdater<CTextureView>(std::bind(
                                 &CResourceManager::updateResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1,
                                 std::placeholders::_2));

        success = success &&
                addUnloader<CTexture>(std::bind(
                                 &CResourceManager::unloadResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addUnloader<CTextureView>(std::bind(
                                 &CResourceManager::unloadResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addUnloader<CRenderPass>(std::bind(
                                 &CResourceManager::loadResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addUnloader<CFrameBuffer>(std::bind(
                                 &CResourceManager::loadResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addDestructor<CTexture>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CTexture>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addDestructor<CTextureView>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CTextureView>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addDestructor<CRenderPass>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CRenderPass>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

        success = success &&
                addDestructor<CFrameBuffer>(std::bind(
                                 &CResourceManager::destroyResourceImpl<CFrameBuffer>,
                                 static_cast<CResourceManagerBase *>(this),
                                 std::placeholders::_1));

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
    EEngineStatus CResourceManager::deinitialize()
    {
        clear();

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

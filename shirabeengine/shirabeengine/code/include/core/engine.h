#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include <platform/platform.h>
#include <os/applicationenvironment.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include <wsi/windowmanager.h>
#include <asset/assetstorage.h>
#include <resources/core/resourceproxyfactory.h>
#include <renderer/irenderer.h>

#include <vulkan/vulkanenvironment.h>

#if   defined SHIRABE_PLATFORM_WINDOWS
#include <wsi/windows/windowserror.h>
#elif defined SHIRABE_PLATFORM_LINUX
#include <wsi/x11/x11error.h>
#endif

#include "buildingblocks/scene.h"

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace engine
{
    using namespace asset;
    using namespace rendering;
    using namespace resources;
    using namespace engine::os;
    using namespace engine::wsi;
    using namespace engine::vulkan;

    /**
     * @brief The CEngineInstance class
     */
    class SHIRABE_LIBRARY_EXPORT CEngineInstance
    {
        SHIRABE_DECLARE_LOG_TAG(EngineInstance)

    public_constructors:
        /**
         * @brief CEngineInstance
         * @param aEnvironment
         */
        CEngineInstance(CStdSharedPtr_t<os::SApplicationEnvironment> const &aEnvironment);

    public_destructors:
        /**
         *
         */
        ~CEngineInstance();

    public_methods:
        /**
         * @brief initialize
         * @return
         */
        EEngineStatus initialize();

        /**
         * @brief deinitialize
         * @return
         */
        EEngineStatus deinitialize();

        /**
         * @brief update
         * @return
         */
        EEngineStatus update();

    private_members:
        // Application
        CStdSharedPtr_t<os::SApplicationEnvironment> mApplicationEnvironment;

        // WSI
        CStdSharedPtr_t<CWindowManager> mWindowManager;
        CStdSharedPtr_t<IWindow>        mMainWindow;

        // Assets & Resources
        CStdSharedPtr_t<CAssetStorage>         mAssetStorage;
        CStdSharedPtr_t<CResourceProxyFactory> mProxyFactory;
        CStdSharedPtr_t<CResourceManagerBase>  mResourceManager;

        // Rendering
        CStdSharedPtr_t<CVulkanEnvironment> mVulkanEnvironment;
        CStdSharedPtr_t<IRenderer>          mRenderer;

        // Internals
        CScene mScene;
    };
}

#endif 


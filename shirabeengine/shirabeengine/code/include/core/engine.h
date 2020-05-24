#ifndef __SHIRABE_ENGINEINSTANCE_H__
#define __SHIRABE_ENGINEINSTANCE_H__

#include <platform/platform.h>
#include <os/applicationenvironment.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include <core/benchmarking/timer/timer.h>
#include <wsi/windowmanager.h>
#include <asset/assetstorage.h>

#include <renderer/rendergraph/framegraphcontexts.h>
#include <resources/cresourcemanager.h>
#include <material/loader.h>
#include <mesh/loader.h>
#include <textures/loader.h>

#include <vulkan_integration/vulkanenvironment.h>

#if   defined SHIRABE_PLATFORM_WINDOWS
    #include <wsi/windows/windowserror.h>
#elif defined SHIRABE_PLATFORM_LINUX
    #include <wsi/x11/x11error.h>
#include <material/loader.h>

#endif

#include "buildingblocks/scene.h"
#include "ecws/componentmanager.h"

class EngineTime; // Fwd Definition to expose concept, but spare definition for later classes.

namespace engine
{
    using namespace asset;
    using namespace rendering;
    using namespace resources;
    using namespace engine::os;
    using namespace engine::wsi;
    using namespace engine::vulkan;
    using material::CMaterialLoader;
    using mesh::CMeshLoader;
    using textures::CTextureLoader;
    using framegraph::SRenderGraphRenderContext;
    using framegraph::SRenderGraphResourceContext;

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
        explicit CEngineInstance(Shared<os::SApplicationEnvironment> aEnvironment);

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
        CEngineResult<> initialize();

        /**
         * @brief deinitialize
         * @return
         */
        CEngineResult<> deinitialize();

        /**
         * @brief update
         * @return
         */
        CEngineResult<> update();

    private_members:
        // Application
        Shared<os::SApplicationEnvironment> mApplicationEnvironment;

        // Timing
        CTimer                             mTimer;

        // WSI
        Shared<CWindowManager>             mWindowManager;
        Shared<IWindow>                    mMainWindow;

        // Assets & Resources
        Shared<CAssetStorage>        mAssetStorage;
        Shared<CResourceManagerBase> mResourceManager;
        Shared<CMaterialLoader>      mMaterialLoader;
        Shared<CMeshLoader>                mMeshLoader;
        Shared<CTextureLoader>             mTextureLoader;

        // Rendering
        Shared<CVulkanEnvironment>         mVulkanEnvironment;
        Shared<SRenderGraphRenderContext>   mRenderContext;
        Shared<SRenderGraphResourceContext> mResourceContext;
        Shared<CRenderer>                  mRenderer;

        // Internals
        CScene                             mScene;
    };
}

#endif 


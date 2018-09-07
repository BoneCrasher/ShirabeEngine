#ifndef __SHIRABE_SCENE_H__
#define __SHIRABE_SCENE_H__

#include <core/benchmarking/timer/timer.h>
#include <buildingblocks/camera.h>
#include <ecws/icomponentfactory.h>
#include <ecws/entity.h>

namespace engine
{
    /**
     * The CScene class wraps all necessary information for a fully functional
     * game scene.
     */
    class CScene
    {
    public_constructors:
        /**
         * Default construct an empty scene.
         */
        CScene();

    public_destructors:
        /**
         * Destroy and run...
         */
        ~CScene();

    public_methods:
        /**
         * Initialize the scene instance for usage.
         *
         * @return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus initialize();

        /**
         * Shutdown and deiniitalize the scene instance.
         *
         * @return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus deinitialize();

        /**
         * Update this scene instance.
         *
         * @return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus update();

    private_members:
        CStdSharedPtr_t<IComponentFactory> mComponentFactory;

        CTimer  mTimer;
        CCamera mPrimaryCamera;

        EntityList mEntities;

    };

}

#endif

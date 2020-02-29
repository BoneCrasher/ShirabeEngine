#ifndef __SHIRABE_SCENE_H__
#define __SHIRABE_SCENE_H__

#include <core/benchmarking/timer/timer.h>
#include <core/datastructures/adjacencytree.h>
#include <buildingblocks/camera.h>
#include <ecws/icomponentfactory.h>
#include <ecws/entity.h>

namespace engine
{
    using namespace engine::datastructures;

    namespace asset
    {
        class CAssetStorage;
    }

    namespace mesh
    {
        class CMeshLoader;
    }

    namespace material
    {
        class CMaterialLoader;
    }

    namespace textures
    {
        class CTextureLoader;
    }

    /**
     * The CScene class wraps all necessary information for a fully functional
     * game scene.
     */
    class CScene
    {
    public_static_constants:
        static constexpr char const *sEmptyEntityName = "";

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
        CEngineResult<> initialize(Shared<asset::CAssetStorage>      aAssetStorage
                                 , Shared<mesh::CMeshLoader>         aMeshLoader
                                 , Shared<material::CMaterialLoader> aMaterialLoader
                                 , Shared<textures::CTextureLoader>  aTextureLoader);

        CEngineResult<> initializeResources();

        CEngineResult<> deinitializeResources();

        /**
         * Shutdown and deiniitalize the scene instance.
         *
         * @return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        CEngineResult<> deinitialize();

        /**
         * Update this scene instance.
         *
         * @return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        CEngineResult<> update(CTimer const &aTimer);

        CEngineResult<> addEntity(Unique<ecws::CEntity> aEntity, std::string const &aParentEntityName = sEmptyEntityName);

        CEngineResult<> removeEntity(Unique<ecws::CEntity> aEntity);

        [[nodiscard]]
        SHIRABE_INLINE
        Vector<Unique<ecws::CEntity>> const &getEntities() const { return mEntities; }

        Unique<ecws::CEntity> const &findEntity(std::string const &aName)
        {
            for(auto const &e : mEntities)
            {
                if(aName == e->name())
                {
                    return e;
                }
            }

            static Unique<ecws::CEntity> gNullEntity = nullptr;
            return gNullEntity;
        }

    private_members:
        Shared<ecws::IComponentFactory> mComponentFactory;

        CTimer  mTimer;
        CCamera mPrimaryCamera;

        Vector<Unique<ecws::CEntity>> mEntities;
        CAdjacencyTree<std::string>   mHierarchy;
    };
}

#endif

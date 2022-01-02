#ifndef __SHIRABE_SCENE_H__
#define __SHIRABE_SCENE_H__

#include <core/benchmarking/timer/timer.h>
#include <core/datastructures/adjacencytree.h>
#include <buildingblocks/camera.h>
#include <ecws/icomponentfactory.h>
#include <ecws/componentsystem.h>
#include <ecws/entity.h>

namespace engine
{
    using namespace engine::datastructures;

    namespace asset
    {
        class CAssetStorage;
    }

    namespace resources
    {
        class CRHIResourceManagerBase;
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

    public_typedefs:
        using EntityRegistry_t         = Map<ecws::CEntity::UID_t,      Unique<ecws::CEntity>>;
        using ComponentRegistry_t      = Map<ecws::PublicComponentId_t, Shared<ecws::IComponent>>;
        using ComponentHierarchyTree_t = CAdjacencyTree<ecws::PublicComponentId_t>;

    public_static_constants:
        static constexpr char const *sEmptyEntityName = "";

    private_members:
        Shared<ecws::IComponentFactory> mComponentFactory;

        EntityRegistry_t mEntities;

        CTimer  mTimer;

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
        CEngineResult<> initialize(Shared<asset::CAssetStorage>               aAssetStorage
                                 , Shared<resources::CRHIResourceManagerBase> aResourceManager
                                 , Shared<mesh::CMeshLoader>                  aMeshLoader
                                 , Shared<material::CMaterialLoader>          aMaterialLoader
                                 , Shared<textures::CTextureLoader>           aTextureLoader);

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

        CEngineResult<> addEntity(Unique<ecws::CEntity> aEntity, ecws::CEntity::UID_t aParentEntityId);
        CEngineResult<> removeEntity(ecws::CEntity::UID_t aEntityId);

        [[nodiscard]]
        SHIRABE_INLINE
        Map<ecws::CEntity::UID_t , Unique<ecws::CEntity>> const &getEntities() const { return mEntities; }

        Unique<ecws::CEntity> const &findEntity(ecws::CEntity::UID_t aEntityId);
        Unique<ecws::CEntity> const &findEntity(String const &aName);
    };
}

#endif

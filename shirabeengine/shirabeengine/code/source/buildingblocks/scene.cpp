#include "buildingblocks/scene.h"

#include <asset/assetstorage.h>
#include <util/crc32.h>
#include <log/log.h>

#include <material/declaration.h>
#include <material/loader.h>
#include <mesh/declaration.h>
#include <mesh/loader.h>
#include <textures/declaration.h>
#include <textures/loader.h>

#include "ecws/meshcomponent.h"
#include "ecws/materialcomponent.h"
#include "ecws/transformcomponent.h"
#include "ecws/cameracomponent.h"

#include

namespace engine
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CScene::CScene()
        : mComponentFactory(nullptr)
        , mTimer()
        , mPrimaryCamera()
        , mEntities()
    {}
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CScene::~CScene()
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<Shared<material::CMaterialInstance>> loadMaterial(Shared<asset::CAssetStorage>      aAssetStorage
                                                                  , Shared<material::CMaterialLoader> aMaterialLoader
                                                                  , asset::AssetId_t                  aAssetId
                                                                  , bool                              aAutoCreatedConfig    = false
                                                                  , bool                              aIncludeSystemBuffers = false)
    {
        auto fetch = aMaterialLoader->loadMaterialInstance(aAssetStorage
                                                         , aAssetId
                                                         , aAutoCreatedConfig
                                                         , aIncludeSystemBuffers);
        if(not fetch.successful())
        {
            CLog::Error("CScene::loadMaterial", "Could not load material instance w/ ID {}", aAssetId);
        }

        return fetch;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<Shared<mesh::CMeshInstance>> loadMesh(Shared<asset::CAssetStorage> aAssetStorage
                                                      , Shared<mesh::CMeshLoader>    aMeshLoader
                                                      , asset::AssetId_t             aAssetId)
    {
        auto fetch = aMeshLoader->loadMeshInstance(aAssetStorage
                                                 , aAssetId);
        if(not fetch.successful())
        {
            CLog::Error("CScene::loadMesh", "Could not load mesh instance w/ ID {}", aAssetId);
        }

        return fetch;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<Shared<textures::CTextureInstance>> loadTexture(Shared<asset::CAssetStorage>     aAssetStorage
                                                                , Shared<textures::CTextureLoader> aTextureLoader
                                                                , asset::AssetId_t                 aAssetId)
    {
        auto fetch = aTextureLoader->loadInstance(aAssetStorage
                                                , aAssetId);
        if(not fetch.successful())
        {
            CLog::Error("CScene::loadTexture", "Could not load texture instance w/ ID {}", aAssetId);
        }

        return fetch;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::initialize(Shared<asset::CAssetStorage>      aAssetStorage
                                     , Shared<mesh::CMeshLoader>         aMeshLoader
                                     , Shared<material::CMaterialLoader> aMaterialLoader
                                     , Shared<textures::CTextureLoader>  aTextureLoader)
    {
        // Materials
        auto const &[r0, mat_core]        = loadMaterial(aAssetStorage, aMaterialLoader, util::crc32FromString("materials/core/core.material.meta"));
        auto const &[r1, mat_lighting]    = loadMaterial(aAssetStorage, aMaterialLoader, util::crc32FromString("materials/deferred/phong/phong_lighting.material.meta"));
        auto const &[r2, mat_compositing] = loadMaterial(aAssetStorage, aMaterialLoader, util::crc32FromString("materials/deferred/compositing/compositing.material.meta"));
        auto const &[r3, mat_standard]    = loadMaterial(aAssetStorage, aMaterialLoader, util::crc32FromString("materials/standard/standard.material.meta"));
        // Meshes
        auto const &[r4, mesh_fish]       = loadMesh(aAssetStorage, aMeshLoader, util::crc32FromString("meshes/barramundi/BarramundiFish.mesh.meta"));
        // Textures
        auto const &[r5, tex_fish_base]   = loadTexture (aAssetStorage, aTextureLoader, util::crc32FromString("textures/BarramundiFish_baseColor.texture.meta"));
        auto const &[r6, tex_fish_normal] = loadTexture (aAssetStorage, aTextureLoader, util::crc32FromString("textures/BarramundiFish_normal.texture.meta"));

        mat_standard->getMutableConfiguration().setSampledImage("diffuseTexture", util::crc32FromString("textures/BarramundiFish_baseColor.texture.meta"));
        mat_standard->getMutableConfiguration().setSampledImage("normalTexture", util::crc32FromString("textures/BarramundiFish_normal.texture.meta"));

        auto coreTransform         = makeShared<ecws::CTransformComponent>("core_transform");
        auto coreMaterialComponent = makeShared<ecws::CMaterialComponent>("core_material");
        coreMaterialComponent->setMaterialInstance(mat_core);

        auto coreEntity = makeUnique<ecws::CEntity>("core");
        coreEntity->addComponent(coreTransform);
        coreEntity->addComponent(coreMaterialComponent);

        auto transformComponent = makeShared<ecws::CTransformComponent>("barramundi_transform");
        auto meshComponent      = makeShared<ecws::CMeshComponent>     ("barramundi_mesh");
        auto materialComponent  = makeShared<ecws::CMaterialComponent> ("barramundi_material");

        meshComponent    ->setMeshInstance(mesh_fish);
        materialComponent->setMaterialInstance(mat_standard);

        auto barramundi = makeUnique<ecws::CEntity>("barramundi");
        barramundi->addComponent(materialComponent);
        barramundi->addComponent(meshComponent);
        barramundi->addComponent(transformComponent);

        CCamera::SFrustumParameters frustum {};
        frustum.width             = 1920;
        frustum.height            = 1080;
        frustum.nearPlaneDistance = 0.1f;
        frustum.farPlaneDistance  = 10.0f;
        frustum.fovY              = static_cast<float>(M_PI) / 4.0f; // 45 degrees

        CCamera::SProjectionParameters projection {};
        projection.projectionType = ECameraProjectionType::Perspective;

        Shared<CCamera> camera = makeShared<CCamera>(ECameraViewType::FreeCamera
                                                     , frustum
                                                     , projection);

        auto cameraTransform = makeShared<ecws::CTransformComponent>("primaryCamera_transform");
        auto cameraComponent = makeShared<ecws::CCameraComponent>("primaryCamera_camera");
        cameraTransform->getMutableTransform().translate(CVector3D<float>({0.0, 0.1, -1.0}));
        cameraComponent->setCamera(camera);

        auto cameraEntity = makeUnique<ecws::CEntity>("primaryCamera");
        cameraEntity->addComponent(cameraTransform);
        cameraEntity->addComponent(cameraComponent);

        this->addEntity(std::move(coreEntity));
        this->addEntity(std::move(barramundi), "core");
        this->addEntity(std::move(cameraEntity), "barramundi");

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::initializeResources()
    {
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::deinitializeResources()
    {
        // Preload all meshes for the frame and spawn resource tasks where necessary.
        RefIndex_t const &meshReferenceIndex = mResourceData.meshes();
        for(auto const &reference : meshReferenceIndex)
        {
            auto [fetchSuccessCode, mesh] = mResourceData.getResourceMutable<SFrameGraphMesh>(reference);
            if(CheckEngineError(fetchSuccessCode))
            {
                CLog::Error(logTag(), "Failed to fetch mesh w/ id {}", reference);
                continue;
            }

            EEngineStatus attributeBufferStatus = EEngineStatus::Ok;
            EEngineStatus indexBufferStatus     = EEngineStatus::Ok;
            attributeBufferStatus = aRenderContext.initializeBuffer(renderContextState, mesh->attributeBuffer);
            if(not CheckEngineError(attributeBufferStatus))
            {
                attributeBufferStatus = aRenderContext.transferBuffer(renderContextState, mesh->attributeBuffer);
            }
            indexBufferStatus = aRenderContext.initializeBuffer(renderContextState, mesh->indexBuffer);
            if(not CheckEngineError(indexBufferStatus))
            {
                indexBufferStatus = aRenderContext.transferBuffer(renderContextState, mesh->indexBuffer);
            }
        }

        RefIndex_t const &materialReferenceIndex = mResourceData.materials();
        for(auto const &reference : materialReferenceIndex)
        {
            auto [fetchSuccessCode, material] = mResourceData.getResourceMutable<SFrameGraphMaterial>(reference);
            if(CheckEngineError(fetchSuccessCode))
            {
                CLog::Error(logTag(), "Failed to fetch material w/ id {}", reference);
                continue;
            }
        }

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::deinitialize()
    {
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::update(CTimer const &aTimer)
    {
        auto const updateTransformFn = [&, this] (std::string const &aSource, std::string const &aTarget) -> bool
        {
            Unique<ecws::CEntity> const &source = findEntity(aSource);
            Unique<ecws::CEntity> const &target = findEntity(aTarget);

            ecws::CBoundedCollection<Shared<ecws::CTransformComponent>> const sourceTransforms = source->getTypedComponentsOfType<ecws::CTransformComponent>();
            ecws::CBoundedCollection<Shared<ecws::CTransformComponent>> const targetTransforms = target->getTypedComponentsOfType<ecws::CTransformComponent>();
            if(sourceTransforms.empty() || targetTransforms.empty())
            {
                return true; // Nothing to be done...
            }

            Shared<ecws::CTransformComponent> const &sourceTransform = *(sourceTransforms.cbegin());
            Shared<ecws::CTransformComponent> const &targetTransform = *(targetTransforms.cbegin());

            targetTransform->getMutableTransform().updateWorldTransform(sourceTransform->getTransform().world());

            return true;
        };

        bool const successful = mHierarchy.foreachEdgeFromRoot(updateTransformFn, "core");

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::addEntity(Unique<ecws::CEntity> aEntity, std::string const &aParentEntityName)
    {
        auto const iterator = std::find(mEntities.cbegin(), mEntities.cend(), aEntity);
        if(mEntities.end() != iterator)
        {
            return EEngineStatus::Error;
        }

        mHierarchy.add(aEntity->name());

        if(not aParentEntityName.empty()) {
            mHierarchy.connect(aParentEntityName, aEntity->name());
        }

        mEntities.emplace_back(std::move(aEntity));

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEngineResult<> CScene::removeEntity(Unique<ecws::CEntity> aEntity)
    {
        auto const iterator = std::find(mEntities.cbegin(), mEntities.cend(), aEntity);
        if(mEntities.end() == iterator)
        {
            return EEngineStatus::Ok;
        }

        Unique<ecws::CEntity> const &entity = *iterator;

        mHierarchy.disconnectMany(aEntity->name());
        mHierarchy.remove(aEntity->name());

        mEntities.erase(iterator);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

#include "buildingblocks/scene.h"
#include "ecws/transformcomponent.h"

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
    CEngineResult<> CScene::initialize()
    {
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

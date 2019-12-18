#include <ecws/transformcomponent.h>
#include "ecws/cameracomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CCameraComponent::CCameraComponent(std::string const &aName)
        : CComponentBase(aName)
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CCameraComponent::~CCameraComponent()
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CCameraComponent::update(CTimer const &aTimer)
    {
        CBoundedCollection<Shared<CTransformComponent>> const  transformComponents = getParentEntity()->getTypedComponentsOfType<ecws::CTransformComponent>();
        Shared<CTransformComponent>                     const &transformComponent  = *(transformComponents.cbegin());

        mCamera->update(aTimer, transformComponent->getTransform());

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

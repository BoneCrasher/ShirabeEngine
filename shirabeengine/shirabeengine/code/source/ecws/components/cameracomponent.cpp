#include <ecws/components/transformcomponent.h>
#include "ecws/components/cameracomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CCameraComponent::CCameraComponent(String aName)
        : AComponentBase<SCameraComponentState>(std::move(aName))
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CCameraComponent::~CCameraComponent()
    {
        getMutableComponentState().camera = nullptr;
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

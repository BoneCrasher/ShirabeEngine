#include <ecws/transformcomponent.h>
#include "ecws/cameracomponent.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CCameraComponent::CCameraComponent()
        : IComponent()
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
        Shared<ecws::CTransformComponent> const &transformComponent = ...; // getParentEntity()->getComponentsTyped<ecws::CTransformComponent>() ...
        Shared<CTransform> const &transform = nullptr;
        mCamera->update(aTimer, *transform);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

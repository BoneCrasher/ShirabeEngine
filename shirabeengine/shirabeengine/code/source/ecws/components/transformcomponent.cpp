#include "ecws/components/transformcomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CTransformComponent::CTransformComponent(String aName)
        : AComponentBase<STransformComponentState>(std::move(aName))
        //, mMaterialInstance(nullptr)
    {
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CTransformComponent::~CTransformComponent()
    {
        //mMaterialInstance = nullptr;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CTransformComponent::update(CTimer const &aTimer)
    {
        SHIRABE_UNUSED(aTimer);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

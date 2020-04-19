#include "ecws/materialcomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CMaterialComponent::CMaterialComponent(std::string const &aName)
        : CComponentBase(aName)
        //, mMaterialInstance(nullptr)
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CMaterialComponent::~CMaterialComponent()
    {
        //mMaterialInstance = nullptr;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CMaterialComponent::update(CTimer const &aTimer)
    {
        SHIRABE_UNUSED(aTimer);

        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CMaterialComponent::setMaterialInstance(Shared<material::CMaterialInstance> aMaterialInstance)
    {
        if(nullptr == aMaterialInstance)
        {
            return EEngineStatus::Error;
        }

        mMaterialInstance = std::move(aMaterialInstance);
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

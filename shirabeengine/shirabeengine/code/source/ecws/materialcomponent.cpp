#include "ecws/materialcomponent.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CMaterialComponent::CMaterialComponent()
        : IComponent()
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
    EEngineStatus CMaterialComponent::setMaterialInstance(Shared<material::CMaterialMaster> aMaterialInstance)
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

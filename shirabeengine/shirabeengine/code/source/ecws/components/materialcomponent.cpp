#include "ecws/components/materialcomponent.h"
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CMaterialComponent::CMaterialComponent(String aName)
        : AComponentBase<SMaterialComponentState>(std::move(aName))
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

        getMutableComponentState().material = std::move(aMaterialInstance);
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    material::CMaterialConfig& CMaterialComponent::getMutableConfiguration()
    {
        static material::CMaterialConfig sEmptyConfig;

        if(not getMutableComponentState().material)
        {
            return sEmptyConfig;
        }

        return getMutableComponentState().material->getMutableConfiguration();
    }
    //<-----------------------------------------------------------------------------
}

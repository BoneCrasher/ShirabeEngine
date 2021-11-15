//
// Created by dottideveloper on 22.11.19.
//
#include "ecws/entity.h"

namespace engine::ecws
{
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEntity::CEntity(std::string aName)
        : mName(std::move(aName))
    {}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    CEntity::~CEntity()
    {
        deinitialize();
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CEntity::initialize()
    {
        if(mRootComponent)
        {
            return mRootComponent->initialize();
        }
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CEntity::deinitialize()
    {
        if(mRootComponent)
        {
            return mRootComponent->deinitialize();
        }
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CEntity::addComponent(PublicComponentId_t   aParentComponentId
                               , Shared<IComponent>  aComponent)
    {
        if (   gInvalidComponentId == aParentComponentId
            || not aComponent
            || not containsComponent(aComponent->getComponentId()))
        {
            return false;
        }

        PublicComponentId_t const childComponentId = aComponent->getComponentId();
        mAssignedComponents.insert({childComponentId, aComponent});
        mComponentHierarchy.add(childComponentId);
        mComponentHierarchy.connect(aParentComponentId, childComponentId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    bool CEntity::removeComponent(PublicComponentId_t aComponentId)
    {
        if(gInvalidComponentId == aComponentId || not containsComponent(aComponentId))
        {
            return false;
        }

        mComponentHierarchy.remove(aComponentId);
        mAssignedComponents.erase(aComponentId);
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    EEngineStatus CEntity::update(CTimer const &aTimer)
    {
        if(mRootComponent)
        {
            return mRootComponent->update(aTimer);
        }
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------
}

#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include <core/enginestatus.h>
#include <core/benchmarking/timer/timer.h>
#include "ecws/componentbase.h"

namespace engine::ecws
{
    /**
     * A CEntity instance describes an engine entity, which can be enriched by various
     * types of engine components.
     */

    class CEntity
        : public CComponentBase
    {
    public_constructors:
        explicit CEntity(std::string aName);

    public_destructors:
        ~CEntity();

    public_methods:
    };

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TComponent>
    EEngineStatus CEntity::addComponent(Shared<TComponent> const &aComponent)
    {
        std::type_index const index = std::type_index(typeid(TComponent));

        if(mComponents.end() == mComponents.find(index))
        {
            mComponents[index] = CBoundedCollection<Shared<IComponent>>(1); // For now permit one component per type...
        }

        CBoundedCollection<Shared<IComponent>> &collection = mComponents.at(index);
        if(collection.contains(aComponent))
        {
            return EEngineStatus::Error;
        }

        aComponent->setParentEntity(this);
        collection.add(aComponent);
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TComponent>
    EEngineStatus CEntity::removeComponent(Shared<TComponent> const &aComponent)
    {
        std::type_index const index = std::type_index(typeid(TComponent));

        if(mComponents.end() != mComponents.find(index))
        {
            return EEngineStatus::Error;
        }

        CBoundedCollection<Shared<IComponent>> &collection = mComponents.at(index);
        if(not collection.contains(aComponent))
        {
            return EEngineStatus::Error;
        }

        collection.remove(aComponent);
        return EEngineStatus::Ok;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    template <typename TComponent>
    bool CEntity::hasComponentOfType() const {
        return false;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    template <typename TComponent>
    CBoundedCollection<Shared<TComponent>> const CEntity::getTypedComponentsOfType() const
    {
        std::type_index const index = std::type_index(typeid(TComponent));
        if(mComponents.end() == mComponents.find(index))
        {
            return {};
        }

        CBoundedCollection<Shared<IComponent>> const  components = mComponents.at(index);
        CBoundedCollection<Shared<TComponent>> casted = {};

        casted.assign(components);

        return casted;
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SHIRABE_DECLARE_LIST_OF_TYPE(Shared<CEntity>, Entity);
    //<-----------------------------------------------------------------------------
}

#endif

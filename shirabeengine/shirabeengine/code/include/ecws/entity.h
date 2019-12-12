#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include "core/enginestatus.h"
#include "ecws/icomponent.h"

namespace engine::ecws
{
    /**
     * A CEntity instance describes an engine entity, which can be enriched by various
     * types of engine components.
     */
    class CEntity
    {
    public_constructors:
        explicit CEntity(std::string aName);

    public_destructors:
        ~CEntity();

    public_methods:
        /**
         * Return the name of the component.
         *
         * @return See brief.
         */
         [[nodiscard]]
        SHIRABE_INLINE const std::string &name() const { return mName; }

        /**
         * Set the component name.
         *
         * @param aName The new name of the component.
         */
        SHIRABE_INLINE void setName(std::string const& aName) { mName = aName; }

        /**
         * Update the component with an optionally usable timer component.
         *
         * @param aTimer A timer providing the currently elapsed time.
         * @return       Return EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus update(CTimer const &aTimer);

        /**
         * Add a component to the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        EEngineStatus addComponent(Shared<TComponent> const &aComponent);

        /**
         * Remove a component from the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        EEngineStatus removeComponent(Shared<TComponent> const &aComponent);

        /**
         * Check, wether a specific component type is available in the internal component collection.
         *
         * @tparam TComponent The type of component to check for.
         */
        template<typename TComponent>
        [[nodiscard]]
        bool hasComponentOfType() const;

        /**
         * Fetch a list of components of specific type TComponent, if any.
         *
         * @tparam TComponent The type of component to enumerate.
         */
        template <typename TComponent>
        CBoundedCollection<Shared<TComponent>> const getTypedComponentsOfType();

    private_members:
        std::string   mName;
        IComponentMap mComponents;
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
    CBoundedCollection<Shared<TComponent>> const CEntity::getTypedComponentsOfType()
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

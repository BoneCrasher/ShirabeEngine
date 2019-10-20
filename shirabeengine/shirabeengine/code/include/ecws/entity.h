#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include "core/enginestatus.h"
#include "ecws/icomponent.h"

namespace engine
{
    /**
     * A CEntity instance describes an engine entity, which can be enriched by various
     * types of engine components.
     */
    class CEntity
    {
    public_constructors:
        CEntity();

    public_destructors:
        ~CEntity();

    public_methods:
        /**
         * Return the name of the component.
         *
         * @return See brief.
         */
        SHIRABE_INLINE const std::string &name() const { return mName; }

        /**
         * Set the component name.
         *
         * @param aName The new name of the component.
         */
        SHIRABE_INLINE void  name(const std::string& name) { mName = name; }

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
        EEngineStatus addComponent(Shared<IComponent> const &aComponent);

        /**
         * Remove a component from the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        EEngineStatus removeComponent(Shared<IComponent> const &aComponent);

        /**
         * Check, wether a specific component type is available in the internal component collection.
         *
         * @tparam TComponent The type of component to check for.
         */
        template<typename TComponent>
        bool hasComponentOfType() const;

        /**
         * Fetch a list of components of specific type TComponent, if any.
         *
         * @tparam TComponent The type of component to enumerate.
         */
        template <typename TComponent>
        const Vector<Shared<TComponent>> getComponentsOfType();

    private_members:
        std::string    mName;
        IComponentList mComponents;
    };

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
    const Vector<Shared<TComponent>> CEntity::getComponentsOfType()
    {
        return {};
    }
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SHIRABE_DECLARE_LIST_OF_TYPE(Shared<CEntity>, Entity);
    //<-----------------------------------------------------------------------------
}

#endif

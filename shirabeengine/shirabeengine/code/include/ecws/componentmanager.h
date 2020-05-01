#ifndef __SHIRABE_COMPONENTMANAGER_H__
#define __SHIRABE_COMPONENTMANAGER_H__

#include <string>
#include <base/declaration.h>
#include <core/enginetypehelper.h>
#include <core/datastructures/adjacencytree.h>
#include <core/benchmarking/timer/timer.h>
#include <core/enginestatus.h>

namespace engine::ecws
{
	class CComponentManager
	{
	public_constructors:
	    CComponentManager() = default;

    public_destructors:
		~CComponentManager() = default;

	public_methods:
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
        EEngineStatus addComponent(std::string const &aComponentName);

        /**
         * Check, wether a specific component type is available in the internal component collection.
         *
         * @tparam TComponent The type of component to check for.
         */
        template<typename TComponent>
        [[nodiscard]]
        bool hasComponentOfType(std::string const &aContainer) const;

        /**
         * Fetch a list of components of specific type TComponent, if any.
         *
         * @tparam TComponent The type of component to enumerate.
         */
        template <typename TComponent>
        CBoundedCollection<Shared<TComponent>> const getTypedComponentsOfType() const;

        /**
         * Remove a component from the internal component collection.
         *
         * @param aComponent The component to add to the entity instance.
         * @return           EEngineStatus::Ok, if successful. An error code otherwise.
         */
        template <typename TComponent>
        EEngineStatus removeComponent(Shared<TComponent> const &aComponent);

    protected_methods:
	    SHIRABE_INLINE
        void setParentEntity(class CEntity const *const aEntity)
        {
	        mParentEntity = aEntity;
        }

	    [[nodiscard]]
	    SHIRABE_INLINE
	    CEntity const *const getParentEntity() const
        {
	        return mParentEntity;
        }

	private_members:
	    std::string mName;
	};
}
#endif

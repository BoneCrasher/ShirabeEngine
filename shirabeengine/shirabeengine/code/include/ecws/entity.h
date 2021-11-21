#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include <core/enginestatus.h>
#include <core/datastructures/adjacencytree.h>
#include <core/benchmarking/timer/timer.h>
#include "ecws/componentsystem.h"

namespace engine::ecws
{
    /**
     * A CEntity instance describes an engine entity, which can be enriched by various
     * types of engine components.
     */
    class CEntity
    {
    public_typedefs:
        using ComponentAssignmentMap_t = Map<PublicComponentId_t, Shared<IComponent>>;
        using ComponentHierarchyTree_t = CAdjacencyTree<PublicComponentId_t>;

    private_members:
        String                   mName;
        Shared<IComponent>       mRootComponent;
        ComponentAssignmentMap_t mAssignedComponents;
        ComponentHierarchyTree_t mComponentHierarchy;

    public_constructors:
        explicit CEntity(std::string aName);

    public_destructors:
        ~CEntity();

    public_methods:
        /**
         * Fetch this entity's root component
         *
         * @return A weak reference to the entity's root component.
         */
        SHIRABE_INLINE Weak<IComponent> getRootComponent() { return mRootComponent; };
        /**
         * Fetch this entity's root component
         *
         * @return An immutable weak reference to the entity's root component.
         */
        SHIRABE_INLINE Weak<IComponent> const getRootComponent() const { return mRootComponent; };

        /**
         * Initialize this entity and all components, if any.
         * @return EEngineStatus::Ok on success; An error code otherwise
         */
        EEngineStatus initialize();

        /**
         * Denitialize this entity and all components, if any.
         * @return EEngineStatus::Ok on success; An error code otherwise
         */
        EEngineStatus deinitialize();

        /**
         * Update the entity and all of it's components, where applicable.
         *
         * @param aTimer Timer instance used to fetch various game thread timings.
         * @return EEngineStatus::Ok on success; An error code otherwise
         */
        EEngineStatus update(CTimer const &aTimer);

        SHIRABE_INLINE String const &name() const
        {
            return mName;
        }

    protected_methods:
        bool addComponent(PublicComponentId_t   aParentComponentId
                          , Shared<IComponent>  aComponent);

        bool removeComponent(PublicComponentId_t aComponentId);

        bool containsComponent(PublicComponentId_t aComponentId);
    };

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SHIRABE_DECLARE_LIST_OF_TYPE(Unique<CEntity>, Entity);
    //<-----------------------------------------------------------------------------
}

#endif

#ifndef __SHIRABE_ENTITY_H__
#define __SHIRABE_ENTITY_H__

#include <core/enginestatus.h>
#include <core/datastructures/adjacencytree.h>
#include <core/benchmarking/timer/timer.h>
#include "ecws/componentsystem.h"

namespace engine::ecws
{
    SHIRABE_DECLARE_LOG_TAG(CEntity)

    /**
     * A CEntity instance describes an engine entity, which can be enriched by various
     * types of engine components.
     */
    class CEntity
    {
    public_typedefs:
        using UID_t = uint64_t;
        using ComponentAssignmentMap_t = Map<PublicComponentId_t, Shared<IComponent>>;
        using ComponentHierarchyTree_t = CAdjacencyTree<PublicComponentId_t>;

    private_members:
        UID_t                    mUid;
        String                   mName;
        Shared<IComponent>       mRootComponent;
        ComponentAssignmentMap_t mAssignedComponents;
        ComponentHierarchyTree_t mComponentHierarchy;

        Atomic<bool> mInitialized;

    public_constructors:
        explicit CEntity(String aName);
        CEntity(CEntity const&) = delete;
        CEntity(CEntity &&)     = delete;

    public_destructors:
        ~CEntity();

    public_operators:
        CEntity &operator=(CEntity const&) = delete;
        CEntity &operator=(CEntity &&)     = delete;

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

        [[nodiscard]]
        SHIRABE_INLINE UID_t uid() const
        {
            return mUid;
        }

        SHIRABE_INLINE String const &name() const
        {
            return mName;
        }

    protected_methods:
        bool addComponent(PublicComponentId_t         aParentComponentId
                          , Shared<IComponent> const& aComponent);

        bool removeComponent(PublicComponentId_t aComponentId, bool bReattachChildren = true);

        bool containsComponent(PublicComponentId_t aComponentId);

    private_static_functions:
        static UID_t getUID()
        {
            Atomic<UID_t> gEntityUIDStore = 0;
            return ++gEntityUIDStore;
        };

    private_methods:
        bool foreachChildOf(Shared<IComponent> &aComponent
                            , std::function<EEngineStatus(Shared<IComponent>&)> aForeachChildFn
                            , CAdjacencyTree<PublicComponentId_t>::EOrder aOrder = CAdjacencyTree<PublicComponentId_t>::EOrder::LeafFirst);
    };

    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
    SHIRABE_DECLARE_LIST_OF_TYPE(Unique<CEntity>, Entity);
    //<-----------------------------------------------------------------------------
}

#endif

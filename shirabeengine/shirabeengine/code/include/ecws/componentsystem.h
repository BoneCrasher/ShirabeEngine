#ifndef __SHIRABE_ECWS_OMPONENT_SYSTEM_H__
#define __SHIRABE_ECWS_OMPONENT_SYSTEM_H__

#include <base/declaration.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include <core/datastructures/adjacencytree.h>
#include "ecws/uidissuer.h"

using namespace engine::datastructures;

namespace engine::ecws
{
    using ComponentSystemId_t          = uint32_t;
    using ComponentId_t                = uint32_t;
    using PublicComponentId_t          = uint64_t;
    using PublicComponentUidComposer_t = uid::composition::CUidComposer<PublicComponentId_t, ComponentSystemId_t, ComponentId_t>;

    constexpr ComponentSystemId_t gUninitializedComponentSystemId = 0;

    class IComponent
    {
        SHIRABE_DECLARE_INTERFACE(IComponent);

    public_api:
    };

    class IComponentSystem
    {
        SHIRABE_DECLARE_INTERFACE(IComponentSystem);

    public_api:
        /**
         * Returns the component system id assigned on registration by the component system manager.
         *
         * @return If the id was initialized by the component system manager, any value larger than 0. Otherwise 0.
         */
        [[nodiscard]]
        virtual ComponentSystemId_t getComponentSystemId() const = 0;

        /**
         * Set the unique component system id for this component system on registration in the component
         * system manager.
         * Has to be any value larger than 0.
         *
         * @param aComponentSystemId Unique component system id as a value larger than 0.
         */
        virtual void setComponentSystemId(ComponentSystemId_t aComponentSystemId) = 0;

        /**
         * Initialize the component system.
         *
         * @return EEngineStatus::Ok, if successful.
         * @return Any EEngineStatus error code, on error.
         */
        virtual EEngineStatus initialize() = 0;

        /**
         * Cleanup and Deinitialize the component system.
         * Will destroy all components & invalidate all component references issued to the engine.
         *
         * @return EEngineStatus::Ok, if successful.
         * @return An error code, on if something went wrong.
         */
        virtual EEngineStatus deinitialize() = 0;

        /**
         * Try to fetch a specific component, if created and registered.
         *
         * @param aId The id of the component to be fetched.
         * @return A valid weak pointer to the component or nullptr;
         */
        virtual Weak<IComponent> getComponent(ComponentId_t aId) = 0;
    };

    class AComponentSystemBase
        : public IComponentSystem
    {
    private_members:
        CUidIssuer<ComponentId_t>            mComponentIdIssuer;
        ComponentSystemId_t                  mComponentSystemId;
        CAdjacencyTree<ComponentId_t>        mComponentHierarchy;
        Map<ComponentId_t, Weak<IComponent>> mComponents;

    public_constructors:
        explicit AComponentSystemBase();

    public_destructors:
        virtual ~AComponentSystemBase() override;

    public_methods:
        /**
         * Returns the component system id assigned on registration by the component system manager.
         *
         * @return If the id was initialized by the component system manager, any value larger than 0. Otherwise 0.
         */
        [[nodiscard]]
        ComponentSystemId_t getComponentSystemId() const override;

        /**
         * Set the unique component system id for this component system on registration in the component
         * system manager.
         * Has to be any value larger than 0.
         *
         * @param aComponentSystemId Unique component system id as a value larger than 0.
         */
        void setComponentSystemId(ComponentSystemId_t aComponentSystemId) override;

        /**
         * Initialize the component system.
         *
         * @return EEngineStatus::Ok, if successful.
         * @return Any EEngineStatus error code, on error.
         */
        EEngineStatus initialize() override;

        /**
         * Cleanup and Deinitialize the component system.
         * Will destroy all components & invalidate all component references issued to the engine.
         *
         * @return EEngineStatus::Ok, if successful.
         * @return An error code, on if something went wrong.
         */
        EEngineStatus deinitialize() override;

        /**
         * Try to fetch a specific component, if created and registered.
         *
         * @param aId The id of the component to be fetched.
         * @return A valid weak pointer to the component or nullptr;
         */
        template <typename TComponentType>
        Weak<TComponentType> getTypedComponent(ComponentId_t aComponentId);

        /**
         * Try to fetch a specific component, if created and registered.
         *
         * @param aId The id of the component to be fetched.
         * @return A valid weak pointer to the component or nullptr;
         */
        Weak<IComponent> getComponent(ComponentId_t aComponentId) final;

    protected_methods:
        /**
         * Type-cast a base component to the specific component type implemented by
         * the component system.
         *
         * @param aComponent The base component to cast.
         * @return A valid weak pointer to the cast base component, if convertible.
         */
        template <typename TComponentType>
        Weak<TComponentType> castComponent(Weak<IComponent> aComponent);
    };

    #include "componentsystem.inl"

    class CComponentSystemManager
    {
    private_members:
        CUidIssuer<ComponentSystemId_t>                    mComponentSystemIdIssuer;
        Map<ComponentSystemId_t, Shared<IComponentSystem>> mRegisteredComponentSystems;

    public_constructors:
        explicit CComponentSystemManager();

    public_destructors:
        ~CComponentSystemManager();

    public_methods:
        CEngineResult<ComponentSystemId_t> registerComponentSystem(Shared<IComponentSystem> &&aComponentSystem);
        CEngineResult<> unregisterComponentSystem(ComponentSystemId_t acomponentSystemId);
    };
}
#endif

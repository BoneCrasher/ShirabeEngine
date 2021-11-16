#ifndef __SHIRABE_ECWS_OMPONENT_SYSTEM_H__
#define __SHIRABE_ECWS_OMPONENT_SYSTEM_H__

#include <typeindex>
#include <base/declaration.h>
#include <core/enginestatus.h>
#include <core/enginetypehelper.h>
#include <core/datastructures/adjacencytree.h>
#include <core/benchmarking/timer/timer.h>
#include "ecws/uidissuer.h"

using namespace engine::datastructures;

namespace engine::ecws
{
    using ComponentSystemId_t          = int32_t;
    using ComponentId_t                = int32_t;
    using PublicComponentId_t          = int64_t;
    using PublicComponentUidComposer_t = uid::composition::CUidComposer<PublicComponentId_t, ComponentSystemId_t, ComponentId_t>;

    constexpr ComponentSystemId_t gInvalidComponentSystemId = -1;
    constexpr ComponentId_t       gInvalidComponentId       = -1;
    constexpr PublicComponentId_t gInvalidPublicComponentId = -1;

    /**
     * Shared public interface of all components.
     */
    class IComponent
    {
        SHIRABE_DECLARE_INTERFACE(IComponent);

        friend class IComponentSystem;

    public_api:
        /**
         * Return the public component id of this component.
         * @return
         */
        [[nodiscard]]
        virtual PublicComponentId_t getComponentId() const = 0;

        /**
         * Retrieve the component's assigned name.
         * @return
         */
        [[nodiscard]]
        virtual String const& getComponentName() const = 0;

        /**
         * Initialize the component.
         *
         * @return EEngineStatus::Ok, if successful.
         * @return Any EEngineStatus error code, on error.
         */
        virtual EEngineStatus initialize() = 0;

        /**
         * Cleanup and Deinitialize the component.
         * Will destroy all components & invalidate all component references issued to the engine.
         *
         * @return EEngineStatus::Ok, if successful.
         * @return An error code, on if something went wrong.
         */
        virtual EEngineStatus deinitialize() = 0;

        /**
         * Update this component using the provided timer instance.
         *
         * @param aTimer Timer instance providing the delta elapsed and total elapsed time of the game/level/world/...!
         * @return EEngineStatus::Ok, if successful.
         * @return An error code, on if something went wrong.
         */
        virtual EEngineStatus update(CTimer const &aTimer) = 0;

    protected_api:
        /**
         * Assign a public component id based on the issuing component system.
         */
        virtual void setComponentIds(ComponentId_t aComponentId, PublicComponentId_t aPublicComponentId) = 0;
    };

    /**
     * Base class for all components.
     * Each component contains an instance of a state, which could simply be an empty struct, if no data is used.
     * To keep the concepts consistent, each component is required to bind against a state definition.
     *
     * @tparam TComponentState The specific state class to be used by the component.
     */
    template <typename TComponentState>
    class AComponentBase
        : public IComponent
    {
    private_members:
        PublicComponentId_t     mPublicComponentId        = gInvalidComponentId;
        ComponentId_t           mComponentId   = gInvalidComponentId;
        String                  mComponentName = u8"";
        Unique<TComponentState> mState         = nullptr;
        bool                    mStateIsExternallyManaged = false;

    public_constructors:
        AComponentBase();
        explicit AComponentBase(String aComponentName);

    public_destructors:
        ~AComponentBase() override;

    public_methods:
        /**
         * @copydoc IComponent::getComponentId()
         */
        [[nodiscard]]
        PublicComponentId_t getComponentId() const final;

        [[nodiscard]]
        String const& getComponentName() const final;

        /**
         * @copydoc IComponent::initialize()
         */
        EEngineStatus initialize() override;

        /**
         * @copydoc IComponent::deinitialize()
         */
        EEngineStatus deinitialize() override;

    protected_methods:
        /**
         * @copydoc IComponent::setPublicComponentId()
         */
        void setComponentIds(ComponentId_t aComponentId, PublicComponentId_t aPublicComponentId) final;

        /**
         * Externally provide a component state to this component from the respective
         * subystem.
         *
         * @param aComponentState
         */
        void setComponentState(TComponentState *aComponentState);

        /**
         * Internal function to effectively update the component state.
         *
         * @param aComponentState
         * @param aIsExternallyManaged
         */
        void setComponentStateInternal(Unique<TComponentState> &&aComponentState, bool aIsExternallyManaged);

        /**
         * Fetch an immutable reference to the internal component state.
         * @return See brief.
         */
        TComponentState const& getComponentState() const;

        /**
         * Fetch a mutable reference to the internal component state.
         * @return See brief.
         */
        TComponentState& getMutableComponentState();
    };

    /**
     * Base class for all components that are integrated into any engine subsystem.
     * On creation, these components will receive a reference to the respective subsystem they integrate with.
     * Additionally, the subsystem shall provide a component state through the parent class' setComponentState-method.
     *
     * @tparam TSubsystem      The static type of the subsystem the component integrates with.
     * @tparam TComponentState The specific state type in the subsystem the component will be assigned by the subsystem.
     */
    template <typename TSubsystem, typename TComponentState>
    class ASubsystemIntegratedComponentBase
        : public AComponentBase<TComponentState>
    {
    private_members:
        /**
         * Reference to the subsystem the component integrates with.
         */
        Shared<TSubsystem> mAttachedSubsystem = nullptr;

    public_constructors:
        explicit ASubsystemIntegratedComponentBase(String aComponentName, Shared<TSubsystem> aSubsystem);

    public_destructors:
        ~ASubsystemIntegratedComponentBase() override;

    public_methods:
        /**
         * @copydoc IComponent::initialize()
         */
        EEngineStatus initialize() override;

        /**
         * @copydoc IComponent::deinitialize()
         */
        EEngineStatus deinitialize() override;

    };

    /**
     * Shared public interface of all component systems.
     */
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
         * Return a list of supported component types that can be created by this component system.
         *
         * @return See brief.
         */
        [[nodiscard]]
        virtual Vector<std::type_index> const &getSupportedComponentTypes() const = 0;

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
         * Test for whether a specific component with the provided id exists.
         *
         * @param aId The id of the component to look for.
         * @return True if a component with the specified id exists.
         */
        [[nodiscard]]
        virtual bool hasComponent(PublicComponentId_t aId) = 0;

        /**
         * Try to fetch a specific component, if created and registered.
         *
         * @param aId The id of the component to be fetched.
         * @return A valid weak pointer to the component or nullptr;
         */
        [[nodiscard]]
        virtual Weak<IComponent> getComponent(PublicComponentId_t aId) = 0;
    };

    /**
     * Internal component system manager binding capabilities for component systems.
     */
    class IComponentSystemManagerBinding
    {
        SHIRABE_DECLARE_INTERFACE(IComponentSystemManagerBinding);

        friend class CComponentSystemManager;

    protected_api:
        /**
         * Bind the component system to a manager, which issues the component system id.
         *
         * @param aComponentSystemId The component system id issued by a component system manager.
         * @return EEngineStatus::Ok on success. Error code otherwise.
         */
        virtual EEngineStatus bindToManager(ComponentSystemId_t aComponentSystemId) = 0;

        /**
         * Unbind the component system from a manager bound to the component system.
         * No-op if not bound.
         */
        virtual void unbindFromManager() = 0;

        /**
         * Check, if the component system is bound.
         *
         * @return True, if bound. False otherwise.
         */
         [[nodiscard]]
        virtual bool isRegisteredInManager() const = 0;
    };

    /**
     * Basic component system implementation that binds into the component system manager and provides
     * core facilities for composed component system id issuing.
     * Implements the "public" interface IComponentSystem, which is the only interface to be based on publicly.
     * Internally, the component system manager will also address functionality declared by IComponentSystemManagerBinding
     * in order to ad-hoc bind a component system to a manager.
     */
    template <typename... TSupportedComponentTypes>
    class AComponentSystemBase
        : public IComponentSystem
        , public IComponentSystemManagerBinding
    {
    private_members:
        /**
         * ComponentSystemManager-assigned system id.
         * Used as the primary part of public component system id composition.
         */
        ComponentSystemId_t mComponentSystemId;
        /**
         * Id issuer for newly created components of the system.
         * Ids created by the issuer are used as the secondary part of public component system id composition.
         * The issuer will be initialized w/ the capacity of the components array.
         */
        CUidIssuer<ComponentId_t> mComponentIdIssuer;
        /**
         * List of supported component types in this base component system.
         */
        Vector<std::type_index> mSupportedComponentTypes;
        /**
         * Vector of created components.
         */
        Vector<Shared<IComponent>> mComponents;

    public_constructors:
        explicit AComponentSystemBase();

    public_destructors:
        ~AComponentSystemBase() override;

    public_methods:
        /**
         *  @copydoc IComponentSystem::getComponentSystemId()
         */
        [[nodiscard]]
        ComponentSystemId_t getComponentSystemId() const final;

        /**
         * @copydoc IComponentSystem::getSupportedComponentTypes()
         */
        Vector<std::type_index> const &getSupportedComponentTypes() const final;

        /**
         *  @copydoc IComponentSystem::initialize()
         */
        EEngineStatus initialize() override;

        /**
         *  @copydoc IComponentSystem::deinitialize()
         */
        EEngineStatus deinitialize() override;

        /**
         * @copydoc IComponentSystem::hasComponent(ComponentId_t)
         */
         [[nodiscard]]
        bool hasComponent(PublicComponentId_t aComponentId) final;

        /**
         *  @copydoc IComponentSystem::getComponent(ComponentId_t)
         */
        [[nodiscard]]
        Weak<IComponent> getComponent(PublicComponentId_t aComponentId) final;

        /**
         * Try to fetch a specific component, if created and registered.
         *
         * @param aId The id of the component to be fetched.
         * @return A valid weak pointer to the component or nullptr;
         */
        template <typename TComponentType>
        Weak<TComponentType> getTypedComponent(PublicComponentId_t aComponentId);


    protected_methods:
        /**
         *  @copydoc IComponentSystemBinding::bindToManager(ComponentSystemId_t)
         */
        EEngineStatus bindToManager(ComponentSystemId_t aComponentSystemId) final;

        /**
         *  @copydoc IComponentSystemBinding::unbindFromManager()
         */
        void unbindFromManager() final;

        /**
         *  @copydoc IComponentSystemBinding::isRegisteredInManager()
         */
        [[nodiscard]]
        bool isRegisteredInManager() const final;

        /**
         * Type-cast a base component to the specific component type implemented by
         * the component system.
         *
         * @param aComponent The base component to cast.
         * @return A valid weak pointer to the cast base component, if convertible.
         */
        template <typename TComponentType>
        Weak<TComponentType> castComponent(Weak<IComponent> aComponent);

        /**
         * Returns the uid issuer of this component system.
         *
         * @return See brief.
         */
        CUidIssuer<ComponentId_t> &getComponentUidIssuer();
    };

    /**
     * Manager implementation to manage (register, fetch, unregister) component systems.
     * Component systems are accessible by their component system id, or any type they support/create.
     */
    class CComponentSystemManager
    {
    private_members:
        /**
         * Id issuer for component system registrations.
         */
        CUidIssuer<ComponentSystemId_t> mComponentSystemIdIssuer;
        /**
         * Mapping of component system id to the respective component system.
         */
        Map<ComponentSystemId_t, Shared<IComponentSystem>> mComponentSystemRegistry;
        /**
         * Mapping of type indices to component system ids to find the appropriate
         * component system for a specific component type.
         */
        Map<std::type_index, Shared<IComponentSystem>> mComponentSystemTypeRegistry;

    public_constructors:
        explicit CComponentSystemManager();

    public_destructors:
        ~CComponentSystemManager();

    public_methods:
        /**
         * Initialize the component system manager.
         *
         * @return EEngineStatus::Ok, if successful. Any error code on error.
         */
        CEngineResult<> initialize();

        /**
         * Cleanup and unregister all systems, then shut the component system manager down.
         *
         * @return EEngineStatus::Ok, if successful. Any error code on error.
         */
        CEngineResult<> deinitialize();

        /**
         * Register a component system in the manager.
         * Implicitly, this will update the component system id issued by the manager and
         * flags the system as "registered".
         * Derived component systems should not create any components unless the system is registered.
         *
         * @param aComponentSystem
         * @return
         */
        CEngineResult<ComponentSystemId_t> registerComponentSystem(Shared<IComponentSystem> &&aComponentSystem);

        /**
         * Unbind and unregister a component system from the manager.
         * Since the stored reference may be the only remaining reference, return it for future use.
         * Ignoring the return value will destroy the component system in that case.
         *
         * @param aComponentSystemId The id of the system to unregister.
         * @return EEngineStatus::Ok with either a valid component system or nullptr as result value.
         */
        CEngineResult<Shared<IComponentSystem>> unregisterComponentSystem(ComponentSystemId_t aComponentSystemId);

        /**
         * Tries to find and fetch a component system.
         * If not registered, will return nullptr.
         *
         * @param aComponentSystemId The id of the component system to fetch.
         * @return A component system reference or nullptr.
         */
        Shared<IComponentSystem> findComponentSystem(ComponentSystemId_t aComponentSystemId);

        /**
         * Tries to find and fetch a component system by a component type the system could create.
         * If not registered, will return nullptr.
         *
         * @tparam TComponentType Type name of the component allegedly creatable by the desired system.
         * @return A component system reference or nullptr.
         */
        template <typename TComponentType>
        Shared<IComponentSystem> findComponentSystemByComponentType();
    };

    #include "componentsystem.inl"
}

#endif

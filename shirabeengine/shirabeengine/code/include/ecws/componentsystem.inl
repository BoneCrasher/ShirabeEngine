#ifndef __SHIRABE_COMPONENT_SYSTEM_INL_H__
#define __SHIRABE_COMPONENT_SYSTEM_INL_H__

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
AComponentBase<TComponentState>::AComponentBase()
    : IComponent()
      , mPublicComponentId(gInvalidComponentId)
      , mComponentId(gInvalidComponentId)
      , mComponentName(u8"Unnamed Component")
      , mState(nullptr)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
AComponentBase<TComponentState>::AComponentBase(String aComponentName)
    : IComponent()
    , mPublicComponentId(gInvalidComponentId)
    , mComponentId(gInvalidComponentId)
    , mComponentName(std::move(aComponentName))
    , mState(nullptr)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
AComponentBase<TComponentState>::~AComponentBase()
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
PublicComponentId_t AComponentBase<TComponentState>::getComponentId() const
{
    return mPublicComponentId;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
String const& AComponentBase<TComponentState>::getComponentName() const
{
    return mComponentName;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
EEngineStatus AComponentBase<TComponentState>::initialize()
{
    if(nullptr == mState)
    {
        setComponentStateInternal(makeUnique<TComponentState>(), false);
    }
    return EEngineStatus::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
EEngineStatus AComponentBase<TComponentState>::deinitialize()
{
    if(nullptr != mState && not mStateIsExternallyManaged)
    {
        mState.reset();
    }
    setComponentStateInternal(nullptr, false);

    return EEngineStatus::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
void AComponentBase<TComponentState>::setComponentIds(ComponentId_t aComponentId, PublicComponentId_t aPublicComponentId)
{
    mPublicComponentId = aPublicComponentId;
    mComponentId       = aComponentId;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
void AComponentBase<TComponentState>::setComponentState(TComponentState *aComponentState)
{
    if(nullptr == aComponentState && not mStateIsExternallyManaged)
    {
        return; // Do not null non-external state.
    }

    if(nullptr != aComponentState)
    {
        // Wrap the state pointer in a non-owning shared-pointer instance.
        setComponentStateInternal(makeNonDeletingUniqueFromInstance(aComponentState), true);
    }
    else
    {
        setComponentStateInternal(makeUnique<TComponentState>(), false);
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
void AComponentBase<TComponentState>::setComponentStateInternal(Unique<TComponentState> &&aComponentState
                                                                , bool                    aIsExternallyManaged)
{
    mState                    = std::move(aComponentState);
    mStateIsExternallyManaged = aIsExternallyManaged;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
TComponentState const& AComponentBase<TComponentState>::getComponentState() const
{
    return *mState;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState>
TComponentState& AComponentBase<TComponentState>::getMutableComponentState()
{
    return *mState;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState, typename... TAttachedSubsystems>
ASubsystemIntegratedComponentBase<TComponentState, TAttachedSubsystems...>
    ::ASubsystemIntegratedComponentBase(String aComponentName, Shared<TAttachedSubsystems>... aSubsystems)
        : AComponentBase<TComponentState>(std::move(aComponentName))
        , mAttachedSubsystems({ Pair_t(typeid(TAttachedSubsystems), Variant_t(aSubsystems))... })
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState, typename... TAttachedSubsystems>
ASubsystemIntegratedComponentBase<TComponentState, TAttachedSubsystems...>::~ASubsystemIntegratedComponentBase()
{
    mAttachedSubsystems.reset();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState, typename... TAttachedSubsystems>
EEngineStatus ASubsystemIntegratedComponentBase<TComponentState, TAttachedSubsystems...>::initialize()
{
    return AComponentSystemBase<TComponentState>::initialize();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentState, typename... TAttachedSubsystems>
EEngineStatus ASubsystemIntegratedComponentBase<TComponentState, TAttachedSubsystems...>::deinitialize()
{
    return AComponentSystemBase<TComponentState>::deinitialize();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
AComponentSystemBase<TSupportedComponentTypes...>::AComponentSystemBase()
    : IComponentSystem()
      , IComponentSystemManagerBinding()
      , mComponentSystemId(gInvalidComponentSystemId)
      , mComponentIdIssuer()
      , mComponents()
{

}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
AComponentSystemBase<TSupportedComponentTypes...>::~AComponentSystemBase()
{
    // TODO: How to clean up here? Just deprecating things? Detaching the components?
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
ComponentSystemId_t AComponentSystemBase<TSupportedComponentTypes...>::getComponentSystemId() const
{
    return mComponentSystemId;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
Vector<std::type_index> const &AComponentSystemBase<TSupportedComponentTypes...>::getSupportedComponentTypes() const
{
    return mSupportedComponentTypes;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
IComponentSystemManagerBinding *const AComponentSystemBase<TSupportedComponentTypes...>::getManagerBindingInterface() const
{
    return this;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
EEngineStatus AComponentSystemBase<TSupportedComponentTypes...>::initialize()
{
    mSupportedComponentTypes.insert(mSupportedComponentTypes.end(), {typeid(TSupportedComponentTypes)...});
    return EEngineStatus::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
EEngineStatus AComponentSystemBase<TSupportedComponentTypes...>::deinitialize()
{
    mComponentIdIssuer.initialize(/* aCapacity */ 2048);
    mComponents.resize(mComponentIdIssuer.capacity());

    return EEngineStatus::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
bool AComponentSystemBase<TSupportedComponentTypes...>::hasComponent(PublicComponentId_t aComponentId)
{
    auto const [systemId, componentId] = PublicComponentUidComposer_t::extract(aComponentId);
    if(getComponentSystemId() != systemId || mComponents.size() <= componentId)
    {
        return {};
    }

    Shared<IComponent> component = mComponents.at(componentId);
    return (nullptr != component);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
Weak<IComponent> AComponentSystemBase<TSupportedComponentTypes...>::getComponent(PublicComponentId_t aComponentId)
{
    auto const [systemId, componentId] = PublicComponentUidComposer_t::extract(aComponentId);
    if(getComponentSystemId() != systemId || mComponents.size() <= componentId)
    {
        return {};
    }

    Shared<IComponent> component = mComponents.at(componentId);
    if(nullptr == component)
    {
        return {};
    }

    return { component };
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
EEngineStatus AComponentSystemBase<TSupportedComponentTypes...>::bindToManager(ComponentSystemId_t aComponentSystemId)
{
    mComponentSystemId = aComponentSystemId;
    return EEngineStatus::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
void AComponentSystemBase<TSupportedComponentTypes...>::unbindFromManager()
{
    mComponentSystemId = gInvalidComponentSystemId;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
bool AComponentSystemBase<TSupportedComponentTypes...>::isRegisteredInManager() const
{
    return (gInvalidComponentSystemId != mComponentSystemId);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
CUidIssuer<ComponentId_t> &AComponentSystemBase<TSupportedComponentTypes...>::getComponentUidIssuer()
{
    return mComponentIdIssuer;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
template <typename TComponentType>
Weak<TComponentType> AComponentSystemBase<TSupportedComponentTypes...>::getTypedComponent(PublicComponentId_t aComponentId)
{
    Weak<IComponent>     component = getComponent(aComponentId);
    Weak<TComponentType> cast      = castComponent<TComponentType>(component);
    return cast;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename... TSupportedComponentTypes>
template <typename TComponentType>
Weak<TComponentType> AComponentSystemBase<TSupportedComponentTypes...>::castComponent(Weak<IComponent> aComponent)
{
    if(aComponent.expired())
    {
        return {};
    }

    Weak<TComponentType> typedComponent = std::static_pointer_cast<TComponentType>(aComponent);
    return typedComponent;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentType>
Shared<IComponentSystem> CComponentSystemManager::findComponentSystemByComponentType()
{
    auto iterator = mComponentSystemTypeRegistry.find(typeid(TComponentType));
    if(mComponentSystemTypeRegistry.end() == iterator)
    {
        return { nullptr };
    }

    auto [id, system] = *iterator;
    return system;
}
//<-----------------------------------------------------------------------------

#endif

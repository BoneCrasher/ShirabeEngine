#ifndef __SHIRABE_COMPONENT_SYSTEM_INL_H__
#define __SHIRABE_COMPONENT_SYSTEM_INL_H__

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
template <typename TComponentType>
Weak<TComponentType> AComponentSystemBase::castComponent(Weak<IComponent> aComponent)
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
Weak<TComponentType> AComponentSystemBase::getTypedComponent(ComponentId_t aComponentId)
{
    Weak<IComponent> component = getComponent(aComponentId);
    return castComponent<TComponentType>(component);
}
//<-----------------------------------------------------------------------------

#endif

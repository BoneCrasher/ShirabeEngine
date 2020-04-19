#include "sr_pch.h"

#include <stdint.h>

#include <core/base/defines/sr_defines.h>
#include <core/base/types/sr_variant.h>

#include "property_system/sr_meta_system.h"
#include "property_system/sr_meta_property.h"

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CStdSharedPtr_t<CMetaSystem> CMetaSystem::get()
{
    static CStdSharedPtr_t<CMetaSystem> sMetaSystemInstance = makeStdSharedPtr<CMetaSystem>();

    const bool metaSystemInitialized = sMetaSystemInstance->isInitialized();
    if(!metaSystemInitialized)
    {
        const bool metaSystemInitSuccessful = sMetaSystemInstance->initialize();
        if(!metaSystemInitSuccessful)
        {
            throw std::runtime_error("Failed to initialize meta-system.");
        }
    }

    return sMetaSystemInstance;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaSystem::CMetaSystem()
    : mPrototypes()
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaSystem::~CMetaSystem()
{
    mPrototypes.clear();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaSystem::isInitialized() const
{
    return mIsInitialized.load();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaSystem::initialize()
{
    mIsInitialized.store(true);

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaSystem::deinitialize()
{
    mIsInitialized.store(false);

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
InstanceUID_t CMetaSystem::generateObjectUID()
{
    return generatePropertyUID(); // (Ab-)use the same generator as for the properties.
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
PropertyUID_t CMetaSystem::generatePropertyUID()
{
    static PropertyUID_t sPropertyUID = 0;

    return (++sPropertyUID); // For now.. just count up.
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/*!
 * Creates a resolver to autoregister a replication handler inside a property instance as observer.
 *
 * @tparam TDataType    The internal datatype of the property to be observed.
 * @tparam TVisitedType The currently visited type from within a SR_RESOLVE::std::visit call.
 */
template <typename TDataType, typename TVisitedType>
struct SRegisterReplicationHandlerResolver
{
    using Observer_t = CMetaSystem::Observer_t<TDataType>;
    using Handler_t  = CMetaSystem::Handler_t<TDataType>;

    static auto getResolver(
            const CMetaProperty<TDataType>& aProperty,
            const Handler_t&                aHandler)
    {
        if constexpr (false == std::is_same_v<TVisitedType, TDataType>)
        {
            // If the types are not matching... Perform no-op.
            // This is a "small" design issue of the std::visit call...
            return [] (const CSubject<const TVisitedType, const uint32_t>& aResolvedInstance) -> void { SR_UNUSED(aResolvedInstance); };
        }
        else
        {
            return [&] (CSubject<const TVisitedType, const uint32_t>& aResolvedInstance) -> void
            {
                CStdSharedPtr_t<Observer_t> observer = makeStdSharedPtr<Observer_t>(aProperty);
                observer->setHandlerFn(aHandler);

                aResolvedInstance.observe(observer);
            };
        }
    }
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaSystem::registerReplicationCallback(
        const CMetaProperty<TDataType>& aProperty,
        const Handler_t<TDataType>&     aHandler)
{
    const PropertyUID_t& propertyUID = aProperty.getPropertyUID();
    SubjectVariant_t&    subject     = mReplicationSubjects[propertyUID];

    SR_RESOLVE_CONVERTER(SRegisterReplicationHandlerResolver, TDataType, subject, aProperty, aHandler);

    return true;
}
//<-----------------------------------------------------------------------------

/*!
 * Creates a resolver usable within std::visit to register a property in the meta system.
 *
 * @tparam TDataType    The internal datatype of the property to be observed.
 * @tparam TVisitedType The currently visited type from within a SR_RESOLVE::std::visit call.
 */
template <typename TDataType, typename TVisitedType>
struct SNotifyReplicationHandlerResolver
{
    using Subject_t = CSubject<TDataType const, uint32_t const>;

    static auto getResolver(
            const TDataType& aValue,
            const uint32_t&  aChangedValueIndex)
    {
        if constexpr (false == std::is_same_v<TVisitedType, TDataType>)
        {
            // If the types are not matching... Perform no-op.
            // This is a "small" design issue of the std::visit call...
            return [] (const CSubject<const TVisitedType, const uint32_t>& aResolvedInstance) -> void { SR_UNUSED(aResolvedInstance); };
        }
        else
        {
            return [&value             = std::as_const(aValue),
                    &changedValueIndex = std::as_const(aChangedValueIndex)]
                    (const CSubject<const TVisitedType, const uint32_t>& aResolvedInstance) -> void
            {
                aResolvedInstance.notify(std::move(value), std::move(changedValueIndex));
            };
        }
    }
};
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaSystem::notifyPropertyReplicationHandlers(
        const CMetaProperty<TDataType>& aProperty,
        const TDataType&                aValue,
        const uint32_t&                 aChangedValueIndex)
{
    const PropertyUID_t&    propertyUID = aProperty.getPropertyUID();
    const SubjectVariant_t& subject     = mReplicationSubjects[propertyUID];

    SR_RESOLVE_CONVERTER(SNotifyReplicationHandlerResolver, TDataType, subject, aValue, aChangedValueIndex);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
#define SR_INSTANTIATE_NOTIFY_PROP_REPLICATION_HANDLER(_aType)            \
    template void CMetaSystem::notifyPropertyReplicationHandlers<_aType>( \
                    const CMetaProperty<_aType>& aProperty,               \
                    const _aType&                aValue,                  \
                    const uint32_t&              aChangedValueIndex);

#define SR_INSTANTIATE_REGISTER_PROP_REPLICATION_HANDLER(_aType)    \
    template bool CMetaSystem::registerReplicationCallback<_aType>( \
                    const CMetaProperty<_aType>& aProperty,         \
                    const Handler_t<_aType>&     aHandler);

SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_NOTIFY_PROP_REPLICATION_HANDLER)
SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_REGISTER_PROP_REPLICATION_HANDLER)


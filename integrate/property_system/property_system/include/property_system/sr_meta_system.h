/*!
 * @file      sr_meta_system.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      25/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <atomic>
#include <string>
#include <unordered_map>
#include <inttypes.h>
#include <typeindex>

#include <core/base/defines/sr_defines.h>
#include <core/base/debug/sr_debug.h>
#include <core/base/patterns/sr_observer.h>
#include <core/base/types/sr_map.h>
#include <core/base/types/smartptr/sr_cppstd_ptr.h>

#include "property_system/sr_meta_def.h"
#include "property_system/sr_meta_property.h"

class CMetaPrototypeBase;

/*!
 * The CMetaSystem-class manages CMetaClass-instances, providing functions for adding
 * and retrieving such meta-classes, identified by a classId (clsId) of type std::string.
 */
class CMetaSystem
{
public_typedefs:
    template <typename TType>
    using Observer_t = CObserver<CMetaProperty<TType>, const TType, const uint32_t>;

    template <typename TType>
    using Handler_t  = typename Observer_t<TType>::HandlerFunction_t;

public_static_functions:
    /*!
     * Creates the CMetaSystem instance if not done yet and returns a reference to it.
     *
     * @return A mutable reference to the CMetaSystem instance, if successfully created.
     * @throws std::runtime_error,, if the system cannot be initialized.
     */
    static CStdSharedPtr_t<CMetaSystem> get();

public_constructors:
    /*!
     * Construct the CMetaSystem-instance and default initialize the class map.
     */
    CMetaSystem();

public_destructors:
    /*!
     * Remove all CMetaClass-instances.
     */
    virtual ~CMetaSystem();

public_methods:
    /*!
     * Checks, whether the CMetaSystem-instance was successfully initialized.
     *
     * @return True, if initialized. False otherwise.
     */
    bool isInitialized() const;
    /*!
     * Initialize the CMetaSystem-instance.
     *
     * @return True, if successful. False otherwise.
     */
    bool initialize();
    /*!
     * Deinitializes the CMetaSystem-instance.
     *
     * @return True, if successful. False otherwise.
     */
    bool deinitialize();

    /*!
     * Attempts to add a CMetaClass for 'aPrototypeId'.
     * If there's already a class added, it will be returned instead.
     *
     * @tparam TPrototypeTYpe    The type of the prototype to be registered.
     * @param  [in] aPrototypeId The class id to be looked up.
     * @return                   A (newly created) reference to a CMetaClass-entry.
     */
    template <typename TPrototypeType>
    CStdSharedPtr_t<TPrototypeType> registerPrototype();

    /*!
     * Tries locate find a mapped CMetaClass for 'aPrototypeId'.
     *
     * @tparam TPrototypeType    The type of the prototype to be retrieved.
     * @param  [in] aPrototypeId The class id to be looked up.
     * @return                   A reference to CMetaClass, if found. Throws otherwise.
     * @throws                   std::runtime_error, if no CMetaClass could be found for 'aPrototypeId'.
     */
    template <typename TPrototypeType>
    CStdSharedPtr_t<TPrototypeType> getPrototype(const std::string& aPrototypeId);

    /*!
     * Register a property hooking into it's value changed notification
     * to make the replication or UI callbacks work.
     *
     * @tparam TDataType      The internal data type of the property to be registered.
     * @param  [in] aProperty The property to link.
     * @return                True, if successful. False otherwise.
     */
    template <typename TDataType>
    bool registerProperty(CMetaProperty<TDataType>& aProperty);

    /*!
     * Register a callback for replication in the system, to be invoked when a property value changes.
     *
     * @param [in] aProperty The property whose values have changed.
     * @param [in] aHandler  The handler to be forwarded to on value change, if the property was to be replicated.
     * @return               True, if successful. False otherwise.
     */
    template <typename TDataType>
    bool registerReplicationCallback(
            const CMetaProperty<TDataType>& aProperty,
            const Handler_t<TDataType>&     aHandler);

    /*!
     * Use the assigned uid generator to provide an object UID.
     *
     * @return A new object UID.
     */
    InstanceUID_t generateObjectUID();

    /*!
     * Use the assigned uid generator to provide a property UID.
     *
     * @return A new property UID.
     */
    PropertyUID_t generatePropertyUID();

private_typedefs:
    /*!
     * Type-Alias to define a named collection of prototypes.
     */
    using PrototypeMap_t = CMap<std::string, CStdSharedPtr_t<CMetaPrototypeBase>>;

    template <typename TType>
    using Subject_t = CSubject<const TType, const uint32_t>;

    using SubjectVariant_t =
        std::variant<
            Subject_t<int8_t>,
            Subject_t<int16_t>,
            Subject_t<int32_t>,
            Subject_t<int64_t>,
            Subject_t<uint8_t>,
            Subject_t<uint16_t>,
            Subject_t<uint32_t>,
            Subject_t<uint64_t>,
            Subject_t<float>,
            Subject_t<double>,
            Subject_t<std::string>,
            Subject_t<std::wstring>>;

    using SubjectVariantMap_t = CMap<PropertyUID_t, SubjectVariant_t>;

private_methods:
    /*!
     * Iterate over all known replication handlers and invoke them.
     *
     * @tparam TDataType              The internal data type of the changed value.
     * @param [in] aProperty          The changed property to replicate.
     * @param [in] aValue             The value changed in the property to replicate.
     * @param [in] aChangedValueIndex The index of the changed value in the property to be replicated.
     */
    template <typename TDataType>
    void notifyPropertyReplicationHandlers(
            const CMetaProperty<TDataType>& aProperty,
            const TDataType&                aValue,
            const uint32_t&                 aChangedValueIndex);

private_members:
    std::atomic<bool>   mIsInitialized;
    PrototypeMap_t      mPrototypes;
    SubjectVariantMap_t mReplicationSubjects;
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TPrototypeType>
CStdSharedPtr_t<TPrototypeType> CMetaSystem::registerPrototype()
{
    const std::string& prototypeId = TPrototypeType::getPrototypeId();

    if(mPrototypes.contains(prototypeId))
    {
        CStdSharedPtr_t<CMetaPrototypeBase> base      = mPrototypes.at(prototypeId);
        CStdSharedPtr_t<TPrototypeType>     prototype = std::static_pointer_cast<TPrototypeType>(base);
    }

    typename TPrototypeType::CreatorFn_t creator   = TPrototypeType::makeDefaultCreator();
    CStdSharedPtr_t<TPrototypeType>      prototype = makeStdSharedPtr<TPrototypeType>(prototypeId, creator);
    prototype->constructPrototype();

    mPrototypes[prototypeId] = prototype;

    return prototype;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TPrototypeType>
CStdSharedPtr_t<TPrototypeType> CMetaSystem::getPrototype(const std::string& aPrototypeId)
{
    const bool prototypeContained = (!mPrototypes.contains(aPrototypeId));
    SR_ASSERTTEXT(false == prototypeContained, "No prototype registered for id '%s'", aPrototypeId);

    CStdSharedPtr_t<CMetaPrototypeBase> base      = mPrototypes[aPrototypeId];
    CStdSharedPtr_t<TPrototypeType>     prototype = std::static_pointer_cast<TPrototypeType>(base);

    return prototype;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaSystem::registerProperty(CMetaProperty<TDataType>& aProperty)
{
    using Observer_t = CObserver<CMetaProperty<TDataType>, const TDataType, const uint32_t>;

    const CMetaPropertyCore::EPropertyReplicationMode& replicationMode = aProperty.getReplicationMode();
    if(CMetaPropertyCore::EPropertyReplicationMode::Replicate == replicationMode)
    {
        const uint32_t propertyCapacity = aProperty.getCapacity();

        bool observed = true;
        for(uint32_t k=0; observed && (k < propertyCapacity); ++k)
        {
            auto const handlerFn = [this] (
                    const CMetaProperty<TDataType>& aProperty,
                    const TDataType&&               aValue,
                    const uint32_t&&                aIndex)
            {
                notifyPropertyReplicationHandlers<TDataType>(aProperty, aValue, aIndex);
            };

            CStdSharedPtr_t<Observer_t> observer = makeStdSharedPtr<Observer_t>(aProperty);
            observer->setHandlerFn(handlerFn);

            observed = observed && aProperty.observe(observer, k);
        }

        return observed;
    }

    return true;
}
//<-----------------------------------------------------------------------------

/*!
 * @file      sr_meta_object.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      25/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <type_traits>
#include <sstream>
#include <locale>
#include <codecvt>
#include <memory>

#include <core/base/defines/sr_defines.h>
#include <core/base/debug/sr_debug.h>
#include <core/base/types/sr_string.h>
#include <core/base/types/sr_passkey.h>
#include <core/base/types/sr_variant.h>

#include "property_system/sr_meta_def.h"
#include "property_system/sr_property_system_error.h"
#include "property_system/sr_meta_system.h"
#include "property_system/sr_meta_property.h"

#include "property_system/serialization/sr_property_de_serialization_interface.h"

/*!
 * Helper macro, to declare the "onPropertyAdded(...)", function of the IMetaPropertyCallback_t
 * for a specific type.
 */
#define SR_DECLARE_PROPERTY_ADDITION_CALLBACK(_aType)                                       \
    /*!
      * Implements a callback to receive property creation notifications
      *
      * @param [in] aPropertyId The id of the property created
      * @param [in] aValue      The initial property value
      * @returns                True of the object's property storage was updated successful.
      *                         False otherwise.
      */                                                                                    \
    bool onPropertyAdded(const std::string& aPropertyid, const _aType& aValue);

/*!
 * Creates a resolver usable within std::visit to register a property in the meta system.
 *
 * @tparam TType The underlying data type of the property.
 */
template <typename TType>
struct SRegisterPropertyResolver
{
    static auto getResolver(const CStdSharedPtr_t<CMetaSystem>& aMetaSystem)
    {
        return [&] (CMetaProperty<TType>& aResolvedInstance) -> void
        {
            aMetaSystem->registerProperty<TType>(aResolvedInstance);
        };
    }
};

/*!
 *  The CMetaObject class is the "link" between CMetaSystem and any class which should be
 *  integrated with it. It provides access to it's CMetaClass and it's property values,
 *  both, mutably and immutably.
 */
class CMetaObject
        : public ISerializable<IPropertySerializer, IPropertyDeserializer>
{
public_typedefs:
    /*!
     * Type-Alias to describe a map of property-variants, reducing declaration redundancy.
     */
    using PropertyMap_t = CMap<std::string, MetaPropertyVariant_t>;

public_static_functions:
    /*!
     * Create a new instance of CMetaObject and store prototype, uid, name and the property values.
     *
     * @param [in] aInstancePrototype      The prototype this object is based on, if any. May be nullptr;
     * @param [in] aInstanceUID            The object's instance UID.
     * @param [in] aInstanceName           The object's name.
     * @param [in] aInstancePropertyValues The object's property set.
     * @return                             A pointer to the newly created CMetaObject. Nullptr otherwise.
     */
    static CMetaObject* create(
            CStdSharedPtr_t<CMetaPrototype<CMetaObject>> aInstancePrototype,
            const InstanceUID_t&                         aInstanceUID,
            const std::string&                           aInstanceName,
            const MetaPropertyMap_t&                     aInstancePropertyValues);

public_constructors:
    /*!
     * Default construct a CMetaObject to "Invalid"
     */
    CMetaObject();

    /*!
     * Construct a CMetaObject with 'aClsId'
     *
     * @param [in] aInstanceUID  The instance unique id of the object to be created.
     * @param [in] aInstanceName A human readable name identification of the object.
     */
    CMetaObject(
            const InstanceUID_t& aInstanceUID,
            const std::string&   aInstanceName);

public_destructors:
    /*!
      * Clear all properties and shutdown
      */
    ~CMetaObject();

public_methods:
    /*!
     * Accept a CPropertySerializer and conduct the object serialization process.
     *
     * @param   [in] aSerializer The serializer to conduct serialization with.
     * @returns                  True, if serialization was successful. False otherwise.
     */
    bool acceptSerializer(IPropertySerializer& aSerializer) const;
    /*!
     * Accept a CPropertyDeserializer and conduct the object deserialization process.
     *
     * @param   [in] aDeserializer The deserializer to conduct deserialization with.
     * @returns                    True, if deserialization was successful. False otherwise.
     */
    bool acceptDeserializer(IPropertyDeserializer& aDeserializer);

    /*!
     * Return the current instance UID of the object.
     *
     * @return The InstanceUID_t of the object.
     */
    const InstanceUID_t& getInstanceUID()  const;

    /*!
     * Return the current instance name of the object.
     *
     * @return The name of the object.
     */
    const std::string& getInstanceName() const;

    /*!
     * Returns (or adds if not done yet) the CMetaClass instance for this class.
     *
     * @return Returns the assigned CMetaClass instance by const ref.
     */
    const CStdSharedPtr_t<CMetaPrototypeBase> getPrototype() const;

    /*!
     * Return the collection of properties currently assigned to this object.
     *
     * @return Returns a map of propertyId -> property of properties currently assigned to this object.
     */
    const PropertyMap_t& getProperties() const;

    /*!
     * Return the collection of mutable properties currently assigned to this object.
     *
     * @return Returns a map of propertyId -> property of properties currently assigned to this object.
     */
    PropertyMap_t& getMutableProperties();

    /*!
     * Remove a property from the internal storage and update.
     *
     * @param  [in] aPropertyId The id of the property affected.
     * @return                  True if the internal storage update was successful.
     *                          False otherwise.
     */
    EPropertySystemError onPropertyRemoved(const std::string& aPropertyId);

    /*!
     * Adds a property with 'aPropertyId' to the internal storage.
     * No-Op, if already contained or the propertyId is empty.
     *
     * @tparam TDataType         The underlying data type of the property to be added.
     * @tparam TEnableFlag       SFINAE-flag for proper specialization selection of the method.
     * @param [in] aPropertyId   The id of the property affected
     * @param [in] aInitialValue The initial value of the property to be added.
     * @return                   The newly added property reference. Debug-breaks otherwise.
     * @tparam                   The data type of the property.
     */
    template <typename TDataType, typename TEnableFlag = std::enable_if_t<!std::is_same_v<TDataType, CStdSharedPtr_t<CMetaObject>>>>
    CMetaProperty<TDataType> &addProperty(
            const std::string& aPropertyId,
            const TDataType&   aInitialValue);

    /*!
     * Adds a property with 'aPropertyId' to the internal storage.
     * No-Op, if already contained or the propertyId is empty.
     *
     * @tparam TDataType         The underlying data type of the property to be added.
     * @param [in] aPropertyId   The id of the property affected
     * @param [in] aInitialValue The initial value of the property to be added.
     * @return                   The newly added property reference. Debug-breaks otherwise.
     * @tparam                   The data type of the property.
     */
    template <typename TDataType>
    CMetaProperty<TDataType> &addProperty(
            const std::string& aPropertyId,
            TDataType&&        aInitialValue = TDataType());

    /*!
     * Tries to retrieve a read only property of type TData from the internal storage.
     *
     * @tparam TDataType         The underlying data type of the property.
     * @param [in] aPropertyId  The id of the property affected
     * @return                  The newly added property reference. Debug-breaks otherwise.
     * @tparam                  The data type of the property.
     */
    template <typename TDataType>
    EPropertySystemError getProperty(
            const std::string&               aPropertyId,
            const CMetaProperty<TDataType>** aOutProperty) const;

    /*!
     * Tries to retrieve a read only property of variant type from the internal storage.
     *
     * @param [in] aPropertyId The id of the property affected
     * @return                 The newly added property reference. Debug-breaks otherwise.
     */
    EPropertySystemError getProperty(
            const std::string&            aPropertyId,
            const MetaPropertyVariant_t** aOutProperty) const;

    /*!
     * Tries to retrieve a read/write property of type TData from the internal storage.
     *
     * @tparam TDataType         The underlying data type of the property.
     * @param  [in] aPropertyId The id of the property affected
     * @return                  The newly added property reference. Debug-breaks otherwise.
     * @tparam                  The data type of the property.
     */
    template <typename TDataType>
    EPropertySystemError getMutableProperty(
            const std::string&         aPropertyId,
            CMetaProperty<TDataType>** aOutProperty);

    /*!
     * Tries to retrieve a read/write property of type variant type from the internal storage.
     *
     * @param [in] aPropertyId The id of the property affected
     * @return                 The newly added property reference. Debug-breaks otherwise.
     */
    EPropertySystemError getMutableProperty(
            const std::string&      aPropertyId,
            MetaPropertyVariant_t** aOutProperty);

    /*!
     * Checks, whether a property with 'aPropertyId' is registered.
     *
     * @param [in] aPropertyId The id of the property affected.
     * @return                 True, if registered. False otherwise.
     */
    bool hasProperty(const std::string& aPropertyId) const;

    /*!
     * Returns the total count of properties registered.
     *
     * @return The total count of properties registered.
     */
    uint32_t getPropertyCount() const;

    /*!
     * Return the property at 'aPropertyName' as ref. to const.
     *
     * @tparam TType              The underlying value type of the property.
     * @param  [in] aPropertyName The name of tye property to be accessed.
     * @return                    Ref. to const to the property required.
     * @remarks                   Debug-Breaks and bails out on error.
     */
    template <typename TType>
    const CMetaProperty<TType>& at(const std::string& aPropertyName) const;

    /*!
     * Return the property at 'aPropertyName' as ref.
     *
     * @tparam TType              The underlying value type of the property.
     * @param  [in] aPropertyName The name of tye property to be accessed.
     * @return                    Ref. to the property required.
     * @remarks                   Debug-Breaks and bails out on error.
     */
    template <typename TType>
    CMetaProperty<TType>& at(const std::string& aPropertyName);

    /*!
     * Removes a property with 'aPropertyId' from the internal storage. No-Op if not found.
     *
     * @param  [in] aPropertyId The id of the property affected.
     * @return                  True, if the update of the internal storage is successful.
     *                          False, otherwise.
     */
    EPropertySystemError removeProperty(const std::string& aPropertyId);

    /*!
     * Deep-clone this object instance.
     *
     * @return A deep cloned full copy of this object including new unique ids and individual registration in the meta system.
     */
    CStdSharedPtr_t<CMetaObject> clone();

protected_members:
    InstanceUID_t                       mInstanceUID;
    std::string                         mInstanceName;
    CStdSharedPtr_t<CMetaPrototypeBase> mPrototype;
    PropertyMap_t                       mProperties;

private_methods:

    /*!
     * Adds a property with 'aPropertyId' to the internal storage.
     * No-Op, if already contained or the propertyId is empty.
     *
     * @tparam TDataType         The underlying data type of the property to be added.
     * @param [in] aPropertyId   The id of the property affected
     * @param [in] aInitialValue The initial value of the property to be added.
     * @return                   The newly added property reference. Debug-breaks otherwise.
     * @tparam                   The data type of the property.
     */
    template <typename TDataType>
    CMetaProperty<TDataType>& addPropertyImpl(
            const std::string& aPropertyId,
            TDataType&&        aInitialValue = TDataType());

    /*!
     * Assign a new set of properties to this object.
     *
     * @param aInstancePropertyValues The new set of properties.
     * @return                        True, if successful. False otherwise.
     */
    bool assignProperties(const MetaPropertyMap_t& aInstancePropertyValues);
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType, typename TEnableFlag>
CMetaProperty<TDataType>& CMetaObject::addProperty(
        const std::string& aPropertyId,
        const TDataType&   aInitialValue)
{
    CMetaProperty<TDataType>& property = addPropertyImpl<TDataType>(aPropertyId, std::move(TDataType(aInitialValue)));
    return property;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType>& CMetaObject::addProperty(
        const std::string& aPropertyId,
        TDataType&&        aInitialValue)
{
    CMetaProperty<TDataType>& property = addPropertyImpl<TDataType>(aPropertyId, std::move(aInitialValue));
    return property;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType>& CMetaObject::addPropertyImpl(
        const std::string& aPropertyId,
        TDataType&&        aInitialValue)
{
    SR_ASSERT(false == aPropertyId.empty()              );
    SR_ASSERT(false == mProperties.contains(aPropertyId));

    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();
    const PropertyUID_t          uid    = system->generatePropertyUID();

    CMetaProperty<TDataType> tempProperty = CMetaProperty<TDataType>(uid, aPropertyId, std::move(aInitialValue));
    MetaPropertyVariant_t    newVariant   = std::move(tempProperty);

    mProperties.add(aPropertyId, std::move(newVariant));

    MetaPropertyVariant_t* storedVariant = nullptr;

    const EPropertySystemError fetched = getMutableProperty(aPropertyId, &storedVariant);
    SR_RESOLVE(SRegisterPropertyResolver, *storedVariant, system);

    CMetaProperty<TDataType> &storedProperty = std::get<CMetaProperty<TDataType>>(*storedVariant);

    return storedProperty;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
EPropertySystemError CMetaObject::getProperty(
        const std::string&               aPropertyId,
        const CMetaProperty<TDataType>** aOutProperty) const
{
    SR_RETURN_IF(nullptr == aOutProperty,                      EPropertySystemError::PropertyPointerIsNull);
    SR_RETURN_IF(true    == aPropertyId.empty(),               EPropertySystemError::PropertyIdIsEmpty);
    SR_RETURN_IF(false   == mProperties.contains(aPropertyId), EPropertySystemError::PropertyNotFound);

    const MetaPropertyVariant_t&    variant  = mProperties.at(aPropertyId);
    const CMetaProperty<TDataType>& property = std::get<CMetaProperty<TDataType>>(variant);

    *aOutProperty = &property;
    return EPropertySystemError::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
EPropertySystemError CMetaObject::getMutableProperty(
        const std::string&         aPropertyId,
        CMetaProperty<TDataType>** aOutProperty)
{
    SR_RETURN_IF(nullptr == aOutProperty, EPropertySystemError::PropertyPointerIsNull);

    const CMetaProperty<TDataType>* property  = nullptr;
    EPropertySystemError            retrieved = const_cast<CMetaObject const*>(this)->getProperty<TDataType>(aPropertyId, &property);
    SR_RETURN_IF( (EPropertySystemError::Ok != retrieved) , retrieved);

    CMetaProperty<TDataType>* mutableProperty = const_cast<CMetaProperty<TDataType>*>(property);
    *aOutProperty = mutableProperty;

    return EPropertySystemError::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TType>
const CMetaProperty<TType>& CMetaObject::at(const std::string& aPropertyName) const
{
    SR_ASSERT(false == aPropertyName.empty());

    const bool propertyContained = mProperties.contains(aPropertyName);
    SR_ASSERT(true  == propertyContained);

    const MetaPropertyVariant_t& variant  = mProperties.at(aPropertyName);
    const CMetaProperty<TType>&  property = std::get<CMetaProperty<TType>>(variant);

    return property;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TType>
CMetaProperty<TType>& CMetaObject::at(const std::string& aPropertyName)
{
    SR_ASSERT(false == aPropertyName.empty());

    const bool propertyContained = mProperties.contains(aPropertyName);
    SR_ASSERT(true  == propertyContained);

    MetaPropertyVariant_t& variant  = mProperties.at(aPropertyName);
    CMetaProperty<TType>&  property = std::get<CMetaProperty<TType>>(variant);

    return property;
}
//<-----------------------------------------------------------------------------

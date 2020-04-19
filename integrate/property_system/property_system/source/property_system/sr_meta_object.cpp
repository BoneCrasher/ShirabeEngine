/*!
 * @file      sr_meta_object.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      11/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"

#include <core/base/types/sr_variant.h>
#include "property_system/sr_meta_object.h"
#include "property_system/sr_meta_prototype.h"

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaObject* CMetaObject::create(
        CStdSharedPtr_t<CMetaPrototype<CMetaObject>> aInstancePrototype,
        const InstanceUID_t&     aInstanceUID,
        const std::string&       aInstanceName,
        const MetaPropertyMap_t& aInstancePropertyValues)
{
    CMetaObject* object = new CMetaObject(aInstanceUID, aInstanceName);

    object->mPrototype = aInstancePrototype;
    object->assignProperties(aInstancePropertyValues);

    return object;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaObject::assignProperties(const MetaPropertyMap_t& aInstancePropertyValues)
{
    mProperties = aInstancePropertyValues;

    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();

    for(auto& [k, v] : mProperties)
    {
        SR_RESOLVE(SRegisterPropertyResolver, v, system);
    }

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaObject::CMetaObject()
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mInstanceUID(0)
    , mInstanceName("Undefined")
    , mPrototype(nullptr)
    , mProperties()
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaObject::CMetaObject(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName)
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mInstanceUID(aInstanceUID)
    , mInstanceName(aInstanceName)
    , mPrototype(nullptr)
    , mProperties()
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaObject::~CMetaObject()
{
    mProperties.clear();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/*!
 * Creates a resolver usable in std::visit to write the correct property property
 * instance to 'aSerializer'.
 *
 * @tparam TType The underlying type of the property.
 */
template <typename TType>
struct SSerializeResolver
{
    static auto getResolver(
            IPropertySerializer& aSerializer,
            const std::string&   aPropertyId,
            bool&                aInOutSerialized)
    {
        return [&] (const CMetaProperty<TType>& aResolvedInstance) -> void
        {
            aInOutSerialized = aInOutSerialized && aSerializer.writeProperty(aPropertyId, aResolvedInstance);
        };
    }
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaObject::acceptSerializer(IPropertySerializer& aSerializer) const
{
    // An object always serializes it's data in the subsequent order:
    //   1. Attributes
    //   2. Properties
    //
    // Since the serializer's methods require the exact type of the property
    // to be written, we visit the MetaPropertyVariant_t to fetch the proper
    // type.

    bool serialized = true;

    std::string prototypeId = "";
    if(mPrototype)
    {
        prototypeId = mPrototype->getInstanceName();
    }

    serialized = serialized && aSerializer.writeAttribute("uid",         mInstanceUID);
    serialized = serialized && aSerializer.writeAttribute("name",        mInstanceName);
    serialized = serialized && aSerializer.writeAttribute("prototypeId", prototypeId);

    serialized = serialized && aSerializer.beginProperties();

    // Important @ Developer: Do not convert this into a structured binding.
    //                        The resolver-lambda declared above, can't reference
    //                        or capture the local reference name of the binding.
    for(const auto& assignment : mProperties)
    {
        const std::string&           propertyId = assignment.first;
        const MetaPropertyVariant_t& property   = assignment.second;

        SR_RESOLVE(SSerializeResolver, property, aSerializer, propertyId, serialized);
    }

    serialized = serialized && aSerializer.commitProperties();

    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/*!
 * Creates a resolver usable in std::visit to read the correct property
 * instance from 'aDeserializer'.
 *
 * @tparam TType The underlying type of the property.
 */
template <typename TType>
struct SDeserializeResolver
{
    static auto getResolver(
            IPropertyDeserializer& aDeserializer,
            const std::string&     aPropertyId,
            bool&                  aInOutDeserialized)
    {
        return [&] (CMetaProperty<TType>& aResolvedInstance) -> void
        {
            aInOutDeserialized = aInOutDeserialized && aDeserializer.readProperty(aPropertyId, aResolvedInstance);
        };
    }
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaObject::acceptDeserializer(IPropertyDeserializer& aDeserializer)
{
    // An object always deserializes it's data in the subsequent order:
    //   1. Attributes
    //   2. Properties
    //
    // The attributes 'uid', 'name' and 'prototype' id are prefetched by the Deserializer,
    // in order to properly create this instance from the prototype!
    //
    // Since the deserializer's methods require the exact type of the property
    // to be written, we visit the MetaPropertyVariant_t to fetch the proper
    // type.

    bool deserialized = true;

    deserialized = deserialized && aDeserializer.beginProperties();

    // Important @ Developer: Do not convert this into a structured binding.
    //                        The resolver-lambda declared above, can't reference
    //                        or capture the local reference name of the binding.
    for(auto& assignment : mProperties)
    {
        const std::string&     propertyId = assignment.first;
        MetaPropertyVariant_t& property   = assignment.second;

        SR_RESOLVE(SDeserializeResolver, property, aDeserializer, propertyId, deserialized);
    }

    deserialized = deserialized && aDeserializer.commitProperties();

    return deserialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
const InstanceUID_t& CMetaObject::getInstanceUID() const
{
    return mInstanceUID;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
const std::string& CMetaObject::getInstanceName() const
{
    return mInstanceName;
}

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
const CStdSharedPtr_t<CMetaPrototypeBase> CMetaObject::getPrototype() const
{
    return mPrototype;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
const CMetaObject::PropertyMap_t& CMetaObject::getProperties() const
{
    return mProperties;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CMetaObject::PropertyMap_t& CMetaObject::getMutableProperties()
{
    return mProperties;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<--------------------------------------------------------------------------
EPropertySystemError CMetaObject::onPropertyRemoved(const std::string& aPropertyId)
{
    return removeProperty(aPropertyId);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CMetaObject::hasProperty(const std::string& aPropertyId) const
{
    const bool contained = mProperties.contains(aPropertyId);
    return contained;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
EPropertySystemError CMetaObject::getProperty(
        const std::string&            aPropertyId,
        const MetaPropertyVariant_t** aOutProperty) const
{
    SR_RETURN_IF(nullptr == aOutProperty,                      EPropertySystemError::PropertyPointerIsNull);
    SR_RETURN_IF(true    == aPropertyId.empty(),               EPropertySystemError::PropertyIdIsEmpty);
    SR_RETURN_IF(false   == mProperties.contains(aPropertyId), EPropertySystemError::PropertyNotFound);

    const MetaPropertyVariant_t& variant  = mProperties.at(aPropertyId);

    *aOutProperty = &variant;
    return EPropertySystemError::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
EPropertySystemError CMetaObject::getMutableProperty(
        const std::string&      aPropertyId,
        MetaPropertyVariant_t** aOutProperty)
{
    SR_RETURN_IF(nullptr == aOutProperty,                      EPropertySystemError::PropertyPointerIsNull);
    SR_RETURN_IF(true    == aPropertyId.empty(),               EPropertySystemError::PropertyIdIsEmpty);
    SR_RETURN_IF(false   == mProperties.contains(aPropertyId), EPropertySystemError::PropertyNotFound);

    MetaPropertyVariant_t& variant  = mProperties.at(aPropertyId);

    *aOutProperty = &variant;
    return EPropertySystemError::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
uint32_t CMetaObject::getPropertyCount() const
{
    const uint32_t count = mProperties.size();

    return count;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
EPropertySystemError CMetaObject::removeProperty(const std::string& aPropertyId)
{
    SR_RETURN_IF(true  == aPropertyId.empty(),               EPropertySystemError::PropertyIdIsEmpty);
    SR_RETURN_IF(false == mProperties.contains(aPropertyId), EPropertySystemError::PropertyNotFound);

    mProperties.remove(aPropertyId);

    return EPropertySystemError::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CStdSharedPtr_t<CMetaObject> CMetaObject::clone()
{
    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();
    CStdSharedPtr_t<CMetaObject> cloned = nullptr;

    const InstanceUID_t instanceUID = system->generateObjectUID();

    const bool hasPrototype = (nullptr != mPrototype);
    if(hasPrototype)
    {
        cloned = mPrototype->createAbstractInstance(instanceUID, mInstanceName, mProperties);
    }
    else
    {
        // Full copy the object's properties.
        MetaPropertyMap_t properties{};
        MetaPropertyMap_t::MapType_t &internal = properties.internalMap();
        internal.insert(mProperties.begin(), mProperties.end());

        for(auto& [key, variant] : properties)
        {
            const PropertyUID_t uid = system->generatePropertyUID();
            SR_RESOLVE(SSetPropertyUIDResolver, variant, uid);
        }

        CMetaObject* instance = CMetaObject::create(nullptr, instanceUID, mInstanceName, properties);
        cloned = CStdSharedPtr_t<CMetaObject>(instance);
    }

    return cloned;
}
//<-----------------------------------------------------------------------------

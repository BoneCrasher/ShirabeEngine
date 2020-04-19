#include "sr_pch.h"

#include <inttypes.h> // PRIu64 etc...
#include <cstdlib>

#include <nlohmann/json.hpp>
#include <core/base/types/sr_enum.h>
#include "property_system/sr_meta_object.h"
#include "property_system/sr_meta_property.h"
#include "property_system/sr_meta_prototype.h"
#include "property_system/serialization/sr_property_json_de_serializer.h"

/*!
 * Convenience wrapper to check for identity of a type T and a CMetaObject shared pointer.
 */
template <typename TType>
constexpr bool TIsObjectPointer = std::is_same_v<TType, CStdSharedPtr_t<CMetaObject>>;

/*!
 * Convenience wrapper to check for identity of a type T and std::wstring.
 */
template <typename TType>
constexpr bool TIsWideString = std::is_same_v<TType, std::wstring>;

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CPropertySerializer::CSerializationResult::CSerializationResult(const nlohmann::json& aData)
    : mData(std::move(aData))
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::CSerializationResult::asString(std::string& aOutString) const
{
    static constexpr const int32_t sIndent      = 4;
    static constexpr const char    sIndentChar  = ' ';
    static constexpr const bool    sEnsureAscii = true;

    const std::string result = mData.dump(sIndent, sIndentChar, sEnsureAscii);
    aOutString = result;

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::CSerializationResult::asBinaryBuffer(std::vector<uint8_t>& aOutBinaryBuffer) const
{
    // @Review.Note: Went through source code of nlohmann.
    //               Shouldn't throw anything during msgpack serialization.
    //               Only the UBJSON variants throw, which we don't use.
    const std::vector<uint8_t> binary = nlohmann::json::to_msgpack(mData);
    aOutBinaryBuffer = binary;

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::initialize()
{
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::deinitialize()
{
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::serialize(
        const CMetaObject&                                  aSource,
        CStdSharedPtr_t<ISerializer<CMetaObject>::IResult>& aOutResult)
{
    nlohmann::json root{};
    pushObject(root);

    const bool serialized = writeObject(aSource);
    if(serialized)
    {
        CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> result = makeStdSharedPtr<CPropertySerializer::CSerializationResult>(root);

        aOutResult = result;
    }
    else
    {
        aOutResult = nullptr;
    }

    popObject();

    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::writeObject(const CMetaObject& aObject)
{
    // Double dispatch. The object knows, what to serialize...
    const bool serialized = aObject.acceptSerializer(*this);
    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::beginProperties()
{
    nlohmann::json& current = mObjectStack.top();
    current["properties"] = nlohmann::json{};

    nlohmann::json& properties = current["properties"];
    pushObject(properties);

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertySerializer::commitProperties()
{
    popObject();

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CPropertySerializer::writePropertyImpl(
        const std::string&              aIdentifier,
        const CMetaProperty<TDataType>& aProperty)
{
    nlohmann::json object{};
    pushObject(object);

    const bool serialized = aProperty.acceptSerializer(*this);

    popObject();

    nlohmann::json& parent = mObjectStack.top();
    parent[aIdentifier] = object;

    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CPropertySerializer::writeAttributeImpl(const std::string& aIdentifier, const TDataType& aValue)
{
    bool serialized = true;

    nlohmann::json& current = mObjectStack.top();

    if constexpr (TIsObjectPointer<TDataType>)
    {
        nlohmann::json object{};
        pushObject(object);

        if(aValue)
        {
            serialized = writeObject(*aValue);
        }

        popObject();

        current[aIdentifier] = object;
    }
    else if constexpr (TIsWideString<TDataType>)
    {
        const std::string narrowed = CString::narrow(aValue);
        current[aIdentifier] = narrowed;
    }
    else
    {
        current[aIdentifier] = aValue;
    }

    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CPropertySerializer::writeValueListImpl(const std::string& aIdentifier, const std::vector<TDataType>& aValues)
{
    bool serialized = true;

    if constexpr (TIsObjectPointer<TDataType>)
    {
        std::vector<nlohmann::json> serializedObjects{};

        for(const CStdSharedPtr_t<CMetaObject>& object : aValues)
        {
            const bool containsValue = (nullptr != object);
            if(containsValue)
            {
                nlohmann::json jsonObject{};
                pushObject(jsonObject);

                serialized = writeObject(*object);

                popObject();

                serializedObjects.push_back(jsonObject);
            }
            else
            {
                serializedObjects.push_back(nullptr); // Will write "null"
            }
        }

        nlohmann::json& current = mObjectStack.top();
        current[aIdentifier] = serializedObjects;
    }
    else
    {
        nlohmann::json list{};

        for(uint32_t k=0; k<aValues.size(); ++k)
        {
            if constexpr (std::is_same_v<TDataType, std::wstring>)
            {
                const std::wstring& widened  = aValues.at(k);
                const std::string   narrowed = CString::narrow(widened);
                list[k] = narrowed;
            }
            else
            {
                list[k] = aValues.at(k);
            }
        }

        nlohmann::json& current = mObjectStack.top();
        current[aIdentifier] = list;
    }

    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CPropertySerializer::pushObject(Object_t& aJSONObjectInstance)
{
    mObjectStack.push(aJSONObjectInstance);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CPropertySerializer::popObject()
{
    mObjectStack.pop();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CPropertyDeserializer::CDeserializationResult::CDeserializationResult(
        CStdSharedPtr_t<CMetaObject> aObject,
        const PathMap_t&             aPropertyPathMap)
    : mObject(aObject)
    , mPropertyPathMap(aPropertyPathMap)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::CDeserializationResult::asT(CStdSharedPtr_t<CMetaObject>& aOutResult) const
{
    if(!mObject)
    {
        aOutResult = nullptr;
        return false;
    }

    aOutResult = mObject;
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::CDeserializationResult::getPropertyPathMap(PathMap_t& aOutPathMap) const
{
    aOutPathMap = mPropertyPathMap;
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CPropertyDeserializer::CPropertyDeserializer(const bool aUseDynamicDeserialization)
    : mUseDynamicDeserialization(aUseDynamicDeserialization)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::initialize()
{
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::deinitialize()
{
    mAdjacencyLists.clear();
    mPropertyReferences.clear();

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::deserialize(
        const std::string&                                    aSource,
        CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult)
{
    // Important @ Developer: For some reason I had to store the result returned
    //                        from deserializeFwd into a temporary object.
    //                        Could be a compiler bug. Will review this (MBT).
    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> result = nullptr;

    nlohmann::json deserializedJSON = nlohmann::json::parse(aSource);

    const bool objectSuccessfullyRead =  deserializeFwd(deserializedJSON, result);
    if(objectSuccessfullyRead)
    {
        aOutResult = result;
    }
    else
    {
        aOutResult = nullptr;
    }

    return objectSuccessfullyRead;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::deserialize(
        const std::vector<uint8_t>&                           aSource,
        CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult)
{
    // Important @ Developer: For some reason I had to store the result returned
    //                        from deserializeFwd into a temporary object.
    //                        Could be a compiler bug. Will review this (MBT).
    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> result = nullptr;

    nlohmann::json deserializedJSON = nlohmann::json::from_msgpack(aSource);

    const bool objectSuccessfullyRead = deserializeFwd(deserializedJSON, result);
    if(objectSuccessfullyRead)
    {
        aOutResult = result;
    }
    else
    {
        aOutResult = nullptr;
    }

    return objectSuccessfullyRead;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

// Forward declaration. Documentation in definition.
bool compilePathMap(
        const CPropertyDeserializer::AdjacencyListMap_t& aAdjacencyLists,
        const CPropertyDeserializer::PropertyRefMap_t&   aPropertyReferences,
        const CPropertyDeserializer::NodeId_t&           aRootNodeId,
        CPropertyDeserializer::PathMap_t&                aOutPathMap);
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::deserializeFwd(
        const nlohmann::json&                                 deserializedJSON,
        CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult)
{
    pushObject(deserializedJSON);

    CStdSharedPtr_t<CMetaObject> instance = nullptr;

    const bool objectSuccessfullyRead = readObjectFwd(instance);
    if(objectSuccessfullyRead)
    {
        PathMap_t pathMap{};
        const bool pathMapCompiled = compilePathMap(mAdjacencyLists, mPropertyReferences, instance->getInstanceUID(), pathMap);
        SR_RETURN_IF(!pathMapCompiled, false)

        CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> result = makeStdSharedPtr<CDeserializationResult>(instance, pathMap);
        aOutResult = result;
    }
    else
    {
        aOutResult = nullptr;
    }

    popObject();

    return objectSuccessfullyRead;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/*!
 * Create an empty typed CMetaProperty and push it to a variant holder.
 *
 * @param aOutVariant A variant out reference holding the result of creation.
 * @return            True, if successful. False, otherwise.
 */
template <typename T>
bool createEmptyProperty(
        const PropertyUID_t&   aUID,
        MetaPropertyVariant_t& aOutVariant)
{
    // Any error case?
    const CMetaProperty<T> property = { aUID, "",  T() };
    MetaPropertyVariant_t  variant  = property;

    aOutVariant = variant;
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/*!
 * Reads a properties valueType and invokes creation of a typed CMetaProperty instance.
 *
 * @param aProperty           The JSONified property from nlohmann::json.
 * @param aOutPropertyVariant A variant out reference holding the result of creation.
 * @return                    True, if successful. False otherwise.
 */
bool determinePropertyForDynamicDeserialization(
        const nlohmann::json&  aProperty,
        MetaPropertyVariant_t& aOutPropertyVariant)
{
    using ValueType_t = CMetaPropertyCore::EPropertyValueType;

    const CEnum::UnderlyingType_t<ValueType_t> valueTypeValue = aProperty["valuetype"];
    const ValueType_t                          valueType      = static_cast<ValueType_t>(valueTypeValue);

    const PropertyUID_t& uid = aProperty["uid"];

    MetaPropertyVariant_t variant{};

    bool deserialized = true;

    switch(valueType)
    {
    default:
    case ValueType_t::Undefined:
        break;
    case ValueType_t::Int8:
        deserialized = deserialized && createEmptyProperty<int8_t>(uid, variant);
        break;
    case ValueType_t::Int16:
        deserialized = deserialized && createEmptyProperty<int16_t>(uid, variant);
        break;
    case ValueType_t::Int32:
        deserialized = deserialized && createEmptyProperty<int32_t>(uid, variant);
        break;
    case ValueType_t::Int64:
        deserialized = deserialized && createEmptyProperty<int64_t>(uid, variant);
        break;
    case ValueType_t::UInt8:
        deserialized = deserialized && createEmptyProperty<uint8_t>(uid, variant);
        break;
    case ValueType_t::UInt16:
        deserialized = deserialized && createEmptyProperty<uint16_t>(uid, variant);
        break;
    case ValueType_t::UInt32:
        deserialized = deserialized && createEmptyProperty<uint32_t>(uid, variant);
        break;
    case ValueType_t::UInt64:
        deserialized = deserialized && createEmptyProperty<uint64_t>(uid, variant);
        break;
    case ValueType_t::Float:
        deserialized = deserialized && createEmptyProperty<float>(uid, variant);
        break;
    case ValueType_t::Double:
        deserialized = deserialized && createEmptyProperty<double>(uid, variant);
        break;
    case ValueType_t::String:
        deserialized = deserialized && createEmptyProperty<std::string>(uid, variant);
        break;
    case ValueType_t::WString:
        deserialized = deserialized && createEmptyProperty<std::wstring>(uid, variant);
        break;
    case ValueType_t::Object:
        deserialized = deserialized && createEmptyProperty<CStdSharedPtr_t<CMetaObject>>(uid, variant);
        break;
    }

    aOutPropertyVariant = variant;

    return deserialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

/*!
 * Attempts to enter the "properties block" of an object and determine all properties available.
 * Foreach property detected, a typed empty property instance will be created and stored in 'aOutMap'.
 *
 * @param aObject The nlohmann::json object optionally containing properties.
 * @param aOutMap The map holding an assignment of propertyId to empty property instance.
 * @return        True, if successful, false otherwise.
 */
bool determinePropertiesForDynamicDeserialization(
        const nlohmann::json& aObject,
        MetaPropertyMap_t&    aOutMap)
{
    using ConstIterator_t = nlohmann::json::const_iterator;

    const bool propertiesContained = aObject.find("properties") != aObject.end();
    SR_RETURN_IF(false == propertiesContained, true); // It is valid to not have properties!

    const nlohmann::json& properties = aObject.at("properties");
    for (ConstIterator_t it = properties.begin(); it != properties.end(); ++it)
    {
        const nlohmann::json& property = it.value();

        MetaPropertyVariant_t variant{};

        const bool determined = determinePropertyForDynamicDeserialization(property, variant);
        SR_RETURN_IF(false == determined, false);

        aOutMap[it.key()] = variant;
    }

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
/*!
 * Creates a resolver usable within std::visit to fetch an iterator to an element
 * inside an object's property map and store it in a property reference map.
 *
 * @tparam T The underlying type of the property.
 */
template <typename T>
struct SGetPropertyReferenceResolver
{
    static auto getResolver(
            CPropertyDeserializer::PropertyRefMap_t& aPropertyReferences,
            CPropertyDeserializer::PropertyMap_t&    aPropertyMap,
            const std::string&                       aUID)
    {
        const auto fn = [&] (CMetaProperty<T> &aResolvedInstance) -> void
        {
            aPropertyReferences[aResolvedInstance.getPropertyUID()] = aPropertyMap.find(aUID);
        };
        return fn;
    }
};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::readObjectFwd(CStdSharedPtr_t<CMetaObject> &aOutObject)
{
    const nlohmann::json& current = mObjectStack.top();

    std::string   prototypeId  = "";
    std::string   instanceName = "";
    InstanceUID_t instanceUID  = 0;

    readAttribute("uid",  instanceUID);
    readAttribute("name", instanceName);

    if(0 == instanceUID)
    {
        CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();
        instanceUID = system->generateObjectUID();
    }

    CStdSharedPtr_t<CMetaObject> instance = nullptr;

    if(mUseDynamicDeserialization) // Read all properties, which can be found!
    {
        MetaPropertyMap_t map{};

        const bool hasProperties = determinePropertiesForDynamicDeserialization(current, map);
        SR_UNUSED(hasProperties);

        CMetaObject* object = CMetaObject::create(nullptr, instanceUID, instanceName, map);
        instance = CStdSharedPtr_t<CMetaObject>(object);
    }
    else
    {
        readAttribute("prototypeId", prototypeId);

        CStdSharedPtr_t<CMetaSystem>        system    = CMetaSystem::get();
        CStdSharedPtr_t<CMetaPrototypeBase> prototype = system->getPrototype<CMetaPrototypeBase>(prototypeId);

        instance  = prototype->createAbstractInstance(instanceUID, instanceName);
    }

    pushAdjacencyLevel(instanceUID);

    const bool objectSuccessfullyRead = readObject(*instance);
    if(objectSuccessfullyRead)
    {
        // PathMap-generation: Fetching property references here.
        //   Important:
        //      We store iterators into the maps, as they won't be invalidated as long as
        //      the desired map is always the same instance.
        CMetaObject::PropertyMap_t &properties = instance->getMutableProperties();
        for(auto& [id, property] : properties)
        {
            const std::string&     uid             = id;
            MetaPropertyVariant_t& propertyVariant = properties.at(uid);

            SR_RESOLVE(SGetPropertyReferenceResolver, propertyVariant, mPropertyReferences, properties, uid);
        }

        aOutObject = instance;
    }
    else
    {
        aOutObject = nullptr;
    }

    popAdjacencyLevel();

    return objectSuccessfullyRead;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::readObject(CMetaObject& aObject)
{
    // @Code.Review: Temporary
    // std::cerr << "Reading object\n";

    const bool deserialized = aObject.acceptDeserializer(*this);
    return deserialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::beginProperties()
{
    const nlohmann::json& current = mObjectStack.top();

    const bool propertyContained = current.find("properties") != current.end();
    SR_RETURN_IF(false == propertyContained, false);

    const nlohmann::json& properties = current.at("properties");
    pushObject(properties);

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool CPropertyDeserializer::commitProperties()
{
    popObject();

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CPropertyDeserializer::readPropertyImpl(
        const std::string&        aIdentifier,
        CMetaProperty<TDataType>& aProperty)
{
    // @Code.Review: Temporary
    // std::cerr << "Reading property " << aIdentifier << "\n";

    const nlohmann::json& current = mObjectStack.top();

    const bool propertyContained = current.find(aIdentifier) != current.end();
    SR_RETURN_IF(false == propertyContained, false);

    const nlohmann::json& propertyJSON = current.at(aIdentifier);

    const bool propertyIsNull = propertyJSON.is_null();
    if(propertyIsNull)
    {
        aProperty = CMetaProperty<TDataType>();
        return true;
    }

    PropertyUID_t propertyUID = propertyJSON["uid"];
    if(0 == propertyUID)
    {
        CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();
        propertyUID = system->generatePropertyUID();
    }

    pushAdjacencyLevel(propertyUID);
    pushObject(propertyJSON);

    const bool deserialized = aProperty.acceptDeserializer(*this);

    popObject();
    popAdjacencyLevel();

    return deserialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CPropertyDeserializer::readAttributeImpl(const std::string& aIdentifier, TDataType& aValue)
{
    // @Code.Review: Temporary
    //std::cerr << "Reading attribute " << aIdentifier << "\n";

    const nlohmann::json& current = mObjectStack.top();

    const bool attributeContained = current.find(aIdentifier) != current.end();
    SR_RETURN_IF(false == attributeContained, false);

    const nlohmann::json& attributeJSON = current.at(aIdentifier);

    const bool attributeIsNull = attributeJSON.is_null();
    if(attributeIsNull)
    {
        aValue = TDataType();
        return true;
    }

    if constexpr (TIsObjectPointer<TDataType>)
    {
        SR_RETURN_IF(false == attributeJSON.is_object(), false);

        CStdSharedPtr_t<CMetaObject> instance = nullptr;

        pushObject(attributeJSON);

        const bool deserialized = readObjectFwd(instance);
        if(deserialized)
        {
            aValue = instance;
        }

        popObject();

        return deserialized;
    }
    else if constexpr (TIsWideString<TDataType>)
    {
        SR_RETURN_IF(false == attributeJSON.is_string(), false);

        const std::string& narrowed = attributeJSON;
        const std::wstring widened  = CString::widen(narrowed);
        aValue = widened;

        return true;
    }
    else
    {
        const TDataType attributeValue = attributeJSON;
        aValue = attributeValue;

        return true;
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CPropertyDeserializer::readValueListImpl(const std::string& aIdentifier, std::vector<TDataType>& aValues)
{
    // @Code.Review: Temporary
    //std::cerr << "Reading value list " << aIdentifier << "\n";

    const nlohmann::json& current = mObjectStack.top();

    const bool valueContained = current.find(aIdentifier) != current.end();
    SR_RETURN_IF(false == valueContained, false);

    const nlohmann::json& listJSON = current.at(aIdentifier);

    const bool valueListIsNull = listJSON.is_null();
    if(valueListIsNull)
    {
        aValues = std::vector<TDataType>();
        return true;
    }

    const bool valueIsArray = listJSON.is_array();
    SR_RETURN_IF(false == valueIsArray, false);

    std::vector<TDataType> list{};

    if(!listJSON.empty())
    {
        for(const nlohmann::json& valueJSON : listJSON)
        {
            if constexpr (std::is_same_v<TDataType, CStdSharedPtr_t<CMetaObject>>)
            {
                CStdSharedPtr_t<CMetaObject> instance = nullptr;

                const bool isNull = valueJSON.is_null();
                if(!isNull)
                {
                    pushObject(valueJSON);

                    const bool deserialized = readObjectFwd(instance);
                    SR_RETURN_IF(false == deserialized, false);

                    popObject();
                }

                list.push_back(std::move(instance));
            }
            else if constexpr (std::is_same_v<TDataType, std::wstring>)
            {
                const std::string  str  = valueJSON;
                const std::wstring wstr = CString::widen(str);

                list.push_back(wstr);
            }
            else
            {
                const TDataType& value = valueJSON;
                list.push_back(value);
            }
        }
    }

    aValues = list;

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CPropertyDeserializer::pushObject(const Object_t& aObjectRef)
{
    mObjectStack.push(aObjectRef);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CPropertyDeserializer::popObject()
{
    mObjectStack.pop();
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CPropertyDeserializer::pushAdjacencyLevel(const NodeId_t& aNodeId)
{
    if(!mAdjacencyStack.empty())
    {
        mCurrentAdjacencyList->get().push_back(aNodeId);
    }

    mCurrentAdjacencyList = mAdjacencyLists[aNodeId]; // Implicit creation
    mAdjacencyStack.push(aNodeId);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CPropertyDeserializer::popAdjacencyLevel()
{
    // If the stack is empty, assume clean state and perform no-op.
    if(mAdjacencyStack.empty())
    {
        return;
    }

    mAdjacencyStack.pop();

    // If the last element was popped from the stack, subsequent access to a "top" element
    // would fail...
    if(mAdjacencyStack.empty())
    {
        mCurrentAdjacencyList.reset();
        return;
    }

    const NodeId_t& parentItemId = mAdjacencyStack.top();
    mCurrentAdjacencyList = mAdjacencyLists[parentItemId];
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------


/*!
 * Algorithm to generate a path-map from a map of adjacency lists.
 *
 * The algorithm is based on the adjacency-list, depth-first-sorting, with
 * small modifications.
 *
 * @param aAdjacencyLists Map of adjacency lists, mapping a NodeId to an array of other NodeIds,
 *                        resembling graph-edges.
 * @param aPropertyRefMap Map of nodeId -> iterator into the internal property-maps of the object instances.
 * @param aRootNodeId     The root-object's nodeId to begin the algorithm with.
 * @param aOutPathMap     A map of string -> iterator into the internal property-maps of the object instances.
 * @return                True, if the map generation was successful. False otherwise.
 */
bool compilePathMap(
        const CPropertyDeserializer::AdjacencyListMap_t& aAdjacencyLists,
        const CPropertyDeserializer::PropertyRefMap_t&   aPropertyRefMap,
        const CPropertyDeserializer::NodeId_t&           aRootNodeId,
        CPropertyDeserializer::PathMap_t&                aOutPathMap)
{
    // Convenience alias to keep code readable.
    using AdjacencyListMap_t = CPropertyDeserializer::AdjacencyListMap_t;
    using AdjacencyList_t    = CPropertyDeserializer::AdjacencyList_t;
    using NodeId_t           = CPropertyDeserializer::NodeId_t;
    using VisitedEdgeMap_t   = std::map<CPropertyDeserializer::NodeId_t, bool>;
    using PathMap_t          = CPropertyDeserializer::PathMap_t;

    // Recursive function declaration, called for edge in the graph.
    std::function<
      void(
        const AdjacencyListMap_t&,/* Map of adjacency lists for next edge access */
        const NodeId_t&          ,/* Current edge-root node id to be processed. */
        VisitedEdgeMap_t&        ,/* Map of edge-roots to "already visited flag" to avoid circularity */
        const std::string&       ,/* Path of the parent property, appended by the currently processed properties' index in the parent property. */
        PathMap_t&                /* Output reference to the path map */)>
            CompileFn_i;

    // Effective function definition, which had to be separated for recursive calls to work.
    CompileFn_i = [&](
      const AdjacencyListMap_t& aEdges,
      const NodeId_t&           aNode,
      VisitedEdgeMap_t&         aVisitedEdges,
      const std::string&        aCurrentPath,
      PathMap_t&                aOutPathMap) -> void
    {
        const bool edgeVisited = aVisitedEdges[aNode];
        SR_RETURN_VOID_IF(edgeVisited);

        aVisitedEdges[aNode] = true;

        // Only proceed if a specific node effectively has outgoing edges, since the path map
        // only stores leaf-nodes, i.e. any property, which is not of type Object.
        const bool edgeContained = aEdges.contains(aNode);
        if(edgeContained)
        {
            std::string path = aCurrentPath;

            // Check, whether the node is a property and generate the property path!
            // For object-nodes, the path is equal to the parent properties' path.
            const bool isProperty = aPropertyRefMap.contains(aNode);
            if(isProperty)
            {
                CPropertyDeserializer::PropertyMap_t::iterator property     = aPropertyRefMap.at(aNode);
                std::string                                    propertyName = "";

                MetaPropertyVariant_t& variant = property->second;

                SR_RESOLVE(SGetPropertyNameResolver, variant, propertyName);

                path = CString::formatString("%s/%s", path.c_str(), propertyName.c_str());
            }

            AdjacencyList_t adjacencyList = aEdges.at(aNode);

            // If no adjacent nodes are available, we found a leaf-property, i.e. not of type Object!
            const bool noAdjacentNodes = adjacencyList.empty();
            if(noAdjacentNodes)
            {
                CPropertyDeserializer::PropertyMap_t::iterator refWrapper = aPropertyRefMap.at(aNode);

                MetaPropertyVariant_t& variant = refWrapper->second;
                SR_RESOLVE(SSetPropertyPathResolver, variant, path);

                aOutPathMap.add(path, refWrapper);
            }
            else
            {
                uint16_t    k         = 0;
                std::string localPath = path;

                for(const NodeId_t& adjacent : aEdges.at(aNode))
                {
                    // Append the descendent property's index in the current parent to the path,
                    // to avoid name-clashes in subpaths and permit path-templates to work
                    // with indexed access for Chain-type properties.
                    localPath = path;
                    if(isProperty)
                    {
                        localPath = CString::formatString("%s/%" PRIu16, path.c_str(), k);
                        ++k;
                    }

                    CompileFn_i(aEdges, adjacent, aVisitedEdges, localPath, aOutPathMap);
                }
            }
        }
    };

    // Make sure that "no edges were visited".
    VisitedEdgeMap_t visitedEdges {};
    for(const auto& [k, v] : aAdjacencyLists)
    {
        visitedEdges[k] = false;
    }

    // Process root object!
    // Imporant:
    //     Since the object is no leaf, the path is empty (see fourth param).
    CompileFn_i(aAdjacencyLists, aRootNodeId, visitedEdges, "", aOutPathMap);

    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

// Explicit instantiations for the template functions.
#define SR_INSTANTIATE_WRITE_PROPERTY(_aType) \
    template bool CPropertySerializer::writePropertyImpl<_aType>(const std::string&, const CMetaProperty<_aType>&);

#define SR_INSTANTIATE_WRITE_ATTRIBUTE(_aType) \
    template bool CPropertySerializer::writeAttributeImpl<_aType>(const std::string&, const _aType&);

#define SR_INSTANTIATE_WRITE_VALUE_LIST(_aType) \
    template bool CPropertySerializer::writeValueListImpl<_aType>(const std::string&, const std::vector<_aType>&);

#define SR_INSTANTIATE_READ_PROPERTY(_aType) \
    template bool CPropertyDeserializer::readPropertyImpl<_aType>(const std::string&, CMetaProperty<_aType>&);

#define SR_INSTANTIATE_READ_ATTRIBUTE(_aType) \
    template bool CPropertyDeserializer::readAttributeImpl<_aType>(const std::string&, _aType&);

#define SR_INSTANTIATE_READ_VALUE_LIST(_aType) \
    template bool CPropertyDeserializer::readValueListImpl<_aType>(const std::string&, std::vector<_aType>&);

SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_WRITE_PROPERTY)
SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_WRITE_ATTRIBUTE)
SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_WRITE_VALUE_LIST)
SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_READ_PROPERTY)
SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_READ_ATTRIBUTE)
SR_APPLY_FOREACH_PROPERTY_TYPE(SR_INSTANTIATE_READ_VALUE_LIST)

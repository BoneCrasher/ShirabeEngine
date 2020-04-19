/*!
 * @file      sr_property_de_serializer.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      11/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <iostream>
#include <stack>

#include <nlohmann/json.hpp>
#include <core/serialization/sr_serialization.h>

#include "property_system/serialization/sr_property_de_serialization_interface.h"
#include "property_system/sr_meta_object.h"
#include "property_system/sr_meta_property.h"

//--------------------------------------------------------------------------------------------------------
// Double-Dispatched CMetaObject De-/Serialization using nlohmann::json
//--------------------------------------------------------------------------------------------------------
//
// References:
// - nlohmann::json: https://github.com/nlohmann/json
// - DoubleDispatch: https://refactoring.guru/design-patterns/visitor-double-dispatch
// - if constexpr:   https://en.cppreference.com/w/cpp/language/if
//
// The de-/serialization mechanism relies on double dispatching between a CDe/-Serializer-instance
// and instances of CMetaObject or CMetaProperty<T>, where as there are 13 types permitted for T.
//
// To avoid signature and method bloating, even though most of them are uniformly implemented,
// the property-related functionality is implemented as templates.
//
// The number of supported types also imply one specialty about the de-/serialization process:
//   10 of the 13 types are numeric, i.e. they can easily be de-/serialized without any issue.
//   The other three types, namely CStdSharedPtr_t<CMetaObject>, std::string and std::wstring
//   need some more careful handling.
//
//   Although std::string can easily be converted into an ASCII-stream (requirement of nlohmann::json),
//   the std::wstring has to be narrowed first, so that it can be stored as an UTF-8 ASCII representation.
//
//   The CStdSharedPtr_t<CMetaObject> is a complex type and introduces a new level of hierarchy into the
//   JSON tree, consequently it has to be handled differently, which will become visible in several
//   places employing an "if constexpr"-construct.
//
// To avoid the objects and properties being publicly exposed, the double dispatch is employed,
// causing the de-/serializers to invoke "acceptDe-/Serializer(...)" on the objects/properties,
// which in turn only require the calls "read/write~" of either "Attribute/Object/Property/ValueList".
//
// The De-/Serialization uses an internal stack of nlohmann::json instances.
// For each hierarchy level, such an instance is created and pushed on to the stack, so that all
// subsequent operations are performed on the stack's current top item.
//
// Several reusable helper methods are employed for this, specifically for the deserialization process.
//
//--------------------------------------------------------------------------------------------------------

/*!
 * The CPropertySerializer implements the double dispatch mechanism of ISerializer<CMetaObject> to JSON format
 * using nlohmann::json.
 *
 * @details The class provides the "serialize(...)" method, performing serialization on a CMetaObject
 *          and returns an instance of CSerializationResult, which in turn provides the results either
 *          as string or binary stream.
 */
class CPropertySerializer
        : public IPropertySerializer
{
public_typedefs:
    using Object_t    = nlohmann::json;
    using ObjectRef_t = std::reference_wrapper<Object_t>;

public_classes:
    /*!
     * A CSerializationResult holds the serialized CMetaObject-tree as a nlohmann::json object
     * and provides methods to get the results either as string or binary stream.
     */
    class CSerializationResult
            : public ISerializer<CMetaObject>::IResult
    {
    public_constructors:
        /*!
         * Initialize this result with the nlohmann::json root, containing
         * a fully serialized CMetaObject tree.
         *
         * @param [in] aData nlohmann::json serialized object tree.
         */
        CSerializationResult(const nlohmann::json& aData);

    public_methods:
        /*!
         * Convert the internal data to string and return it.
         *
         * @param [out] aOutString Placeholder for the string representation to be returned.
         * @return                 True, if successful. False, otherwise.
         */
        bool asString(std::string& aOutString) const;

        /*!
         * Convert the internal data to a byte buffer and return it.
         *
         * @param [out] aOutBuffer Placeholder for the byte buffer representation to be returned.
         * @return                 True, if successful. False, otherwise.
         */
        bool asBinaryBuffer(std::vector<uint8_t>& aOutBuffer) const;

    private_members:
        const nlohmann::json mData;
    };

public_methods:
    /*!
     * Make this serializer ready for operation.
     *
     * @return True if successful. False otherwise.
     */
    bool initialize();
    /*!
     * Stop operation, clear and destroy.
     *
     * @return True if successful. False otherwise.
     */
    bool deinitialize();

    /*!
     * Accepts a CMetaObject reference and performs depth-first traversing serialization of the CMetaObject
     * tree. The results, if successful, will be stored in a CSerializationResult instance..
     *
     * @param [in]  aSource    The CMetaObject-tree root object to serialize.
     * @param [out] aOutResult CSerializationResult handle providing the results, if serialization was successful.
     * @return                 True if successful, false otherwise.
     */
    bool serialize(const CMetaObject& aSource, CStdSharedPtr_t<ISerializer<CMetaObject>::IResult>& aOutResult);

    /*!
     * Serialize an object instance into the current nlohmann::json instance pushed on the stack.
     *
     * @param [in] aObject The object instance to serialize.
     * @return             True, if the entire serialization tree of aObject was serialized successfully.
     *                     False otherwise.
     */
    bool writeObject(const CMetaObject& aObject);

    /*!
     * Write a "properties" block and have all subsequent property writes take place in it.
     *
     * @return True, if successful. False otherwise.
     */
    bool beginProperties();

    /*!
     * Pop the properties block from the stack and return to the object block.
     *
     * @return True, if successful. False otherwise.
     */
    bool commitProperties();

    #define SR_DEFINE_WRITE_PROPERTY(_aType) \
        /*!
         * Serialize a named property instance into the current nlohmann::json instance pushed on the stack.
         *
         * @param  [in] aIdentifier The name of the property to serialize.
         * @param  [in] aProperty   The CMetaProperty<T> instance to serialize.
         * @return                  True, if the entire serialization tree of aProperty was serialized successfully.
         *                          False otherwise.
         */                                                           \
        SR_INLINE bool writeProperty(                                 \
            const std::string&           aIdentifier,                 \
            const CMetaProperty<_aType>& aProperty)                   \
        {                                                             \
            return writePropertyImpl<_aType>(aIdentifier, aProperty); \
        }

    #define SR_DEFINE_WRITE_ATTRIBUTE(_aType) \
        /*!
         * Serialize a named attribute into the current nlohmann::json instance pushed on the stack.
         *
         * @tparam T                The underlying data type of the attribute.
         * @param  [in] aIdentifier The name of the attribute to serialize.
         * @param  [in] aValue      The attribute value to serialize.
         * @return                  True, if the entire serialization tree of aValue was serialized successfully.
         *                          False otherwise.
         */                                                          \
        SR_INLINE bool writeAttribute(                               \
            const std::string& aIdentifier,                          \
            const _aType&      aValue)                               \
        {                                                            \
            return writeAttributeImpl<_aType>(aIdentifier, aValue);  \
        }

    #define SR_DEFINE_WRITE_VALUELIST(_aType) \
        /*!
         * Serialize a named list of values into a JSON list in the current nlohmann::json instance pushed on the stack.
         * @tparam T                The underlying data type of the value list.
         * @param  [in] aIdentifier The name of the list to serialize.
         * @param  [in] aValues     The list of values to serialize.
         * @return                  True, if the entire serialization trees of aValues were serialized successfully.
         *                          False otherwise.
         */                                                          \
        SR_INLINE bool writeValueList(                               \
            const std::string&         aIdentifier,                  \
            const std::vector<_aType>& aValues)                      \
        {                                                            \
            return writeValueListImpl<_aType>(aIdentifier, aValues); \
        }

    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DEFINE_WRITE_PROPERTY)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DEFINE_WRITE_ATTRIBUTE)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DEFINE_WRITE_VALUELIST)

private_methods:

    /*!
     * Serialize a named property instance into the current nlohmann::json instance pushed on the stack.
     *
     * @tparam T                The underlying data type of the property.
     * @param  [in] aIdentifier The name of the property to serialize.
     * @param  [in] aProperty   The CMetaProperty<T> instance to serialize.
     * @return                  True, if the entire serialization tree of aProperty was serialized successfully.
     *                          False otherwise.
     */
    template <typename T>
    bool writePropertyImpl(const std::string& aIdentifier, const CMetaProperty<T>& aProperty);

    /*!
     * Serialize a named attribute into the current nlohmann::json instance pushed on the stack.
     *
     * @tparam T                The underlying data type of the attribute.
     * @param  [in] aIdentifier The name of the attribute to serialize.
     * @param  [in] aValue      The attribute value to serialize.
     * @return                  True, if the entire serialization tree of aValue was serialized successfully.
     *                          False otherwise.
     */
    template <typename T>
    bool writeAttributeImpl(const std::string& aIdentifier, const T& aValue);

    /*!
     * Serialize a named list of values into a JSON list in the current nlohmann::json instance pushed on the stack.
     * @tparam T                The underlying data type of the value list.
     * @param  [in] aIdentifier The name of the list to serialize.
     * @param  [in] aValues     The list of values to serialize.
     * @return                  True, if the entire serialization trees of aValues were serialized successfully.
     *                          False otherwise.
     */
    template <typename T>
    bool writeValueListImpl(const std::string& aIdentifier, const std::vector<T>& aValues);


    /*!
     * Push a new nlohmann::json instance onto the internal stack to have all subsequent operations
     * be performed on this object.
     *
     * @param [in] aJSONObjectInstance The externally provided nlohmann::json instance as mutable reference.
     */
    void pushObject(Object_t& aJSONObjectInstance);
    /*!
     * Pop the top-most nlohmann::json instance from the internal stack to have all subsequent operations
     * be performed on the parent nlohmann::json instance.
     */
    void popObject();

private_members:
    std::stack<ObjectRef_t> mObjectStack;
};

/*!
 * The CPropertyDeserializer implements the double dispatch mechanism of IDeserializer<CMetaObject> to JSON
 * format using nlohmann::json.
 *
 * @details The class provides the "deserialize(...)" method, performing deserialization on a CMetaObject
 *          and returns an instance of CDeserializationResult, which in turn provides a
 *          CStdSharedPtr_t<CMetaObject> instance containing the deserialized results.
 */
class CPropertyDeserializer
        : public IPropertyDeserializer
{
public_typedefs:
    using Object_t           = nlohmann::json;
    using ObjectRef_t        = std::reference_wrapper<const Object_t>;

    // These typedefs are used for the adjacency list setup and property path generation.

    /*!
     * Stores a 64-bit unique id of a adjacency node.
     */
    using NodeId_t           = GenericUID_t;
    /*!
     * Stores a list of adjacent nodes by id IN SEQUENCE AS ENCOUNTERED.
     */
    using AdjacencyList_t    = std::vector<NodeId_t>;
    /*!
     * Maps an adjacency list to a specific nodeId to resemble edges from nodeId -> list[k] foreach k.
     */
    using AdjacencyListMap_t = CMap<NodeId_t, AdjacencyList_t>;
    /*!
     * Optional-wrapper to store (or not) an adjacency list by reference for clean "has adjacent nodes" checking.
     */
    using AdjacencyListRef_t = std::optional<std::reference_wrapper<AdjacencyList_t>>;

    /*!
     * Realiasing of the CMetaObject's PropertyMap_t for convenience.
     */
    using PropertyMap_t      = CMetaObject::PropertyMap_t;

    /*!
     * Mapping from nodeId to shared pointer to CMetaObject, if a node is an object instance.
     */
    using ObjectRefMap_t     = CMap<NodeId_t,    CStdSharedPtr_t<CMetaObject>>;
    /*!
     * Mapping from nodeId to property map iterator, if a node is a propertyinstance.
     */
    using PropertyRefMap_t   = CMap<NodeId_t,    PropertyMap_t::iterator>;

    /*!
     * Path map type, mapping a property-path (e.g. /Imagers/0/RegionOfInterest/1) to the respective property in the property-tree.
     */
    using PathMap_t          = CMap<std::string, PropertyMap_t::iterator>;

public_classes:
    /*!
     * A CDeserializationResult holds the deserialized CMetaObject-tree as shared pointer as well as
     * the generated path-map to access the properties and provides methods to access the data.
     */
    class CDeserializationResult
            : public IDeserializer<CMetaObject>::IResult
    {
    public_constructors:
        /*!
         * Initializes this result with a CMetaObject pointer containing the fully deserialized
         * nlohmann::json tree.
         *
         * @param [out] aOutObject          Pointer-handle which will be initialized to the internal object representation
         *                                  of the deserialized data.
         * @param [out] aOutPropertyPathMap Placeholder to store the deserialized path-map into.
         */
        CDeserializationResult(
                CStdSharedPtr_t<CMetaObject> aOutObject,
                const PathMap_t&             aOutPropertyPathMap);

    public_methods:
        bool asT(CStdSharedPtr_t<CMetaObject>& aOutResult) const;

        /*!
         * Access the property path map generated on deserialization.
         *
         * @param  [out] aOutPathMap Placeholder to copy the property path map to.
         * @return                   True, if successful. False otherwise.
         */
        bool getPropertyPathMap(PathMap_t& aOutPathMap) const;

    private_members:
        CStdSharedPtr_t<CMetaObject> mObject;
        PathMap_t                    mPropertyPathMap;
    };

public_constructors:
    /*!
     * Construct a deserializer in dynamic or prototype based derialization mode.
     *
     * @param [in] aUseDynamicDeserialization Use dynamic deserialization or not. Default: False.
     * @remarks                               Dynamic deserialization means, that no prototypes are accessed matched
     *                                        but whatever properties are listed are stored in a generic CMetaObject-instance.
     *                                        This is required to work around binary-divergence between API and Sensor, while
     *                                        still permitting full sensor state replication.
     */
    CPropertyDeserializer(const bool aUseDynamicDeserialization = false);

public_methods:
    /*!
     * Make this serializer ready for operation.
     *
     * @return True if successful. False otherwise.
     */
    bool initialize();
    /*!
     * Stop operation, clear and destroy.
     *
     * @return True if successful. False otherwise.
     */
    bool deinitialize();

    /*!
     * Deserialize a string representation of a JSON tree into a CMetaObject.
     *
     * @param [in]  aSource    Valid JSON string to deserialize.
     * @param [out] aOutResult A CDeserializationResult containing the deserialized JSON as a CMetaObject.
     * @return                 True, if the entire JSON tree was deserialized properly. False otherwise.
     */
    bool deserialize(
            const std::string&                                    aSource,
            CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult);

    /*!
     * Deserialize a byte buffer representation of a JSON tree into a CMetaObject.
     *
     * @param [in]  aSource    Valid JSON byte buffer to deserialize.
     * @param [out] aOutResult A CDeserializationResult containing the deserialized JSON as a CMetaObject.
     * @return                 True, if the entire JSON tree was deserialized properly. False otherwise.
     */
    bool deserialize(
            const std::vector<uint8_t>&                           aSource,
            CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult);

    /*!
     * Read the current nlohmann::json object into a CMetaObject instance.
     *
     * @param [in] aObject An object ref to fill up with data from the current stack's topmost nlohmann::json.
     * @return             True, if deserialization was successful. False otherwise.
     */
    bool readObject(CMetaObject& aObject);

    /*!
     * Write a "properties" block and have all subsequent property reads take place in it.
     *
     * @return True, if successful. False otherwise.
     */
    bool beginProperties();

    /*!
     * Pop the properties block from the stack and return to the object block.
     *
     * @return True, if successful. False otherwise.
     */
    bool commitProperties();

    #define SR_DEFINE_READ_PROPERTY(_aType) \
        /*!
         * Read the current nlohmann::json object into a named CMetaProperty<T> instance.
         *
         * @param [in] aIdentifier The name of the property to be deserialized.
         * @param [in] aProperty   A property ref to fill up with data from the current stack's
         *                         topomost nlohmann::json.
         * @return                 True, if deserialization was successful. False otherwise.
         */                                                         \
        SR_INLINE bool readProperty(                                \
            const std::string&     aIdentifier,                     \
            CMetaProperty<_aType>& aProperty)                       \
        {                                                           \
            return readPropertyImpl<_aType>(aIdentifier, aProperty);\
        }

    #define SR_DEFINE_READ_ATTRIBUTE(_aType) \
        /*!
         * Read a named attribute from the current nlohmann::json object.
         *
         * @param  [in] aIdentifier The name of the attribute to be deserialized.
         * @param  [in] aValue      The value reference to copy the deserialization result to.
         * @return                  True, if deserialization was successful. False otherwise.
         */                                                            \
        SR_INLINE bool readAttribute(                                  \
            const std::string& aIdentifier,                            \
            _aType&            aValue)                                 \
        {                                                              \
            return readAttributeImpl<_aType>(aIdentifier, aValue);     \
        }

    #define SR_DEFINE_READ_VALUELIST(_aType)                        \
        /*!
         * Read a named list of values from the current nlohmann::json object.
         *
         * @param  [in] aIdentifier The name of the list to be deserialized.
         * @param  [in] aValues     The value list reference to copy the deserialization result to.
         * @return                  True, if deserialization was successful. False otherwise.
         */                                                         \
        bool readValueList(                                         \
            const std::string&   aIdentifier,                       \
            std::vector<_aType>& aValues)                           \
        {                                                           \
            return readValueListImpl<_aType>(aIdentifier, aValues); \
        }

    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DEFINE_READ_PROPERTY)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DEFINE_READ_ATTRIBUTE)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DEFINE_READ_VALUELIST)

    /*!
     * Check, whether we are in dynamic deserialization mode.
     *
     * @return True, if dynamic. False, if not.
     */
    SR_INLINE bool isDynamicDeserialization() const { return mUseDynamicDeserialization; }

private_methods:

    /*!
     * Read the current nlohmann::json object into a named CMetaProperty<T> instance.
     *
     * @tparam TDataType        The underlying data type of the property.
     * @param  [in] aIdentifier The name of the property to be deserialized.
     * @param  [in] aProperty   A property ref to fill up with data from the current stack's
     *                          topomost nlohmann::json.
     * @return                  True, if deserialization was successful. False otherwise.
     */
    template <typename TDataType>
    bool readPropertyImpl(const std::string& aIdentifier, CMetaProperty<TDataType>& aProperty);

    /*!
     * Read a named attribute from the current nlohmann::json object.
     *
     * @tparam TDataType        The underlying data type of the property.
     * @param  [in] aIdentifier The name of the attribute to be deserialized.
     * @param  [in] aValue      The value reference to copy the deserialization result to.
     * @return                  True, if deserialization was successful. False otherwise.
     */
    template <typename TDataType>
    bool readAttributeImpl(const std::string& aIdentifier, TDataType& aValue);

    /*!
     * Read a named value list from the current nlohmann::json object.
     *
     * @tparam TDataType        The underlying data type of the property.
     * @param  [in] aIdentifier The name of the list to be deserialized.
     * @param  [in] aValues     The value list reference to copy the deserialization result to.
     * @return                  True, if deserialization was successful. False otherwise.
     */
    template <typename TDataType>
    bool readValueListImpl(const std::string& aIdentifier, std::vector<TDataType>& aValues);

    /*!
     * Reusable implementation of the first deserialization step to reduce redundancy,
     * as we have two entry points.
     *
     * @param [in]  aDeserializedJSON Forwarded, nlohmann::json representation of the object tree.
     * @param [out] aOutResult        Result containing the deserialized CMetaObject, if successful.
     * @return                        True, if the deserialization was successful. False otherwise.
     */
    bool deserializeFwd(
            const nlohmann::json&                                 aDeserializedJSON,
            CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult);

    /*!
     * Reusable implementation of object deserialization containing the prototype fetch and instance
     * creation mechanism. Used to reduce redundancy, since the contained functionality is required
     * in multiple places.
     *
     * @param [out] aOutObject The object pointer, initialized the JSON deserialized object initialized if
     *                         deserialization was successful.
     * @return                 True, if deserialization was successful. False otherwise.
     */
    bool readObjectFwd(CStdSharedPtr_t<CMetaObject>& aOutObject);

    /*!
     * Push a nlohmann::json instance onto the internal stack to have all subsequent operations
     * be performed based on this object.
     *
     * @param [in] aJSONObjectInstance The externally provided nlohmann::json instance as mutable referensce.
     */
    void pushObject(const Object_t& aJSONObjectInstance);
    /*!
     * Pop the top-most nlohmann::json instance from the internal stack to have all subsequent operations
     * be performed based on the parent nlohmann::json instance.
     */
    void popObject();

    /*!
     * Push a new NodeId_t onto the adjacency stack, store it in the current adjacency level and create
     * another adjacency level for the pushed node.
     *
     * @param [in] aNodeId The node to be pushed and focused.
     */
    void pushAdjacencyLevel(const NodeId_t& aNodeId);

    /*!
     * Step back to the parent level by popping the current item from the stack.
     */
    void popAdjacencyLevel();

private_members:
    const bool              mUseDynamicDeserialization;
    std::stack<ObjectRef_t> mObjectStack;

    PropertyRefMap_t        mPropertyReferences;

    std::stack<NodeId_t>    mAdjacencyStack;
    AdjacencyListMap_t      mAdjacencyLists;
    AdjacencyListRef_t      mCurrentAdjacencyList;
};

/*!
 * @file      sr_property_de_serialization_interface.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      26/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <core/serialization/sr_serialization.h>
#include "property_system/sr_meta_def.h"

/*!
 * The IPropertySerializer interface describes the basic requirements for a property system serializer instance
 * accepted by CMetaObject and CMetaPropertyBase and derivates.
 */
class IPropertySerializer
        : public ISerializer<CMetaObject>
{
    SR_DECLARE_INTERFACE(IPropertySerializer)

public_api:
    /*!
     * Make this serializer ready for operation.
     *
     * @return True if successful. False otherwise.
     */
    virtual bool initialize() = 0;
    /*!
     * Stop operation, clear and destroy.
     *
     * @return True if successful. False otherwise.
     */
    virtual bool deinitialize() = 0;

    /*!
     * Accepts a CMetaObject reference and performs depth-first traversing serialization of the CMetaObject
     * tree. The results, if successful, will be stored in a CSerializationResult instance..
     *
     * @param  [in]  aSource    The CMetaObject-tree root object to serialize.
     * @param  [out] aOutResult CSerializationResult handle providing the results, if serialization was successful.
     * @return                  True if successful, false otherwise.
     */
    virtual bool serialize(
            const CMetaObject&                                  aSource,
            CStdSharedPtr_t<ISerializer<CMetaObject>::IResult>& aOutResult) = 0;

    /*!
     * Serialize an object instance into the current nlohmann::json instance pushed on the stack.
     *
     * @param [in]  aObject The object instance to serialize.
     * @return              True, if the entire serialization tree of aObject was serialized successfully.
     *                      False otherwise.
     */
    virtual bool writeObject(const CMetaObject& aObject) = 0;

    /*!
     * Write a "properties" block and have all subsequent property writes take place in it.
     *
     * @return True, if successful. False otherwise.
     */
    virtual bool beginProperties() = 0;

    /*!
     * Pop the properties block from the stack and return to the object block.
     *
     * @return True, if successful. False otherwise.
     */
    virtual bool commitProperties() = 0;

    #define SR_DECLARE_WRITE_PROPERTY(_aType)         \
        /*!
         * Serialize a named property instance into the current nlohmann::json instance pushed on the stack.
         *
         * @param  [in] aIdentifier The name of the property to serialize.
         * @param  [in] aProperty   The CMetaProperty<T> instance to serialize.
         * @return                  True, if the entire serialization tree of aProperty was serialized successfully.
         *                          False otherwise.
         */                                           \
        virtual bool writeProperty(                   \
            const std::string&           aIdentifier, \
            const CMetaProperty<_aType>& aProperty) = 0;

    #define SR_DECLARE_WRITE_ATTRIBUTE(_aType) \
        /*!
         * Serialize a named attribute into the current nlohmann::json instance pushed on the stack.
         *
         * @param  [in] aIdentifier The name of the attribute to serialize.
         * @param  [in] aValue      The attribute value to serialize.
         * @return                  True, if the entire serialization tree of aValue was serialized successfully.
         *                          False otherwise.
         */                                        \
        virtual bool writeAttribute(               \
            const std::string& aIdentifier,        \
            const _aType&      aValue) = 0;        \

    #define SR_DECLARE_WRITE_VALUELIST(_aType)     \
        /*!
         * Serialize a named list of values into a JSON list in the current nlohmann::json instance pushed on the stack.
         *
         * @param  [in] aIdentifier The name of the list to serialize.
         * @param  [in] aValues     The list of values to serialize.
         * @return                  True, if the entire serialization trees of aValues were serialized successfully.
         *                          False otherwise.
         */                                        \
        virtual bool writeValueList(               \
            const std::string&         aIdentifier,\
            const std::vector<_aType>& aValues) = 0;

    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DECLARE_WRITE_PROPERTY)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DECLARE_WRITE_ATTRIBUTE)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DECLARE_WRITE_VALUELIST)
};

/*!
 * The IPropertyDeserializer interface describes the basic requirements for a property system deserializer instance
 * accepted by CMetaObject and CMetaPropertyBase and derivates.
 */
class IPropertyDeserializer
        : public IDeserializer<CMetaObject>
{
    SR_DECLARE_INTERFACE(IPropertyDeserializer)

public_api:/*!
     * Make this serializer ready for operation.
     *
     * @return True if successful. False otherwise.
     */
    virtual bool initialize() = 0;
    /*!
     * Stop operation, clear and destroy.
     *
     * @return True if successful. False otherwise.
     */
    virtual bool deinitialize() = 0;

    /*!
     * Deserialize a string representation of a JSON tree into a CMetaObject.
     *
     * @param  [in]  aSource    Valid JSON string to deserialize.
     * @param  [out] aOutResult A CDeserializationResult containing the deserialized JSON as a CMetaObject.
     * @return                  True, if the entire JSON tree was deserialized properly. False otherwise.
     */
    virtual bool deserialize(
            const std::string&                                    aSource,
            CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult) = 0;

    /*!
     * Deserialize a byte buffer representation of a JSON tree into a CMetaObject.
     *
     * @param [in]  aSource    Valid JSON byte buffer to deserialize.
     * @param [out] aOutResult A CDeserializationResult containing the deserialized JSON as a CMetaObject.
     * @return                 True, if the entire JSON tree was deserialized properly. False otherwise.
     */
    virtual bool deserialize(
            const std::vector<uint8_t>&                           aSource,
            CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult>& aOutResult) = 0;

    /*!
     * Read the current nlohmann::json object into a CMetaObject instance.
     *
     * @param [in] aObject An object ref to fill up with data from the current stack's topmost nlohmann::json.
     * @return             True, if deserialization was successful. False otherwise.
     */
    virtual bool readObject(CMetaObject& aObject) = 0;

    /*!
     * Write a "properties" block and have all subsequent property reads take place in it.
     *
     * @return True, if successful. False otherwise.
     */
    virtual bool beginProperties() = 0;

    /*!
     * Pop the properties block from the stack and return to the object block.
     *
     * @return True, if successful. False otherwise.
     */
    virtual bool commitProperties() = 0;

    #define SR_DECLARE_READ_PROPERTY(_aType)          \
        /*!
         * Read the current nlohmann::json object into a named CMetaProperty<T> instance.
         *
         * @param  [in] aIdentifier The name of the property to be deserialized.
         * @param  [in] aProperty   A property ref to fill up with data from the current stack's
         *                          topomost nlohmann::json.
         * @return                  True, if deserialization was successful. False otherwise.
         */                                           \
        virtual bool readProperty(                    \
            const std::string&     aIdentifier,       \
            CMetaProperty<_aType>& aProperty) = 0;

    #define SR_DECLARE_READ_ATTRIBUTE(_aType) \
        /*!
         * Read a named attribute from the current nlohmann::json object.
         *
         * @param  [in] aIdentifier The name of the attribute to be deserialized.
         * @param  [in] aValue      The value reference to copy the deserialization result to.
         * @return                  True, if deserialization was successful. False otherwise.
         */ \
        virtual bool readAttribute(const std::string& aIdentifier, _aType& aValue) = 0;

    #define SR_DECLARE_READ_VALUELIST(_aType) \
        /*!
         * Read a named list of values from the current nlohmann::json object.
         *
         * @param [in]     aIdentifier The name of the attribute to be deserialized.
         * @param [in,out] aValues     A vector of T to store the list entries into.
         * @return
         */ \
        virtual  bool readValueList(const std::string& aIdentifier, std::vector<_aType>& aValues) = 0;

    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DECLARE_READ_PROPERTY)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DECLARE_READ_ATTRIBUTE)
    SR_APPLY_FOREACH_PROPERTY_TYPE(SR_DECLARE_READ_VALUELIST)
};


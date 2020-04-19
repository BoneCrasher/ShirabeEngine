/*!
 * @file      sr_meta_property.h
 * @author    Marc-Anton Boehm-von Thenen
 * @date      25/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <stdint.h>
#include <string>
#include <variant>
#include <vector>

#include <core/base/defines/sr_defines.h>
#include <core/base/debug/sr_debug.h>
#include <core/base/types/smartptr/sr_cppstd_ptr.h>
#include <core/base/types/sr_map.h>
#include <core/base/patterns/sr_observer.h>

#include <PROPERTY_SYSTEM_Export.h>

#include "property_system/sr_meta_def.h"
#include "property_system/sr_meta_object_callback.h"
#include "property_system/serialization/sr_property_de_serialization_interface.h"

/*!
 * Fwd-declare CMetaObject to avoid circular dependency and permit usage of CMetaObject as property type.
 */
class CMetaObject;

/*!
 * Metaprogramming construct to check, whether a type T is integral or floating point.
 *
 * @tparam TType Type to be evaluated.
 */
template <typename TType>
using is_numeric_type_t =
    typename std::enable_if<std::is_integral_v<TType> || std::is_floating_point_v<TType>>::type;

/*!
 * Metaprogramming construct to check, whether a type T is neither integral nor floating point.
 *
 * @tparam TType Type to be evaluated.
 */
template <typename TType>
using is_other_type_t =
    typename std::enable_if<!(std::is_integral_v<TType> || std::is_floating_point_v<TType>)>::type;

/*!
 * The CMetaPropertyCore class contains shared declarations/definitions for all
 * CMetaProperty types.
 */
class PROPERTY_SYSTEM_LIBRARY_EXPORT CMetaPropertyCore
{
public_typedefs:
    /*!
     * Type-alias to declare the base class for all supported property
     * types' addition and removal callbacks.
     */
    using IMetaPropertyCallback_t = IMetaObjectPropertyCallback<SR_SUPPORTED_PROPERTY_TYPES>;

public_enums:
    /*!
     * The EPropertyCompositeType enum defines identifiers to describe the quantity of values
     * of a property, i.e. single or multiple values of a value type.
     */
    enum class EPropertyStructureType
        : int8_t
    {
        Undefined = 0, //< Unknown
        Atom,          //< Single value:            -[]-
        Chain,         //< Multiple ordered values: -[]=[]=[]=[]-
    };

    /*!
     * The EPropertyValueType enum defines identifiers for each supported
     * property value type.
     */
    enum class EPropertyValueType
            : int8_t
    {
        Undefined = 0, //< Unknown
        Bool,          //< bool:               8-bit unsigned integer, with 0 or 1 as value range
        Int8,          //< stdint.h::int8_t:   8-bit signed integer value
        Int16,         //< stdint.h::int16_t:  16-bit signed integer value
        Int32,         //< stdint.h::int32_t:  32-bit signed integer value
        Int64,         //< stdint.h::int64_t:  64_bit signed integer value
        UInt8,         //< stdint.h::uint8_t:  8-bit signed integer value
        UInt16,        //< stdint.h::uint16_t: 16-bit unsigned integer value
        UInt32,        //< stdint.h::uint32_t: 32-bit unsigned integer value
        UInt64,        //< stdint.h::uint64_t: 64_bit unsigned integer value
        Float,         //< float:              single-precision floating-point value
        Double,        //< double:             double-precision floating-point value
        String,        //< std::string:        char-based string class with \0-terminated char-array
        WString,       //< std::wstring:       wchar_t-based string class with L\0-terminated wchar_t-array
        Object         //< CMetaObject:        Serialized CMetaObject-instance as value
    };

    /*!
     * The EPropertyReplicationMode enum describes flags to control replication behaviour
     * for a property between interconnected peers.
     */
    enum class EPropertyReplicationMode
    {
        DoNotReplicate = 0, //< Value changes, should not be replicated to other connected peers.
        Replicate      = 1, //< Replicate changed values w/ other connected peers.
    };
};

/*!
 * Template base defintion, to map a typename to EPropertyValueType-value.
 *
 * @tparam TDataType Datatype to be mapped.
 */
template <typename TDataType>
struct SMetaPropertyTypeOf
{ };

/*!
 * Helper-Macro to specialize a mapping of typename to enum value.
 */
#define SR_DEFINE_META_PROPERTY_TYPE_OF(_aType, _aEnum)                               \
            /*!
            * Template specialization of SMetaPropertyTypeOf<TData> to map the 'type'
            * to 'enum'.
            */                                                                        \
            template <>                                                               \
            struct SMetaPropertyTypeOf<_aType>                                        \
            {                                                                         \
                static constexpr CMetaPropertyCore::EPropertyValueType value =        \
                        CMetaPropertyCore::EPropertyValueType::_aEnum;                \
            };

SR_DEFINE_META_PROPERTY_TYPE_OF(bool,              Bool)
SR_DEFINE_META_PROPERTY_TYPE_OF(int8_t,            Int8)
SR_DEFINE_META_PROPERTY_TYPE_OF(int16_t,           Int16)
SR_DEFINE_META_PROPERTY_TYPE_OF(int32_t,           Int32)
SR_DEFINE_META_PROPERTY_TYPE_OF(int64_t,           Int64)
SR_DEFINE_META_PROPERTY_TYPE_OF(uint8_t,           UInt8)
SR_DEFINE_META_PROPERTY_TYPE_OF(uint16_t,          UInt16)
SR_DEFINE_META_PROPERTY_TYPE_OF(uint32_t,          UInt32)
SR_DEFINE_META_PROPERTY_TYPE_OF(uint64_t,          UInt64)
SR_DEFINE_META_PROPERTY_TYPE_OF(float,             Float)
SR_DEFINE_META_PROPERTY_TYPE_OF(double,            Double)
SR_DEFINE_META_PROPERTY_TYPE_OF(std::string,       String)
SR_DEFINE_META_PROPERTY_TYPE_OF(std::wstring,      WString)
SR_DEFINE_META_PROPERTY_TYPE_OF(CStdSharedPtr_t<class CMetaObject>, Object)

/*!
 * The CMetaPropertyBase<TData> class wraps a typed value of type TData
 * and provides a read/write API to manipulate and read the stored value.
 * The value can be observed, to receive value change notifications.
 *
 * @tparam TDataType The underlying data type of the property.
 * @remarks          This class can not be instantiated directly, as it is a base class for
 *                   the CMetaProperty<TData> subclasses.
 */
template <typename TDataType>
class PROPERTY_SYSTEM_LIBRARY_EXPORT CMetaPropertyBase
        : public CMetaPropertyCore
        , public ISerializable<IPropertySerializer, IPropertyDeserializer>
{
public_typedefs:
    /*!
     * Type-Alias to describe the stored data.
     */
    using DataType_t  = TDataType;
    /*!
     * Type-Alias to describe this property-specialization.
     */
    using ClassType_t = CMetaPropertyBase<DataType_t>;

    /*!
     * Type-Alias to describe this property's value subjects.
     */
    using Subject_t = CSubject<const DataType_t, const uint32_t>;

public_destructors:
    /*!
     * Reset the internal value and shut down.
     */
    virtual ~CMetaPropertyBase() = default;

public_operators:
    /*!
     * Assign another CMetaPropertyBase<TData> by copy.
     *
     * @param  [in] aOther The other property to copy from.
     * @return             Self-Reference
     */
    ClassType_t& operator=(const ClassType_t& aOther);
    /*!
     * Assign another CMetaPropertyBase<TData> by moving from it.
     *
     * @param  [in] aOther The other property to copy from.
     * @return             Self-Reference
     */
    ClassType_t& operator=(ClassType_t&& aOther);
    /*!
     * Assign another value of type TData.
     *
     * @param  [in] aData The value to be assigned.
     * @return            Self-Reference
     */
    ClassType_t& operator=(const DataType_t& aOther);
    /*!
     * Assign another value of type TData.
     *
     * @param  [in] aData The value to be assigned.
     * @return            Self-Reference
     */
    ClassType_t& operator=(DataType_t  &&aOther);

public_methods:
    /*!
     * Accept a CPropertySerializer instance and conduct property serialization with it.
     *
     * @param   [in] aSerializer The serializer to conduct serialization with.
     * @returns                  True, if successful. False otherwise.
     */
    bool acceptSerializer(IPropertySerializer& aSerializer) const;
    /*!
     * Accept a CPropertyDeserializer instance and conduct property deserialization with it.
     *
     * @param   [in] aSerializer The deserializer to conduct deserialization with.
     * @returns                  True, if successful. False otherwise.
     */
    bool acceptDeserializer(IPropertyDeserializer& aDeserializer);

    /*!
     * Access the property's UID
     *
     * @return The UID of the property.
     */
    const PropertyUID_t& getPropertyUID() const;

    /*!
     * Set the property's UID
     *
     * @param aUID The new UID of the property.
     */
    void setPropertyUID(const PropertyUID_t& aUID);

    /*!
     * Get the property's assigned path.
     *
     * @return
     */
    const std::string& getPropertyPath() const;

    /*!
     * Set the property's assigned path.
     *
     * @param [in] aPropertyPath The path of the property in property-path syntax.
     */
    void setPropertyPath(const std::string& aPropertyPath);

    /*!
     * Access the property's name
     *
     * @return The name of the property.
     */
    const std::string& getPropertyName() const;

    /*!
     * Set the property's name
     *
     * @param [in] aPropertyName The new name of the property.
     */
    void setPropertyName(const std::string& aPropertyName);

    /*!
     * Set the internal value explicitly [at index '0']
     *
     * @param [in] aValue The new value to be assigned.
     */
    void setValue(const DataType_t& aValue);

    /*!
     * Set the internal value explicitly [at index 'aValueIndex']
     *
     * @param [in] aValue      The new value to be assigned.
     * @param [in] aValueIndex If this property is of structure type "Chain", provide the value at the given index.
     */
    void setValue(const DataType_t& aValue, const uint32_t& aValueIndex);

    /*!
     * Set the internal value explicitly [at index '0']
     *
     * @param [in] aValue The new value to be assigned.
     */
    void setValue(DataType_t&& aValue);

    /*!
     * Set the internal value explicitly [at index 'aValueIndex']
     *
     * @param [in] aValue      The new value to be assigned.
     * @param [in] aValueIndex If this property is of structure type "Chain", provide the value at the given index.
     */
    void setValue(DataType_t&& aValue, const uint32_t& aValueIndex);

    /*!
     * Get the internal value immutably [at index 0]
     *
     * @return The internal value by ref.
     */
    const DataType_t& getValue() const;
    /*!
     * Get the internal value immutably [at index 'aValueIndex']
     *
     * @param  [in] aValueIndex If this property is of structure type "Chain", provide the value at the given index.
     * @return                  The internal value by const ref.
     */
    const DataType_t& getValue(const uint32_t& aValueIndex) const;

    /*!
     * Declare the maximum capacity of values to be stored within the property.
     * Automatically derives the structure type of the property.
     *
     * @param  [in] aCapacity The maximum capacity of values in the chain-property.
     * @return                Self-Reference for Call-Chaining.
     */
    CMetaPropertyBase<TDataType>& setCapacity(const uint32_t& aCapacity);

    /*!
     * Return the current capacity of the property container.
     *
     * @return Capacity of the container of range [1..n].
     */
    const uint32_t& getCapacity() const;

    /*!
     * Set the default value of the Atomic-Property or foreach element of a Chain-Property.
     *
     * @param  [in] aDefaultValue Default value of type TData.
     * @return                    Self-Reference for Call-Chaining.
     */
    CMetaPropertyBase<TDataType>& setDefaultValue(const TDataType& aDefaultValue);

    /*!
     * Return the current default value of the property, to be assigned for each newly created entry.
     *
     * @return The set default value.
     */
    const TDataType& getDefaultValue() const;

    /*!
     * Configure the property to allow write-operations.
     *
     * @param  [in] aIsWritable Declare writability: True, if writeable. False if not.
     * @return                  Self-Reference for Call-Chaining.
     */
    CMetaPropertyBase<TDataType>& setWriteable(bool aIsWritable);

    /*!
     * Return whether the property can be written or not.
     *
     * @return True, if writable. False otherwise.
     */
    bool isWritable() const;

    /*!
     * Configure the property for peer-synchronization (i.e. replication).
     *
     * @param  [in] aReplicationMode Flag indicating the replication behaviour.
     * @return                       Self-Reference for Call-Chaining.
     */
    CMetaPropertyBase<TDataType> &setReplicationMode(const EPropertyReplicationMode& aReplicationMode);

    /*!
     * Return whether the property is being replicated.
     *
     * @return EPropertyReplicationMode flag indicating replication behaviour.
     */
    EPropertyReplicationMode getReplicationMode() const;

    /*!
     * Register and observer to observe the value at 'aValueIndex'.
     *
     * @param [in] aObserver   Observer-instance to be registered.
     * @param [in] aValueIndex Index of the value to be observed.
     * @return                 True, if the observer was successfully registered.
     *                         False, if the value index is out of range or the observer
     *                         is invalid.
     */
    bool observe(
            typename Subject_t::ObserverPtr_t aObserver,
            const uint32_t&                   aValueIndex);

    /*!
     * Ignore the value at index 'aValueIndex'.
     *
     * @param [in] aObserver   Observer-instance to be registered.
     * @param [in] aValueIndex Index of the value to be observed.
     * @return                 True, if the observer was successfully removed.
     *                         False, if the value index is out of range or the observer
     *                         is invalid.
     */
    bool ignore(
            typename Subject_t::ObserverPtr_t aObserver,
            const uint32_t&                   aValueIndex);

protected_constructors:
    /*!
     * Construct a default property with invalid id.
     */
    CMetaPropertyBase();

    /*!
     * Constructs a new CMetaPropertyBase<TData> by copying from another instance of
     * equal underlying type TData.
     *
     * @param [in] aOther The other property instance.
     */
    CMetaPropertyBase(const ClassType_t& aOther);
    /*!
     * Constructs a new CMetaPropertyBase<TData> by moving from another instance of
     * equal underlying type TData.
     *
     * @param [in] aOther The other property instance.
     */
    CMetaPropertyBase(ClassType_t&& aOther);
    /*!
     * Constructs a new CMetaPropertyBase<TData> by initializing with the value 'aData' of type TData.
     *
     * @param [in] aUID          The uid of the property.
     * @param [in] aPropertyName The name of the property.
     * @param [in] aData         The initial data value.
     */
    CMetaPropertyBase(const PropertyUID_t& aUID, const std::string& aPropertyName, const DataType_t& aData);
    /*!
     * Constructs a new CMetaPropertyBase<TData> by initializing with the value 'aData' of type TData.
     *
     * @param [in] aUID          The uid of the property.
     * @param [in] aPropertyName The name of the property.
     * @param [in] aData         The initial data value.
     */
    CMetaPropertyBase(const PropertyUID_t& aUID, const std::string& aPropertyName, DataType_t&& aData);

protected_methods:
    /*!
     * Check the value index against the current capacity.
     *
     * @param [in] aIndex Index to be checked.
     * @return            True, if index is [0..capacity-1]. False, otherwise.
     */
    EPropertySystemError checkIndex(const uint32_t& aIndex);

private_static_constants:
    static const constexpr uint32_t SR_FIRST_VALUE_INDEX = 0;

private_methods:
    /*!
     * Assign a single aValue at aValueIndex. Will reallocate the internal buffer, if too small.
     *
     * @param [in] aValue      The data to be assigned.
     * @param [in] aValueIndex The index in the value array to assign data to.
     */
    void assign(const DataType_t& aValue, const uint32_t& aValueIndex);
    /*!
     * Assign a single aValue at aValueIndex. Will reallocate the internal buffer, if too small.
     *
     * @param [in] aValue      The data to be assigned.
     * @param [in] aValueIndex The index in the value array to assign data to.
     */
    void assign(DataType_t&& aValue, const uint32_t& aValueIndex);

    /*!
     * Assign an entire data vector. Will swap contents, if there's a current set of values.
     *
     * @param [in] aOther The list of values to assign.
     */
    void assign(const std::vector<DataType_t>& aOther);

    /*!
     * Move-Assign an entire data vector. Will swap contents, if there's a current set of values.
     *
     * @param [in] aOther The list of values to assign.
     */
    void assign(std::vector<DataType_t>&& aOther);

protected_members:
    PropertyUID_t                               mPropertyUID;
    std::string                                 mPropertyName;
    std::string                                 mPropertyPath;

    CMetaPropertyCore::EPropertyStructureType   mStructureType;
    uint32_t                                    mCapacity;

    CMetaPropertyCore::EPropertyValueType       mValueType;
    DataType_t                                  mDefaultValue;
    std::vector<DataType_t>                     mValues;

    std::vector<Subject_t>                      mSubjects;

    bool                                        mWritable;
    EPropertyReplicationMode                    mReplicationMode;
};

/*!
 * The CMetaProperty class variants extend the basic CMetaPropertyBase<TData>-template by type
 * dependent functionality.
 * Fallback for any failed SFINAE/Metaprogramming check.
 *
 * @tparam TDataType   The underlying datatype of the property.
 * @tparam TEnableFlag SFINAE flag to control specialization resolution.
 */
template <typename TDataType, typename TEnableFlag>
class CMetaProperty
{
    static constexpr const int32_t sSFINAEVerifyFlag = 0;
};

/*!
 * The CMetaProperty class variants extend the basic CMetaPropertyBase<TData>-template by type
 * dependent functionality.
 * This version with TEnable = void is a basic wrapper around CMetaPropertyBase<TData>
 * and serves as a fallback for all SFINAE-specializations whose substitution failed.
 * The specialization will not extend functionality of the base class.
 *
 * @tparam TDataType The underlying datatype of the property.
 */
template <typename TDataType>
class CMetaProperty<TDataType, is_other_type_t<TDataType>>
        : public CMetaPropertyBase<TDataType>
{
public_static_constants:
    static constexpr const int32_t sSFINAEVerifyFlag = 1;

public_typedefs:
    using DataType_t  = TDataType;
    using ClassType_t = CMetaProperty<TDataType, void>;

public_constructors:
    /*!
     * Constructs a new CMetaProperty<TData>, default initializing it's value to TData(0).
     */
    CMetaProperty();
    /*!
     * Constructs a new CMetaProperty<TData> by copying from another instance of
     * equal underlying type TData.
     *
     * @param [in] aOther The other property instance.
     */
    CMetaProperty(const ClassType_t& aOther);
    /*!
     * Constructs a new CMetaProperty<TData> by moving from another instance of
     * equal underlying type TData.
     *
     * @param [in] aOther The other property instance.
     */
    CMetaProperty(ClassType_t&& aOther);
    /*!
     * Constructs a new CMetaProperty<TData> by initializing with the value 'aData' of type TData.
     *
     * @param [in] aUID          The uid of the property.
     * @param [in] aPropertyName The name of the property.
     * @param [in] aData         The initial data value.
     */
    CMetaProperty(const PropertyUID_t& aUID, const std::string& aPropertyName, const DataType_t& aData);
    /*!
     * Constructs a new CMetaProperty<TData> by initializing with the value 'aData' of type TData.
     *
     * @param [in] aUID          The uid of the property.
     * @param [in] aPropertyName The name of the property.
     * @param [in] aData         The initial data value.
     */
    CMetaProperty(const PropertyUID_t& aUID, const std::string& aPropertyName, DataType_t&& aData);

public_destructors:
    /*!
     * Reset the internal value and shut down.
     */
    ~CMetaProperty() = default;

public_operators:
    /*!
     * Assign another CMetaProperty<TData> by copy.
     *
     * @param  [in] aOther The other property to copy from.
     * @return        Self-Reference
     */
    ClassType_t& operator=(const ClassType_t& aOther);
    /*!
     * Assign another CMetaProperty<TData> by moving from it.
     *
     * @param  [in] aOther The other property to copy from.
     * @return             Self-Reference
     */
    ClassType_t& operator=(ClassType_t&& aOther);

public_methods:
    /*!
     * Declare the maximum capacity of values to be stored within the property.
     * Automatically derives the structure type of the property.
     *
     * @param  [in] aCapacity The maximum capacity of values in the chain-property.
     * @return                Self-Reference for Call-Chaining.
     */
    SR_INLINE ClassType_t& setCapacity(const uint32_t& aCapacity)
    {
        CMetaPropertyBase<TDataType>::setCapacity(aCapacity);
        return (*this);
    }

    /*!
     * Set the default value of the Atomic-Property or foreach element of a Chain-Property.
     *
     * @param  [in] aDefaultValue Default value of type TData.
     * @return                    Self-Reference for Call-Chaining.
     */
    SR_INLINE ClassType_t& setDefaultValue(const TDataType& aDefaultValue)
    {
        CMetaPropertyBase<TDataType>::setDefaultValue(aDefaultValue);
        return (*this);
    }

    /*!
     * Configure the property to allow write-operations.
     *
     * @param [in] aIsWritable Declare writability: True, if writeable. False if not.
     * @return                 Self-Reference for Call-Chaining.
     */
    SR_INLINE ClassType_t& setWriteable(bool aIsWritable)
    {
        CMetaPropertyBase<TDataType>::setWriteable(aIsWritable);
        return (*this);
    }

};

/*!
 * The CMetaProperty class variants extend the basic CMetaPropertyBase<TData>-template by type
 * dependent functionality.
 * This version with TEnable = is_numeric_type_t<TData>, extends the basic property
 * CMetaPropertyBase<TData> by integral type or floating point type only functionality, like ranges.
 *
 * @tparam TDataType The underlying datatype of the property.
 */
template <typename TDataType>
class CMetaProperty<TDataType, is_numeric_type_t<TDataType>>
        : public CMetaPropertyBase<TDataType>
{
public_static_constants:
    static constexpr const int32_t sSFINAEVerifyFlag = 2;

public_typedefs:
    using DataType_t  = TDataType;
    using ClassType_t = CMetaProperty<TDataType, is_numeric_type_t<TDataType>>;

public_constructors:
    /*!
     * Constructs a new CMetaProperty<TData>, default initializing it's value to TData(0).
     */
    CMetaProperty();
    /*!
     * Constructs a new CMetaProperty<TData> by copying from another instance of
     *                equal underlying type TData.
     * @param [in] aOther The other property instance.
     */
    CMetaProperty(const ClassType_t& aOther);
    /*!
     * Constructs a new CMetaProperty<TData> by moving from another instance of
     *                equal underlying type TData.
     * @param [in] aOther The other property instance.
     */
    CMetaProperty(ClassType_t&& aOther);
    /*!
     * Constructs a new CMetaProperty<TData> by initializing with the value 'aData' of type TData.
     *
     * @param [in] aUID          The uid of the property.
     * @param [in] aPropertyName The name of the property.
     * @param [in] aData         The initial data value.
     */
    CMetaProperty(const PropertyUID_t& aUID, const std::string& aPropertyName, const DataType_t&);
    /*!
     * Constructs a new CMetaProperty<TData> by initializing with the value 'aData' of type TData.
     *
     * @param [in] aUID          The uid of the property.
     * @param [in] aPropertyName The name of the property.
     * @param [in] aData         The initial data value.
     */
    CMetaProperty(const PropertyUID_t& aUID, const std::string& aPropertyName, DataType_t&&);

public_destructors:
    /*!
     * Reset the internal value and shut down.
     */
    ~CMetaProperty() = default;

public_operators:
    /*!
     * Assign another CMetaProperty<TData> by copy.
     *
     * @param  [in] aOther The other property to copy from.
     * @return             Self-Reference
     */
    ClassType_t& operator=(const ClassType_t& aOther);
    /*!
     * Assign another CMetaProperty<TData> by moving from it.
     *
     * @param  [in] aOther The other property to copy from.
     * @return             Self-Reference
     */
    ClassType_t& operator=(ClassType_t&& aOther);

public_methods:

    /*!
     * Set the internal value explicitly [at index '0']
     *
     * @param [in] aValue The new value to be assigned.
     */
    SR_INLINE void setValue(const DataType_t& aValue)
    {
        ClassType_t::setValue(aValue, 0);
    }

    /*!
     * Set the internal value explicitly [at index 'aValueIndex']
     *
     * @param [in] aValue      The new value to be assigned.
     * @param [in] aValueIndex If this property is of structure type "Chain", provide the value at the given index.
     */
    SR_INLINE void setValue(const DataType_t& aValue, const uint32_t& aValueIndex)
    {
        const bool inRange = checkRange(aValue);
        SR_RETURN_VOID_IF(!inRange);

        CMetaPropertyBase<TDataType>::setValue(aValue, aValueIndex);
    }

    /*!
     * Declare the maximum capacity of values to be stored within the property.
     * Automatically derives the structure type of the property.
     *
     * @param [in]  aCapacity The maximum capacity of values in the chain-property.
     * @return                Self-Reference for Call-Chaining.
     */
    SR_INLINE ClassType_t& setCapacity(const uint32_t& aCapacity)
    {
        CMetaPropertyBase<TDataType>::setCapacity(aCapacity);
        return (*this);
    }

    /*!
     * Set the default value of the Atomic-Property or foreach element of a Chain-Property.
     *
     * @param  [in] aDefaultValue Default value of type TData.
     * @return                    Self-Reference for Call-Chaining.
     */
    SR_INLINE ClassType_t& setDefaultValue(const TDataType& aDefaultValue)
    {
        CMetaPropertyBase<TDataType>::setDefaultValue(aDefaultValue);
        return (*this);
    }

    /*!
     * Configure the property to allow write-operations.
     *
     * @param [in] aIsWritable Declare writability: True, if writeable. False if not.
     * @return                 Self-Reference for Call-Chaining.
     */
    SR_INLINE ClassType_t& setWriteable(bool aIsWritable)
    {
        CMetaPropertyBase<TDataType>::setWriteable(aIsWritable);
        return (*this);
    }

    /*!
     * Define the minimum and maximum value range for each of the values in the property.
     *
     * @param [in] aMin Minimum value of type TData.
     * @param [in] aMax Maximum value of type TData.
     * @return          Self-Reference for Call-Chaining.
     * @remarks         If not called, min and max are defined to the numeric-limits of the internal data type, if numeric.
     */
    ClassType_t& setRange(const DataType_t& aMin, const DataType_t& aMax);

    /*!
     * Return the current minimum value permitted for this property.
     *
     * @return See brief.
     */
    const DataType_t& getMinValue() const;

    /*!
     * Return the current maximum value permitted for this property.
     *
     * @return See brief.
     */
    const DataType_t& getMaxValue() const;

private_methods:
    /*!
     * Check a value against the current min/max value range.
     *
     * @param  [in] aData Value to be checked.
     * @remarks           Debugbreaks, if the value is out of range.
     */
    bool checkRange(const DataType_t& aData);

private_members:
    DataType_t mMinValue;
    DataType_t mMaxValue;
};

/*!
 * @brief Type alias to enable storage of all supported types within a common type.
 */
using MetaPropertyVariant_t =
    std::variant<
        //CMetaProperty<bool>,
        CMetaProperty<int8_t>,
        CMetaProperty<int16_t>,
        CMetaProperty<int32_t>,
        CMetaProperty<int64_t>,
        CMetaProperty<uint8_t>,
        CMetaProperty<uint16_t>,
        CMetaProperty<uint32_t>,
        CMetaProperty<uint64_t>,
        CMetaProperty<float>,
        CMetaProperty<double>,
        CMetaProperty<std::string>,
        CMetaProperty<std::wstring>,
        CMetaProperty<CStdSharedPtr_t<CMetaObject>>
    >;

/*!
 * @brief Convenience typedef to declare maps of MetaPropertyVariant_t.
 */
using MetaPropertyMap_t = CMap<std::string, MetaPropertyVariant_t>;

/*!
 * Creates a resolver usable within std::visit to set the propertyUID on the correct
 * property instance of a variant.
 *
 * @tparam TType The underlying data type of the property.
 */
template <typename TType>
struct SSetPropertyUIDResolver
{
    static auto getResolver(const PropertyUID_t& aUID)
    {
        return [&] (CMetaProperty<TType>& aResolvedInstance) -> void
        {
            aResolvedInstance.setPropertyUID(aUID);
        };
    }
};

/*!
 * Creates a resolver usable within std::visit to read the property name from the correct
 * property instance of a variant.
 *
 * @tparam TType The underlying data type of the property.
 */
template <typename TType>
struct SGetPropertyNameResolver
{
    static auto getResolver(std::string& aOutPropertyName)
    {
        return [&] (CMetaProperty<TType>& aResolvedInstance) -> void
        {
            aOutPropertyName = aResolvedInstance.getPropertyName();
        };
    }
};

/*!
 * Creates a resolver usable within std::visit to set the property path to the correct
 * property instance of a variant.
 *
 * @tparam TType The underlying data type of the property.
 */
template <typename TType>
struct SSetPropertyPathResolver
{
    static auto getResolver(std::string& aOutPropertyPath)
    {
        return [&] (CMetaProperty<TType>& aResolvedInstance) -> void
        {
            aResolvedInstance.setPropertyPath(aOutPropertyPath);
        };
    }
};

//-----------------------------------------------------------------------------------------------------
// STATIC VERIFICATION OF SFINAE CONSTRUCTS! :P
//-----------------------------------------------------------------------------------------------------
// As long as the below code compiles, the SFINAE partial template specialization and selection
// works fine!
struct     SSFINAEVerifyStruct {};
enum class ESFINAEVerifyEnum   {};

static void verifySFINAEPartialSpecialization()
{
    static_assert(CMetaProperty<SSFINAEVerifyStruct>         ::sSFINAEVerifyFlag == 1); // Struct types
    static_assert(CMetaProperty<ESFINAEVerifyEnum>           ::sSFINAEVerifyFlag == 1); // Enum class types
    static_assert(CMetaProperty<std::string>                 ::sSFINAEVerifyFlag == 1); // String
    static_assert(CMetaProperty<std::wstring>                ::sSFINAEVerifyFlag == 1); // WString
    static_assert(CMetaProperty<bool>                        ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<int8_t>                      ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<int16_t>                     ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<int32_t>                     ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<int64_t>                     ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<uint8_t>                     ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<uint16_t>                    ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<uint32_t>                    ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<uint64_t>                    ::sSFINAEVerifyFlag == 2); // Integral
    static_assert(CMetaProperty<float>                       ::sSFINAEVerifyFlag == 2); // Floating Point
    static_assert(CMetaProperty<double>                      ::sSFINAEVerifyFlag == 2); // Floating Point
    static_assert(CMetaProperty<CStdSharedPtr_t<CMetaObject>>::sSFINAEVerifyFlag == 1); // Object-Types
}
//-----------------------------------------------------------------------------------------------------

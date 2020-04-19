/*!
 * @file      sr_meta_object.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      11/07/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include <utility>
#include <algorithm>
#include <functional>

#include <core/base/defines/sr_defines.h>
#include <core/base/debug/sr_debug.h>
#include <core/base/types/sr_enum.h>

#include "property_system/sr_meta_object.h"
#include "property_system/sr_meta_property.h"

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>::CMetaPropertyBase()
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mPropertyUID(PropertyUID_t())
    , mPropertyName("")
    , mStructureType(EPropertyStructureType::Undefined)
    , mCapacity(0)
    , mValueType(EPropertyValueType::Undefined)
    , mDefaultValue(TDataType())
    , mValues(0)
    , mSubjects(0)
    , mWritable(true)
    , mReplicationMode(EPropertyReplicationMode::DoNotReplicate)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>::CMetaPropertyBase(const ClassType_t& aOther)
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mPropertyUID(aOther.mPropertyUID)
    , mPropertyName(aOther.mPropertyName)
    , mStructureType(aOther.mStructureType)
    , mCapacity(aOther.mCapacity)
    , mValueType(aOther.mValueType)
    , mDefaultValue(aOther.mDefaultValue)
    , mValues(aOther.mCapacity)
    , mSubjects(aOther.mSubjects)
    , mWritable(aOther.mWritable)
    , mReplicationMode(aOther.mReplicationMode)
{
    assign(aOther.mValues);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>::CMetaPropertyBase(ClassType_t&& aOther)
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mPropertyUID(aOther.mPropertyUID)
    , mPropertyName(aOther.mPropertyName)
    , mStructureType(aOther.mStructureType)
    , mCapacity(aOther.mCapacity)
    , mValueType(aOther.mValueType)
    , mDefaultValue(aOther.mDefaultValue)
    , mValues(std::move(aOther.mValues))
    , mSubjects(std::move(aOther.mSubjects))
    , mWritable(aOther.mWritable)
    , mReplicationMode(aOther.mReplicationMode)
{
    // assign(std::move(aOther.mValues));
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>::CMetaPropertyBase(
        const PropertyUID_t& aUID,
        const std::string&   aPropertyName,
        const DataType_t&    aData)
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mPropertyUID(aUID)
    , mPropertyName(aPropertyName)
    , mStructureType(EPropertyStructureType::Atom)
    , mCapacity(1)
    , mValueType(SMetaPropertyTypeOf<TDataType>::value)
    , mDefaultValue(TDataType())
    , mValues({ aData })
    , mSubjects(1)
    , mWritable(true)
    , mReplicationMode(EPropertyReplicationMode::DoNotReplicate)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>::CMetaPropertyBase(
        const PropertyUID_t& aUID,
        const std::string&   aPropertyName,
        DataType_t&&         aData)
    : ISerializable<IPropertySerializer, IPropertyDeserializer>()
    , mPropertyUID(aUID)
    , mPropertyName(aPropertyName)
    , mStructureType(EPropertyStructureType::Atom)
    , mCapacity(1)
    , mValueType(SMetaPropertyTypeOf<TDataType>::value)
    , mDefaultValue(TDataType())
    , mValues({ aData })
    , mSubjects(1)
    , mWritable(true)
    , mReplicationMode(EPropertyReplicationMode::DoNotReplicate)
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaPropertyBase<TDataType>::ClassType_t&
CMetaPropertyBase<TDataType>::operator=(const ClassType_t& aOther)
{
    mPropertyUID     = aOther.mPropertyUID;
    mPropertyName    = aOther.mPropertyName;
    mStructureType   = aOther.mStructureType;
    mCapacity        = aOther.mCapacity;
    mValueType       = aOther.mValueType;
    mDefaultValue    = aOther.mDefaultValue;
    mSubjects        = aOther.mSubjects;
    mWritable        = aOther.mWritable;
    mReplicationMode = aOther.mReplicationMode;
    assign(aOther.mValues);

    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaPropertyBase<TDataType>::ClassType_t&
CMetaPropertyBase<TDataType>::operator=(ClassType_t&& aOther)
{
    mPropertyUID     = aOther.mPropertyUID;
    mPropertyName    = aOther.mPropertyName;
    mStructureType   = aOther.mStructureType;
    mCapacity        = aOther.mCapacity;
    mValueType       = aOther.mValueType;
    mDefaultValue    = aOther.mDefaultValue;
    mSubjects        = aOther.mSubjects;
    mWritable        = aOther.mWritable;
    mReplicationMode = aOther.mReplicationMode;
    assign(aOther.mValues);

    aOther.mSubjects.clear();
    aOther.mValues.clear();
    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaPropertyBase<TDataType>::ClassType_t &
CMetaPropertyBase<TDataType>::operator=(const DataType_t& aData)
{
    setValue(aData);
    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaPropertyBase<TDataType>::ClassType_t&
CMetaPropertyBase<TDataType>::operator=(DataType_t&& aData)
{
    setValue(std::move(aData));
    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaPropertyBase<TDataType>::acceptSerializer(IPropertySerializer& aSerializer) const
{
    using StructureTypeUnderlying_t   = CEnum::UnderlyingType_t<CMetaPropertyCore::EPropertyStructureType>;
    using ValueTypeUnderlying_t       = CEnum::UnderlyingType_t<CMetaPropertyCore::EPropertyStructureType>;
    using ReplicationModeUnderlying_t = CEnum::UnderlyingType_t<CMetaPropertyCore::EPropertyReplicationMode>;

    const StructureTypeUnderlying_t   structureType   = static_cast<StructureTypeUnderlying_t>  (mStructureType);
    const ValueTypeUnderlying_t       valueType       = static_cast<ValueTypeUnderlying_t>      (mValueType);
    const ReplicationModeUnderlying_t replicationMode = static_cast<ReplicationModeUnderlying_t>(mReplicationMode);

    const int8_t writable = static_cast<int8_t>(mWritable);

    bool serialized = true;

    serialized = serialized && aSerializer.writeAttribute("uid",             mPropertyUID);
    serialized = serialized && aSerializer.writeAttribute("name",            mPropertyName);
    serialized = serialized && aSerializer.writeAttribute("structuretype",   structureType);
    serialized = serialized && aSerializer.writeAttribute("capacity",        mCapacity);
    serialized = serialized && aSerializer.writeAttribute("valuetype",       valueType);
    serialized = serialized && aSerializer.writeAttribute("default",         mDefaultValue);
    serialized = serialized && aSerializer.writeValueList("values",          mValues);
    serialized = serialized && aSerializer.writeAttribute("writable",        writable);
    serialized = serialized && aSerializer.writeAttribute("replicationmode", replicationMode);

    return serialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------

template <typename TDataType>
bool CMetaPropertyBase<TDataType>::acceptDeserializer(IPropertyDeserializer &aDeserializer)
{
    CEnum::UnderlyingType_t<CMetaPropertyCore::EPropertyStructureType>   structureType   {};
    CEnum::UnderlyingType_t<CMetaPropertyCore::EPropertyValueType>       valueType       {};
    CEnum::UnderlyingType_t<CMetaPropertyCore::EPropertyReplicationMode> replicationMode {};

    int8_t writeable = 0;

    bool deserialized = true;

    deserialized = deserialized && aDeserializer.readAttribute("uid",             mPropertyUID);
    deserialized = deserialized && aDeserializer.readAttribute("name",            mPropertyName);
    deserialized = deserialized && aDeserializer.readAttribute("structuretype",   structureType);
    deserialized = deserialized && aDeserializer.readAttribute("capacity",        mCapacity);
    deserialized = deserialized && aDeserializer.readAttribute("valuetype",       valueType);
    deserialized = deserialized && aDeserializer.readAttribute("default",         mDefaultValue);
    deserialized = deserialized && aDeserializer.readValueList("values",          mValues);
    deserialized = deserialized && aDeserializer.readAttribute("writable",        writeable);
    deserialized = deserialized && aDeserializer.readAttribute("replicationmode", replicationMode);

    mStructureType   = static_cast<CMetaPropertyCore::EPropertyStructureType>(structureType);
    mValueType       = static_cast<CMetaPropertyCore::EPropertyValueType>(valueType);
    mReplicationMode = static_cast<CMetaPropertyCore::EPropertyReplicationMode>(replicationMode);
    mWritable        = static_cast<bool>(writeable);

    return deserialized;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
PropertyUID_t const &CMetaPropertyBase<TDataType>::getPropertyUID() const
{
    return mPropertyUID;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setPropertyUID(const PropertyUID_t& aUID)
{
    mPropertyUID = aUID;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setPropertyPath(const std::string& aPropertyPath)
{
    mPropertyPath = aPropertyPath;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const std::string& CMetaPropertyBase<TDataType>::getPropertyPath() const
{
    return mPropertyPath;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const std::string& CMetaPropertyBase<TDataType>::getPropertyName() const
{
    return mPropertyName;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setPropertyName(const std::string& aPropertyName)
{
    mPropertyName = aPropertyName;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setValue(const DataType_t& aValue)
{
    setValue(aValue, SR_FIRST_VALUE_INDEX);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setValue(const DataType_t& aValue, const uint32_t& aValueIndex)
{
    SR_RETURN_VOID_IF(false == mWritable); // Prevent writes, if not writable.

    const EPropertySystemError status        = checkIndex(aValueIndex);
    const bool                 indexInBounds = (status == EPropertySystemError::Ok);
    SR_ASSERT(indexInBounds);

    assign(aValue, aValueIndex);

    const Subject_t& subject = mSubjects.at(aValueIndex);
    subject.notify(std::move(aValue), std::move(aValueIndex));
}

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setValue(DataType_t &&aValue)
{
    setValue(std::move(aValue), SR_FIRST_VALUE_INDEX);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::setValue(DataType_t&& aValue, const uint32_t& aValueIndex)
{
    SR_RETURN_VOID_IF(false == mWritable); // Prevent writes, if not writable.

    const EPropertySystemError status        = checkIndex(aValueIndex);
    const bool                 indexInBounds = (status == EPropertySystemError::Ok);
    SR_ASSERT(indexInBounds);

    assign(std::move(aValue), aValueIndex);

    const Subject_t& subject = mSubjects.at(aValueIndex);
    subject.notify(std::move(aValue), std::move(aValueIndex));
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const typename CMetaPropertyBase<TDataType>::DataType_t&
CMetaPropertyBase<TDataType>::getValue() const
{
    return getValue(SR_FIRST_VALUE_INDEX);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const typename CMetaPropertyBase<TDataType>::DataType_t&
CMetaPropertyBase<TDataType>::getValue(const uint32_t& aValueIndex) const
{
    const uint64_t size = mValues.size();
    SR_ASSERT(aValueIndex < size);

    return mValues.at(aValueIndex);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>& CMetaPropertyBase<TDataType>::setCapacity(const uint32_t& aCapacity)
{
    SR_ASSERT((0 < aCapacity));

    const uint32_t oldCapacity = mCapacity;

    mCapacity      = aCapacity;
    mStructureType =
            (mCapacity == 1)
                ? CMetaPropertyCore::EPropertyStructureType::Atom
                : CMetaPropertyCore::EPropertyStructureType::Chain;
    mValues.resize(mCapacity);
    mSubjects.resize(mCapacity); // Will implicitly destroy the observer pointers.

    // Make sure to fill up the possibly free space in the end with valid data.
    if(mCapacity > oldCapacity)
    {
        std::fill(mValues.begin() + oldCapacity, mValues.end(), mDefaultValue);
    }

    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const uint32_t& CMetaPropertyBase<TDataType>::getCapacity() const
{
    return mCapacity;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>& CMetaPropertyBase<TDataType>::setDefaultValue(const TDataType& aDefaultValue)
{
    mDefaultValue = aDefaultValue;

    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const TDataType& CMetaPropertyBase<TDataType>::getDefaultValue() const
{
    return mDefaultValue;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>& CMetaPropertyBase<TDataType>::setWriteable(bool aIsWritable)
{
    mWritable = aIsWritable;

    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyBase<TDataType>& CMetaPropertyBase<TDataType>::setReplicationMode(
        const CMetaPropertyCore::EPropertyReplicationMode& aReplicationMode)
{
    mReplicationMode = aReplicationMode;

    // TBDone: Unregister from CMetaSystem.

    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaPropertyCore::EPropertyReplicationMode CMetaPropertyBase<TDataType>::getReplicationMode() const
{
    return mReplicationMode;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaPropertyBase<TDataType>::isWritable() const
{
    return mWritable;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaPropertyBase<TDataType>::observe(
        typename Subject_t::ObserverPtr_t aObserver,
        const uint32_t&                   aValueIndex)
{
    checkIndex(aValueIndex);

    Subject_t& subject = mSubjects.at(aValueIndex);

    const bool observing = subject.observe(aObserver);
    return observing;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaPropertyBase<TDataType>::ignore(
        typename Subject_t::ObserverPtr_t aObserver,
        const uint32_t&                   aValueIndex)
{
    checkIndex(aValueIndex);

    Subject_t &subject = mSubjects.at(aValueIndex);

    const bool ignoring = subject.ignore(aObserver);
    return ignoring;
}

//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::assign(const DataType_t& aValue, const uint32_t& aValueIndex)
{
    if constexpr (std::is_same_v<TDataType, CStdSharedPtr_t<CMetaObject>>)
    {
        // Deep copy is required.
        CStdSharedPtr_t<CMetaObject> source = aValue;
        CStdSharedPtr_t<CMetaObject> target = nullptr;

        if(nullptr != source)
        {
            target = source->clone();
        }

        mValues[aValueIndex] = target;
    }
    else
    {
        mValues[aValueIndex] = aValue;
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::assign(DataType_t&& aValue, const uint32_t& aValueIndex)
{
    mValues[aValueIndex] = std::move(aValue);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::assign(const std::vector<DataType_t>& aOther)
{
    if constexpr (std::is_same_v<TDataType, CStdSharedPtr_t<CMetaObject>>)
    {
        // Deep copy is required.
        const size_t count = std::min(aOther.size(), static_cast<size_t>(mCapacity));
        for(size_t k=0; k<count; ++k)
        {
            CStdSharedPtr_t<CMetaObject> source = aOther.at(k);
            CStdSharedPtr_t<CMetaObject> target = nullptr;

            if(nullptr != source)
            {
                target = source->clone();
            }

            mValues[k] = target;
        }
    }
    else
    {
        mValues = aOther;
    }
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
void CMetaPropertyBase<TDataType>::assign(std::vector<DataType_t>&& aOther)
{
    mValues.swap(aOther);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
EPropertySystemError CMetaPropertyBase<TDataType>::checkIndex(const uint32_t& aIndex)
{
    const bool outOfBounds = (aIndex >= mCapacity);
    SR_RETURN_IF(true == outOfBounds,  EPropertySystemError::PropertyValueIndexOutOfBounds);

    return EPropertySystemError::Ok;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaProperty<TDataType, is_other_type_t<TDataType>>::ClassType_t&
CMetaProperty<TDataType, is_other_type_t<TDataType>>::operator=(const ClassType_t& aOther)
{
    CMetaPropertyBase<TDataType>::operator=(aOther);
    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaProperty<TDataType, is_other_type_t<TDataType>>::ClassType_t&
CMetaProperty<TDataType, is_other_type_t<TDataType>>::operator=(ClassType_t&& aOther)
{
    CMetaPropertyBase<TDataType>::operator=(std::move(aOther));
    return (*this);
}


template <typename TDataType>
CMetaProperty<TDataType, is_other_type_t<TDataType>>::CMetaProperty()
    : CMetaPropertyBase<TDataType>()
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_other_type_t<TDataType>>::CMetaProperty(const ClassType_t& aOther)
    : CMetaPropertyBase<TDataType>(aOther)
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_other_type_t<TDataType>>::CMetaProperty(ClassType_t&& aOther)
    : CMetaPropertyBase<TDataType>(std::move(aOther))
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_other_type_t<TDataType>>::CMetaProperty(
        const PropertyUID_t& aUID,
        const std::string&   aPropertyName,
        const DataType_t&    aData)
    : CMetaPropertyBase<TDataType>(aUID, aPropertyName, aData)
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_other_type_t<TDataType>>::CMetaProperty(
        const PropertyUID_t& aUID,
        const std::string&   aPropertyName,
        DataType_t&&         aData)
    : CMetaPropertyBase<TDataType>(aUID, aPropertyName, std::move(aData))
{ }
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::CMetaProperty()
    : CMetaPropertyBase<TDataType>()
    , mMinValue(std::numeric_limits<TDataType>::min())
    , mMaxValue(std::numeric_limits<TDataType>::max())
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::CMetaProperty(const ClassType_t& aOther)
    : CMetaPropertyBase<TDataType>(aOther)
    , mMinValue(aOther.mMinValue)
    , mMaxValue(aOther.mMaxValue)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::CMetaProperty(ClassType_t&& aOther)
    : CMetaPropertyBase<TDataType>(std::move(aOther))
    , mMinValue(aOther.mMinValue)
    , mMaxValue(aOther.mMaxValue)
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::CMetaProperty(
        const PropertyUID_t& aUID,
        const std::string&   aPropertyName,
        const DataType_t&    aData)
    : CMetaPropertyBase<TDataType>(aUID, aPropertyName, aData)
    , mMinValue(std::numeric_limits<TDataType>::min())
    , mMaxValue(std::numeric_limits<TDataType>::max())
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::CMetaProperty(
        const PropertyUID_t& aUID,
        const std::string&   aPropertyName,
        DataType_t&&         aData)
    : CMetaPropertyBase<TDataType>(aUID, aPropertyName, std::move(aData))
    , mMinValue(std::numeric_limits<TDataType>::min())
    , mMaxValue(std::numeric_limits<TDataType>::max())
{
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::ClassType_t&
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::operator=(const ClassType_t& aOther)
{
    CMetaPropertyBase<TDataType>::operator=(aOther);
    mMinValue = aOther.mMinValue;
    mMaxValue = aOther.mMaxValue;
    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::ClassType_t&
CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::operator=(ClassType_t&& aOther)
{
    CMetaPropertyBase<TDataType>::operator=(std::move(aOther));
    mMinValue = aOther.mMinValue;
    mMaxValue = aOther.mMaxValue;
    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
typename CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::ClassType_t&
    CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::setRange(
        const DataType_t& aMin,
        const DataType_t& aMax)
{
    SR_ASSERT(aMin <= aMax);

    mMinValue = aMin;
    mMaxValue = aMax;

    // On range change, clamp the existing values to valid ranges!
    const auto adjustFn = [this](DataType_t& value) -> void
    {
        DataType_t  adjusted = std::clamp(value, mMinValue, mMaxValue);
        value = adjusted;
    };

    std::for_each(this->mValues.begin(), this->mValues.end(), adjustFn);

    return (*this);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const typename CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::DataType_t&
    CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::getMinValue() const
{
    return mMinValue;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
const typename CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::DataType_t&
    CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::getMaxValue() const
{
    return mMaxValue;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
template <typename TDataType>
bool CMetaProperty<TDataType, is_numeric_type_t<TDataType>>::checkRange(const DataType_t& aData)
{
    const bool inRange = (aData >= mMinValue && aData <= mMaxValue);
    return inRange;
}
//<-----------------------------------------------------------------------------


//
// Explicit template specialization
// -> Declare the supported types here
// IMPORTANT:
//    This has to be done for the base class, too!
//
template class CMetaPropertyBase<int8_t>;
template class CMetaPropertyBase<int16_t>;
template class CMetaPropertyBase<int32_t>;
template class CMetaPropertyBase<int64_t>;
template class CMetaPropertyBase<uint8_t>;
template class CMetaPropertyBase<uint16_t>;
template class CMetaPropertyBase<uint32_t>;
template class CMetaPropertyBase<uint64_t>;
template class CMetaPropertyBase<float>;
template class CMetaPropertyBase<double>;
template class CMetaPropertyBase<std::string>;
template class CMetaPropertyBase<std::wstring>;
template class CMetaPropertyBase<CStdSharedPtr_t<CMetaObject>>;

template class CMetaProperty<int8_t>;
template class CMetaProperty<int16_t>;
template class CMetaProperty<int32_t>;
template class CMetaProperty<int64_t>;
template class CMetaProperty<uint8_t>;
template class CMetaProperty<uint16_t>;
template class CMetaProperty<uint32_t>;
template class CMetaProperty<uint64_t>;
template class CMetaProperty<float>;
template class CMetaProperty<double>;
template class CMetaProperty<std::string>;
template class CMetaProperty<std::wstring>;
template class CMetaProperty<CStdSharedPtr_t<CMetaObject>>;

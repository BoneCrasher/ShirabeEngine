#pragma once

#include <gtest/gtest.h>
#include <property_system/sr_meta_prototype.h>

//!< Helper methods for string printing and concatenation
#define SR_STR_EXPAND(_aToken) #_aToken
#define SR_STR(_aToken)        SR_STR_EXPAND(_aToken)
//!< Concatenates a string in the format: <typename>Property, e.g. intProperty
#define SR_PROPERTY_ID(_aType) SR_STR(_aType) "Property"

//!< Convenience helper to get a property and assert successful access.
#define SR_FETCH_PROPERTY(_aObject, _aType, _aName)                              \
    EPropertySystemError _aName##PropertyFetched =                               \
        _aObject.getProperty<_aType>(SR_PROPERTY_ID(_aName), &_aName##Property); \
    ASSERT_EQ(EPropertySystemError::Ok, _aName##PropertyFetched);

/*!
 * Write a string to a file.
 *
 * @param [in] aFilename The output filename of the file to write.
 * @param [in] aData     The string data to write into the file.
 * @return               True, if successful. False, otherwise.
 */
bool writeFile(const std::string& aFilename, const std::string& aData);

/*!
 * Read a file from disk to string.
 *
 * @param [in]  aFilename The input filename of the file to read.
 * @param [out] aOutData  Placeholder for the string data to read.
 * @return                True, if successful. False otherwise.
 */
bool readFile(const std::string& aFilename, std::string& aOutData);

/*!
 * Write a byte-buffer to a binary file.
 *
 * @param [in] aFilename The output filename of the file to write.
 * @param [in] aData     The string data to write into the file.
 * @return               True, if successful. False, otherwise.
 */
bool writeBinaryFile(const std::string& aFilename, const std::vector<uint8_t>& aData);

/*!
 * Read a binary file from disk to a byte-buffer.
 *
 * @param [in]  aFilename The input filename of the file to read.
 * @param [out] aOutData  Placeholder for the byte-buffer data to read.
 * @return                True, if successful. False otherwise.
 */
bool readBinaryFile(const std::string& aFilename, std::vector<uint8_t>& aOutData);

/*!
 * Test routine to check an object for compliance with the default object instantiation of the test cases.
 *
 * @param [in] aObject The object to test.
 */
void testSerializedObject(CMetaObject& aObject);

/*!
 * Validate all values of a property against the provided extected values.
 *
 * @param [in] aProperty         The property to evaluate.
 * @param [in] aTestCapacity     The expected capacity of the property.
 * @param [in] aTestDefaultValue The expected default value of the property.
 * @param [in] aTestValues       The expected values of the property.
 * @param [in] aTestIsWritable   The expected write-mode of the property.
 * @param [in] aTestMinValue     If the property is numeric: The expected min value permitted for the property.
 * @param [in] aTestMaxValue     If the property is numeric: The expected max value permitted for the property.
 */
template <typename T>
void testPropertyValues(
        const CMetaProperty<T>& aProperty,
        const uint32_t&         aTestCapacity,
        const T&                aTestDefaultValue,
        const std::vector<T>&   aTestValues,
        const bool              aTestIsWritable,
        const T&                aTestMinValue,
        const T&                aTestMaxValue)
{
    ASSERT_EQ(aTestCapacity, aTestValues.size());

     const uint32_t& propertyCapacity     = aProperty.getCapacity();
     const T&        propertyDefaultValue = aProperty.getDefaultValue();
     const bool&     propertyIsWritable   = aProperty.isWritable();

    ASSERT_EQ(aTestCapacity,     propertyCapacity);
    ASSERT_EQ(aTestDefaultValue, propertyDefaultValue);
    ASSERT_EQ(aTestIsWritable,   propertyIsWritable);

    for(uint32_t k=0; k<aTestCapacity; ++k)
    {
        const T& propertyValue = aProperty.getValue(k);
        const T& testValue     = aTestValues.at(k);

        ASSERT_EQ(testValue, propertyValue);
    }

    if constexpr(!(std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring> || std::is_same_v<T, CStdSharedPtr_t<CMetaObject>>))
    {
        const T& propertyMinValue = aProperty.getMinValue();
        const T& propertyMaxValue = aProperty.getMaxValue();

        ASSERT_EQ(aTestMinValue, propertyMinValue);
        ASSERT_EQ(aTestMaxValue, propertyMaxValue);
    }
}

/*!
 * Validate a specific property with 'aPropertyId' against specific expected values.
 *
 * @param [in] aObject           The object expected to contain the property.
 * @param [in] aPropertyId       The id of the property to validate.
 * @param [in] aTestCapacity     The expected capacity of the property.
 * @param [in] aTestDefaultValue The expected default value of the property.
 * @param [in] aTestValues       The expected values of the property.
 * @param [in] aTestIsWritable   The expected write-mode of the property.
 * @param [in] aTestMinValue     If the property is numeric: The expected min value permitted for the property.
 * @param [in] aTestMaxValue     If the property is numeric: The expected max value permitted for the property.
 */
template <typename T>
void testProperty(
        const CMetaObject&    aObject,
        const std::string&    aPropertyId,
        const uint32_t&       aTestCapacity,
        const T&              aTestDefaultValue,
        const std::vector<T>& aTestValues,
        const bool            aTestIsWritable,
        const T&              aTestMinValue,
        const T&              aTestMaxValue)
{
    const CMetaProperty<T>* property = nullptr;

    EPropertySystemError err = aObject.getProperty<T>(aPropertyId, &property);
    ASSERT_EQ(EPropertySystemError::Ok, err);

    testPropertyValues<T>(
                *property,
                aTestCapacity,
                aTestDefaultValue,
                aTestValues,
                aTestIsWritable,
                aTestMinValue,
                aTestMaxValue);
}

/*!
 * Simple test class creating an providing an instance of CSimpleTest with a two-level
 * hierarchy.
 * All test cases involving this class test basic prototype instantiation and
 * interaction test cases.
 * It will only contain an int32_t and std::string property.
 */
class CSimpleTest
    : public CMetaObject
{
public_static_functions:
    /*!
     * Create an instance of CSimpleTest
     *
     * @param [in] aInstancePrototype      The prototype instance this instance is based on.
     * @param [in] aInstanceUID            The instance UID of the instance to create.
     * @param [in] aInstanceName           The human readable name of the instance to create.
     * @param [in] aInstancePropertyValues The override property values of the instance to create.
     * @return
     */
    static CSimpleTest* create(
            CStdSharedPtr_t<CMetaPrototype<CSimpleTest>> aInstancePrototype,
            const InstanceUID_t&                         aInstanceUID,
            const std::string&                           aInstanceName,
            const MetaPropertyMap_t&                     aInstancePropertyValues);

public_constructors:
    /*!
     * Instantiate a CSimpleTest instance with the provided uid and name.
     *
     * @param [in] aInstanceUID  The instance UID of the instance to create.
     * @param [in] aInstanceName The human readable name of the instance to create.
     */
    CSimpleTest(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName);
};

/*!
 * The CSimpleTestPrototype class implements the prototype of all CSimpleTest instances.
 */
class CSimpleTestPrototype
        : public CMetaPrototype<CSimpleTest>
{
    SR_DECLARE_PROTOTYPE(CTestPrototype)

public_constructors:
    /*!
     * Instantiate the prototype with it's attached prototypeId and CSimpleTest instance creation functor.
     *
     * @param [in] aPrototypeId The system wide prototypeId of the prototype.
     * @param [in] aCreatorFn   A functor creating a prototype-state copy instance of type CTest.
     */
    CSimpleTestPrototype(
            const std::string& aPrototypeId,
            const CreatorFn_t& aCreatorFn);

public_destructors:
    /*!
     * Destroy and run...
     */
    ~CSimpleTestPrototype() final = default;

protected_methods:
    /*!
     * Invoked on prototype instantiation.
     * Defines the internal layout and properties of the prototype for CSimpleTest.
     */
    void definePrototype();
};

/*!
 * The CExtendedTest class provides a more sophisticated structure,
 * containing one property of each supported type and respective reference values.
 * Tests related to this prototype will evaluate all template-instantiation variants
 * possible with the current prototype system.
 */
class CExtendedTest
        : public CMetaObject
{
public_static_functions:
    /*!
     * Create an instance of CExtendedTest
     *
     * @param [in] aInstancePrototype      The prototype instance this instance is based on.
     * @param [in] aInstanceUID            The instance UID of the instance to create.
     * @param [in] aInstanceName           The human readable name of the instance to create.
     * @param [in] aInstancePropertyValues The override property values of the instance to create.
     * @return
     */
    static CExtendedTest* create(
            CStdSharedPtr_t<CMetaPrototype<CExtendedTest>> aInstancePrototype,
            const InstanceUID_t&                           aInstanceUID,
            const std::string&                             aInstanceName,
            const MetaPropertyMap_t&                       aInstancePropertyValues);

public_constructors:
    /*!
     * Instantiate a CExtendedTest instance with the provided uid and name.
     *
     * @param [in] aInstanceUID  The instance UID of the instance to create.
     * @param [in] aInstanceName The human readable name of the instance to create.
     */
    CExtendedTest(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName);
};

/*!
 * The CExtendedTestPrototype class implements the prototype of all CExtendedTest instances.
 */
class CExtendedTestPrototype
        : public CMetaPrototype<CExtendedTest>
{
    SR_DECLARE_PROTOTYPE(CExtendedTestPrototype);
public_constructors:
    /*!
     * Instantiate the prototype with it's attached prototypeId and CExtendedTest instance creation functor.
     *
     * @param [in] aPrototypeId The system wide prototypeId of the prototype.
     * @param [in] aCreatorFn   A functor creating a prototype-state copy instance of type CExtendedTest.
     */
    CExtendedTestPrototype(
            const std::string& aPrototypeId,
            const CreatorFn_t& aCreatorFn);

public_destructors:
    /*!
     * Destroy and run...
     */
    ~CExtendedTestPrototype() final = default;

protected_methods:

    /*!
     * Invoked on prototype instantiation.
     * Defines the internal layout and properties of the prototype for CExtendedTest.
     */
    void definePrototype();
};

/*!
 * The CCloneTest class is another simple structured test class, which is used for
 * evaluating deep-clone operations on property-copy or instantiation.
 */
class CCloneTest
        : public CMetaObject
{
public_static_functions:
    /*!
     * Create an instance of CCloneTest
     *
     * @param [in] aInstancePrototype      The prototype instance this instance is based on.
     * @param [in] aInstanceUID            The instance UID of the instance to create.
     * @param [in] aInstanceName           The human readable name of the instance to create.
     * @param [in] aInstancePropertyValues The override property values of the instance to create.
     * @return
     */
    static CCloneTest* create(
            CStdSharedPtr_t<CMetaPrototype<CCloneTest>> aInstancePrototype,
            const InstanceUID_t&                        aInstanceUID,
            const std::string&                          aInstanceName,
            const MetaPropertyMap_t&                    aInstancePropertyValues);


public_constructors:
    /*!
     * Instantiate a CCloneTest instance with the provided uid and name.
     *
     * @param [in] aInstanceUID  The instance UID of the instance to create.
     * @param [in] aInstanceName The human readable name of the instance to create.
     */
    CCloneTest(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName);
};

/*!
 * The CCloneTestPrototype class implements the prototype of all CCloneTest instances.
 */
class CCloneTestPrototype
        : public CMetaPrototype<CCloneTest>
{
    SR_DECLARE_PROTOTYPE(CCloneTestPrototype);

public_constructors:
    /*!
     * Instantiate a CCloneTest instance with the provided uid and name.
     *
     * @param [in] aInstanceUID  The instance UID of the instance to create.
     * @param [in] aInstanceName The human readable name of the instance to create.
     */
    CCloneTestPrototype(
            const std::string& aPrototypeId,
            const CreatorFn_t& aCreatorFn);

public_destructors:
    /*!
     * Destroy and run...
     */
    ~CCloneTestPrototype() final = default;

protected_methods:
    /*!
     * Invoked on prototype instantiation.
     * Defines the internal layout and properties of the prototype for CExtendedTest.
     */
    void definePrototype();
};

/*!
 * Verify, that the simple properties and there attributes as defined in definePrototype are correct
 * after instantiation of an instance.
 *
 * @param aObject The instantiated instance to verify.
 */
void testAgainstSimpleTestPrototype(CMetaObject& aObject);

/*!
 * Verify, that the CCloneTest instance created through the clone() operation fulfills the
 * CCloneTest-prototype requirements and contains the appropriate values.
 *
 * @param aObject The clone to verify.
 */
void testClonedObject(CMetaObject& aObject);

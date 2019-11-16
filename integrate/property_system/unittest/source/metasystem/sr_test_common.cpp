#include "sr_test_common.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>

#if __GNUC__ && __GNUC__ >= 8
#include <filesystem>
#elif __GNUC__ && __GNUC__ >= 7
#include <experimental/filesystem>
#endif

#include <gtest/gtest.h>

//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
bool writeFile(const std::string& aFilename, const std::string& aData)
{
    SR_RETURN_IF(aFilename.empty(), false);
    SR_RETURN_IF(aData.empty(),     false);

    std::ofstream file(aFilename, std::ios::out);

    SR_RETURN_IF(file.bad(), false);

    file << aData;

    // File will be closed when leaving scope.
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool readFile(const std::string& aFilename, std::string &aOutData)
{
    SR_RETURN_IF(aFilename.empty(), false);

#if __GNUC__ && __GNUC__ >= 8
    bool const fileExists = std::filesystem::exists(aFilename);
#elif __GNUC__ && __GNUC__ >= 7
    bool const fileExists = std::experimental::filesystem::exists(aFilename);
#endif

    SR_RETURN_IF(!fileExists, false);

    std::ifstream file(aFilename, std::ios::in);

    SR_RETURN_IF(file.bad() || file.fail() || file.eof(), false);

    std::string data((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

    aOutData = data;
    // File will be closed when leaving scope.
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool writeBinaryFile(const std::string& aFilename, const std::vector<uint8_t>& aData)
{
    SR_RETURN_IF(aFilename.empty(), false);
    SR_RETURN_IF(aData.empty(),     false);

    std::ofstream file(aFilename, std::ios::out | std::ios::binary);

    SR_RETURN_IF(file.bad(), false);

    file.write((char*)aData.data(), aData.size());

    // File will be closed when leaving scope.
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
bool readBinaryFile(const std::string& aFilename, std::vector<uint8_t>& aOutData)
{
    SR_RETURN_IF(aFilename.empty(), false);

#if __GNUC__ && __GNUC__ >= 8
    bool const fileExists = std::filesystem::exists(aFilename);
#elif __GNUC__ && __GNUC__ >= 7
    bool const fileExists = std::experimental::filesystem::exists(aFilename);
#endif

    SR_RETURN_IF(!fileExists, false);

    std::ifstream file(aFilename, std::ios::in | std::ios::binary);

    SR_RETURN_IF(file.bad() || file.fail() || file.eof(), false);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    std::streampos fileSize = std::streampos(0);

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data;
    data.reserve(fileSize);

    data.insert(data.begin(),
                std::istream_iterator<uint8_t>(file),
                std::istream_iterator<uint8_t>());

    aOutData = data;
    // File will be closed when leaving scope.
    return true;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void testSerializedObject(CMetaObject& aObject)
{
    testProperty<int8_t>  (aObject, "int8Property",   1, 0, {23},            true, std::numeric_limits<int8_t>::min(),   std::numeric_limits<int8_t>::max());
    testProperty<int16_t> (aObject, "int16Property",  1, 0, {-4232},         true, std::numeric_limits<int16_t>::min(),  std::numeric_limits<int16_t>::max());
    testProperty<int32_t> (aObject, "int32Property",  1, 0, {2323333},       true, std::numeric_limits<int32_t>::min(),  std::numeric_limits<int32_t>::max());
    testProperty<int64_t> (aObject, "int64Property",  1, 0, {-23554545342},  true, std::numeric_limits<int64_t>::min(),  std::numeric_limits<int64_t>::max());
    testProperty<uint8_t> (aObject, "uint8Property",  2, 0, {45, 0},         true, std::numeric_limits<uint8_t>::min(),  std::numeric_limits<uint8_t>::max());
    testProperty<uint16_t>(aObject, "uint16Property", 1, 0, {0},             true, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
    testProperty<uint32_t>(aObject, "uint32Property", 1, 0, {12351611},      true, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
    testProperty<uint64_t>(aObject, "uint64Property", 1, 0, {6646777643353}, true, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
    testProperty<float>   (aObject, "floatProperty",  1, 0, {9.9999999f},    true, std::numeric_limits<float>::min(),    std::numeric_limits<float>::max());
    testProperty<double>  (aObject, "doubleProperty", 1, 0, {M_PI_2},        true, std::numeric_limits<double>::min(),   std::numeric_limits<double>::max());

    testProperty<std::string> (aObject, "stringProperty",  1, "",  {"Tralala"},                true,  "", "");
    testProperty<std::wstring>(aObject, "wstringProperty", 1, L"", {L"This is ridiculous..."}, true, L"", L"");

    testProperty<CStdSharedPtr_t<CMetaObject>>(aObject, "objectProperty", 1, nullptr, {nullptr}, true, nullptr, nullptr);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CSimpleTest::CSimpleTest(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName)
    : CMetaObject(aInstanceUID, aInstanceName)
{};
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CSimpleTest* CSimpleTest::create(
        CStdSharedPtr_t<CMetaPrototype<CSimpleTest>> aInstancePrototype,
        const InstanceUID_t&                         aInstanceUID,
        const std::string&                           aInstanceName,
        const MetaPropertyMap_t&                     aInstancePropertyValues)
{
    SR_ASSERT(nullptr != aInstancePrototype);

    CSimpleTest* test = new CSimpleTest(aInstanceUID, aInstanceName);

    test->mPrototype  = aInstancePrototype;
    test->mProperties = aInstancePropertyValues;

    return test;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CSimpleTestPrototype::CSimpleTestPrototype(
        const std::string& aPrototypeId,
        const CreatorFn_t& aCreatorFn)
    : CMetaPrototype<CSimpleTest>(aPrototypeId, aCreatorFn)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CSimpleTestPrototype::definePrototype()
{
    CMetaProperty<int32_t>& int32Property = addProperty<int32_t>("IntegralTest", 1337);
    int32Property
            .setCapacity(10)
            .setDefaultValue(100)
            .setRange(0, 100);

    CMetaProperty<std::string>& stringProperty = addProperty<std::string>("StringTest");
    stringProperty
            .setCapacity(1)
            .setWriteable(false)
            .setDefaultValue("I'm default...")
            .setValue("I'm a non-default value...");
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CExtendedTest::CExtendedTest(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName)
    : CMetaObject(aInstanceUID, aInstanceName)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CExtendedTest* CExtendedTest::create(
        CStdSharedPtr_t<CMetaPrototype<CExtendedTest>> aInstancePrototype,
        const InstanceUID_t&                           aInstanceUID,
        const std::string&                             aInstanceName,
        const MetaPropertyMap_t&                       aInstancePropertyValues)
{
    SR_ASSERT(nullptr != aInstancePrototype);

    CExtendedTest* test = new CExtendedTest(aInstanceUID, aInstanceName);
    test->mPrototype    = aInstancePrototype;
    test->mProperties   = aInstancePropertyValues;

    return test;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void testAgainstSimpleTestPrototype(CMetaObject& aObject)
{
    EPropertySystemError status = EPropertySystemError::Ok;

    // Verify successful construction of the prototype using it's callback.
    const bool hasInt32Property = aObject.hasProperty("IntegralTest");
    ASSERT_EQ(hasInt32Property, true);

    const CMetaProperty<int32_t>* int32Property = nullptr;
    status = aObject.getProperty<int32_t>("IntegralTest", &int32Property);
    ASSERT_EQ(EPropertySystemError::Ok, status);
    ASSERT_NE(int32Property, nullptr);

    const int32_t int32PropertyAccessorValue = aObject.at<int32_t>("IntegralTest").getValue();
    ASSERT_EQ(int32PropertyAccessorValue, 100);

    const uint32_t& int32PropertyCapacity     = int32Property->getCapacity();
    const int32_t&  int32PropertyDefaultValue = int32Property->getDefaultValue();
    const int32_t&  int32PropertyValue0       = int32Property->getValue();
    const int32_t&  int32PropertyValue1To9    = int32Property->getValue(1);
    const bool      int32PropertyIsWritable   = int32Property->isWritable();
    const int32_t&  int32PropertyMinValue     = int32Property->getMinValue();
    const int32_t&  int32PropertyMaxValue     = int32Property->getMaxValue();

    ASSERT_EQ(int32PropertyCapacity,     10);
    ASSERT_EQ(int32PropertyDefaultValue, 100);
    // The next two lines also test a capacity resize without overwriting old values as
    // well as clamping on range change.
    ASSERT_EQ(int32PropertyValue0,       100);
    ASSERT_EQ(int32PropertyValue1To9,    0);
    ASSERT_EQ(int32PropertyIsWritable,   true);
    ASSERT_EQ(int32PropertyMinValue,     0);
    ASSERT_EQ(int32PropertyMaxValue,     100);

    const bool hasStringProperty = aObject.hasProperty("StringTest");

    ASSERT_EQ(hasStringProperty, true);

    const CMetaProperty<std::string>* stringProperty = nullptr;
    status = aObject.getProperty<std::string>("StringTest", &stringProperty);

    ASSERT_EQ(EPropertySystemError::Ok, status);
    ASSERT_NE(stringProperty, nullptr);

    const std::string stringPropertyAccessorValue = aObject.at<std::string>("StringTest").getValue();

    ASSERT_EQ(stringPropertyAccessorValue, ""); // Property is NOT WRITABLE!

    const uint32_t&    stringPropertyCapacity     = stringProperty->getCapacity();
    const std::string& stringPropertyDefaultValue = stringProperty->getDefaultValue();
    const std::string& stringPropertyValue        = stringProperty->getValue();
    const bool         stringPropertyIsWritable   = stringProperty->isWritable();

    ASSERT_EQ(stringPropertyCapacity,     1);
    ASSERT_EQ(stringPropertyDefaultValue, "I'm default...");
    ASSERT_EQ(stringPropertyValue,        "");
    ASSERT_EQ(stringPropertyIsWritable,   false);

    // // Check, that access to unknown properties yields proper behaviour (redundant test actually...)
    // bool    hasInt16Property   = aObject.hasProperty("IntegralTest16Bit");
    // int32_t int16PropertyValue = aObject.at<int32_t>("IntegralTest16Bit").getValue();
    // ASSERT_EQ(hasInt16Property,   false);
    // ASSERT_EQ(int16PropertyValue, 0);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CExtendedTestPrototype::CExtendedTestPrototype(
        const std::string& aPrototypeId,
        const CreatorFn_t& aCreatorFn)
    : CMetaPrototype<CExtendedTest>(aPrototypeId, aCreatorFn)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CExtendedTestPrototype::definePrototype()
{
    addProperty<int8_t>                      ("int8Property"   , 23);
    addProperty<int16_t>                     ("int16Property"  , -4232);
    addProperty<int32_t>                     ("int32Property"  , 2323333);
    addProperty<int64_t>                     ("int64Property"  , -23554545342);
    addProperty<uint8_t>                     ("uint8Property"  , 45).setCapacity(2);
    addProperty<uint16_t>                    ("uint16Property" , 0);
    addProperty<uint32_t>                    ("uint32Property" , 12351611);
    addProperty<uint64_t>                    ("uint64Property" , 6646777643353);
    addProperty<float>                       ("floatProperty"  , 9.9999999f);
    addProperty<double>                      ("doubleProperty" , M_PI_2);
    addProperty<std::string>                 ("stringProperty" , "Tralala");
    addProperty<std::wstring>                ("wstringProperty", L"This is ridiculous...");
    addProperty<CStdSharedPtr_t<CMetaObject>>("objectProperty" , nullptr);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CCloneTest::CCloneTest(
        const InstanceUID_t& aInstanceUID,
        const std::string&   aInstanceName)
    : CMetaObject(aInstanceUID, aInstanceName)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CCloneTest* CCloneTest::create(
        CStdSharedPtr_t<CMetaPrototype<CCloneTest>> aInstancePrototype,
        const InstanceUID_t&                        aInstanceUID,
        const std::string&                          aInstanceName,
        const MetaPropertyMap_t&                    aInstancePropertyValues)
{
    SR_ASSERT(nullptr != aInstancePrototype);

    CCloneTest* test  = new CCloneTest(aInstanceUID, aInstanceName);
    test->mPrototype  = aInstancePrototype;
    test->mProperties = aInstancePropertyValues;

    return test;
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
CCloneTestPrototype::CCloneTestPrototype(
        const std::string& aPrototypeId,
        const CreatorFn_t& aCreatorFn)
    : CMetaPrototype<CCloneTest>(aPrototypeId, aCreatorFn)
{}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void CCloneTestPrototype::definePrototype()
{
    CCloneTest* instance = new CCloneTest(134, "alsalkf");
    CStdSharedPtr_t<CCloneTest> otherInstance = CStdSharedPtr_t<CCloneTest>(instance);
    otherInstance->addProperty<int16_t>                     ("int16Property"  , -4232);
    otherInstance->addProperty<CStdSharedPtr_t<CMetaObject>>("objectProperty" , nullptr);

    this->addProperty<int16_t>                     ("int16Property"  , -4232);
    this->addProperty<CStdSharedPtr_t<CMetaObject>>("objectProperty" , nullptr);
    this->at<CStdSharedPtr_t<CMetaObject>>("objectProperty").setValue(std::move(otherInstance));
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
void testClonedObject(CMetaObject &aObject)
{
    testProperty<int16_t>(aObject, "int16Property", 1, 0, {-4232}, true, std::numeric_limits<int16_t>::min(),  std::numeric_limits<int16_t>::max());

    CStdSharedPtr_t<CMetaObject> childObject = aObject.at<CStdSharedPtr_t<CMetaObject>>("objectProperty").getValue();
    ASSERT_NE(nullptr, childObject);

    testProperty<int16_t>(*childObject, "int16Property",  1, 0, {-4232}, true, std::numeric_limits<int16_t>::min(),  std::numeric_limits<int16_t>::max());
    testProperty<CStdSharedPtr_t<CMetaObject>>(*childObject, "objectProperty", 1, nullptr, {nullptr}, true, nullptr, nullptr);
}
//<-----------------------------------------------------------------------------

//<-----------------------------------------------------------------------------
//<
//<-----------------------------------------------------------------------------
static CMetaPrototypeBase::PrototypeStaticHandle_t gCTestPrototypeStaticHandle         = spawnPrototype<CSimpleTestPrototype>();
static CMetaPrototypeBase::PrototypeStaticHandle_t gCExtendedTestPrototypeStaticHandle = spawnPrototype<CExtendedTestPrototype>();
static CMetaPrototypeBase::PrototypeStaticHandle_t gCClonedTestPrototypeStaticHandle   = spawnPrototype<CCloneTestPrototype>();
//<-----------------------------------------------------------------------------


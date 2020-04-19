#include "sr_pch.h"
#include <stdint.h>
#include "sr_test_common.h"
#include "property_system/serialization/sr_property_json_de_serializer.h"

/*!
 * Test-Fixture to test meta object related functionality.
 */
class Fixture_MetaObject
    : public testing::Test
{
public_constructors:
    /*!
     * Default-Construct the fixture and set the internal object to nullptr.
     */
    SR_INLINE Fixture_MetaObject()
        : mMetaObject(nullptr)
    { }

public_methods:
    /*!
     * Per test case setup.
     */
    SR_INLINE void SetUp()
    {
        ::testing::Test::SetUp();

        CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();

        const InstanceUID_t objectUID = system->generateObjectUID();
        mMetaObject = std::make_shared<CMetaObject>(objectUID, "Fixture_MetaObject::TestInstance");
    }

    /*!
     * Per test case clean up.
     */
    SR_INLINE void TearDown()
    {
        mMetaObject = nullptr;

        ::testing::Test::TearDown();
    }

public_members:
    std::shared_ptr<CMetaObject> mMetaObject;
};

/*!
 * Test case to check whether adding properties to an object with corresponding default
 * values is working.
 *
 * This test case will add properties with a specific name and default value, being returned a reference to the
 * respective property on addition.
 *
 * The default values are checked against in two ways:
 * 1. Accessing the value from the returned property reference and checking them against the defaults.
 * 2. Refetching the property from the MetaObject and checking them against the defaults.
 *
 */
TEST_F(Fixture_MetaObject, Basic_AddProperties)
{
    ASSERT_NE(mMetaObject, nullptr);

    const int8_t       defaultInt8Property    = std::numeric_limits<int8_t>  ::max();
    const int16_t      defaultInt16Property   = std::numeric_limits<int16_t> ::max();
    const int32_t      defaultInt32Property   = std::numeric_limits<int32_t> ::max();
    const int64_t      defaultInt64Property   = std::numeric_limits<int64_t> ::max();
    const uint8_t      defaultUint8Property   = std::numeric_limits<uint8_t> ::max();
    const uint16_t     defaultUint16Property  = std::numeric_limits<uint16_t>::max();
    const uint32_t     defaultUint32Property  = std::numeric_limits<uint32_t>::max();
    const uint64_t     defaultUint64Property  = std::numeric_limits<uint64_t>::max();
    const float        defaultFloatProperty   = std::numeric_limits<float>   ::max();
    const double       defaultDoubleProperty  = std::numeric_limits<double>  ::max();
    const std::string  defaultStringProperty  = "Default string";
    const std::wstring defaultWstringProperty = L"Default wstring";

    CMetaProperty<int8_t>       &int8Property    = mMetaObject->addProperty<int8_t>      ("int8Property",    defaultInt8Property   );
    CMetaProperty<int16_t>      &int16Property   = mMetaObject->addProperty<int16_t>     ("int16Property",   defaultInt16Property  );
    CMetaProperty<int32_t>      &int32Property   = mMetaObject->addProperty<int32_t>     ("int32Property",   defaultInt32Property  );
    CMetaProperty<int64_t>      &int64Property   = mMetaObject->addProperty<int64_t>     ("int64Property",   defaultInt64Property  );
    CMetaProperty<uint8_t>      &uint8Property   = mMetaObject->addProperty<uint8_t>     ("uint8Property",   defaultUint8Property  );
    CMetaProperty<uint16_t>     &uint16Property  = mMetaObject->addProperty<uint16_t>    ("uint16Property",  defaultUint16Property );
    CMetaProperty<uint32_t>     &uint32Property  = mMetaObject->addProperty<uint32_t>    ("uint32Property",  defaultUint32Property );
    CMetaProperty<uint64_t>     &uint64Property  = mMetaObject->addProperty<uint64_t>    ("uint64Property",  defaultUint64Property );
    CMetaProperty<float>        &floatProperty   = mMetaObject->addProperty<float>       ("floatProperty",   defaultFloatProperty  );
    CMetaProperty<double>       &doubleProperty  = mMetaObject->addProperty<double>      ("doubleProperty",  defaultDoubleProperty );
    CMetaProperty<std::string>  &stringProperty  = mMetaObject->addProperty<std::string> ("stringProperty",  defaultStringProperty );
    CMetaProperty<std::wstring> &wstringProperty = mMetaObject->addProperty<std::wstring>("wstringProperty", defaultWstringProperty);

    ASSERT_EQ(int8Property   .getValue(), std::numeric_limits<int8_t>  ::max());
    ASSERT_EQ(int16Property  .getValue(), std::numeric_limits<int16_t> ::max());
    ASSERT_EQ(int32Property  .getValue(), std::numeric_limits<int32_t> ::max());
    ASSERT_EQ(int64Property  .getValue(), std::numeric_limits<int64_t> ::max());
    ASSERT_EQ(uint8Property  .getValue(), std::numeric_limits<uint8_t> ::max());
    ASSERT_EQ(uint16Property .getValue(), std::numeric_limits<uint16_t>::max());
    ASSERT_EQ(uint32Property .getValue(), std::numeric_limits<uint32_t>::max());
    ASSERT_EQ(uint64Property .getValue(), std::numeric_limits<uint64_t>::max());
    ASSERT_EQ(floatProperty  .getValue(), std::numeric_limits<float>   ::max());
    ASSERT_EQ(doubleProperty .getValue(), std::numeric_limits<double>  ::max());
    ASSERT_EQ(stringProperty .getValue(), "Default string");
    ASSERT_EQ(wstringProperty.getValue(), L"Default wstring");

    const int8_t       int8PropertyValue    = mMetaObject->at<int8_t>      ("int8Property"   ).getValue();
    const int16_t      int16PropertyValue   = mMetaObject->at<int16_t>     ("int16Property"  ).getValue();
    const int32_t      int32PropertyValue   = mMetaObject->at<int32_t>     ("int32Property"  ).getValue();
    const int64_t      int64PropertyValue   = mMetaObject->at<int64_t>     ("int64Property"  ).getValue();
    const uint8_t      uint8PropertyValue   = mMetaObject->at<uint8_t>     ("uint8Property"  ).getValue();
    const uint16_t     uint16PropertyValue  = mMetaObject->at<uint16_t>    ("uint16Property" ).getValue();
    const uint32_t     uint32PropertyValue  = mMetaObject->at<uint32_t>    ("uint32Property" ).getValue();
    const uint64_t     uint64PropertyValue  = mMetaObject->at<uint64_t>    ("uint64Property" ).getValue();
    const float        floatPropertyValue   = mMetaObject->at<float>       ("floatProperty"  ).getValue();
    const double       doublePropertyValue  = mMetaObject->at<double>      ("doubleProperty" ).getValue();
    const std::string  stringPropertyValue  = mMetaObject->at<std::string> ("stringProperty" ).getValue();
    const std::wstring wstringPropertyValue = mMetaObject->at<std::wstring>("wstringProperty").getValue();

    ASSERT_EQ(int8PropertyValue   , std::numeric_limits<int8_t>  ::max());
    ASSERT_EQ(int16PropertyValue  , std::numeric_limits<int16_t> ::max());
    ASSERT_EQ(int32PropertyValue  , std::numeric_limits<int32_t> ::max());
    ASSERT_EQ(int64PropertyValue  , std::numeric_limits<int64_t> ::max());
    ASSERT_EQ(uint8PropertyValue  , std::numeric_limits<uint8_t> ::max());
    ASSERT_EQ(uint16PropertyValue , std::numeric_limits<uint16_t>::max());
    ASSERT_EQ(uint32PropertyValue , std::numeric_limits<uint32_t>::max());
    ASSERT_EQ(uint64PropertyValue , std::numeric_limits<uint64_t>::max());
    ASSERT_EQ(floatPropertyValue  , std::numeric_limits<float>   ::max());
    ASSERT_EQ(doublePropertyValue , std::numeric_limits<double>  ::max());
    ASSERT_EQ(stringPropertyValue , "Default string");
    ASSERT_EQ(wstringPropertyValue, L"Default wstring");
}

/*!
 * Test case to check whether adding properties to an object with corresponding default
 * values and removing individual properties is working.
 *
 * This test case will add properties with a specific name and default value, being returned a reference to the
 * respective property on addition.
 *
 * The default values are checked against in two ways:
 * 1. Accessing the value from the returned property reference and checking them against the defaults.
 * 2. Refetching the property from the MetaObject and checking them against the defaults.
 */
TEST_F(Fixture_MetaObject, Basic_RemoveProperties)
{    
    ASSERT_NE(mMetaObject, nullptr);

    const int8_t       defaultInt8Property    = std::numeric_limits<int8_t>  ::max();
    const int16_t      defaultInt16Property   = std::numeric_limits<int16_t> ::max();
    const int32_t      defaultInt32Property   = std::numeric_limits<int32_t> ::max();
    const int64_t      defaultInt64Property   = std::numeric_limits<int64_t> ::max();
    const uint8_t      defaultUint8Property   = std::numeric_limits<uint8_t> ::max();
    const uint16_t     defaultUint16Property  = std::numeric_limits<uint16_t>::max();
    const uint32_t     defaultUint32Property  = std::numeric_limits<uint32_t>::max();
    const uint64_t     defaultUint64Property  = std::numeric_limits<uint64_t>::max();
    const float        defaultFloatProperty   = std::numeric_limits<float>   ::max();
    const double       defaultDoubleProperty  = std::numeric_limits<double>  ::max();
    const std::string  defaultStringProperty  = "Default string";
    const std::wstring defaultWstringProperty = L"Default wstring";

    CMetaProperty<int8_t>       &int8Property    = mMetaObject->addProperty<int8_t>      ("int8Property",    defaultInt8Property   );
    CMetaProperty<int16_t>      &int16Property   = mMetaObject->addProperty<int16_t>     ("int16Property",   defaultInt16Property  );
    CMetaProperty<int32_t>      &int32Property   = mMetaObject->addProperty<int32_t>     ("int32Property",   defaultInt32Property  );
    CMetaProperty<int64_t>      &int64Property   = mMetaObject->addProperty<int64_t>     ("int64Property",   defaultInt64Property  );
    CMetaProperty<uint8_t>      &uint8Property   = mMetaObject->addProperty<uint8_t>     ("uint8Property",   defaultUint8Property  );
    CMetaProperty<uint16_t>     &uint16Property  = mMetaObject->addProperty<uint16_t>    ("uint16Property",  defaultUint16Property );
    CMetaProperty<uint32_t>     &uint32Property  = mMetaObject->addProperty<uint32_t>    ("uint32Property",  defaultUint32Property );
    CMetaProperty<uint64_t>     &uint64Property  = mMetaObject->addProperty<uint64_t>    ("uint64Property",  defaultUint64Property );
    CMetaProperty<float>        &floatProperty   = mMetaObject->addProperty<float>       ("floatProperty",   defaultFloatProperty  );
    CMetaProperty<double>       &doubleProperty  = mMetaObject->addProperty<double>      ("doubleProperty",  defaultDoubleProperty );
    CMetaProperty<std::string>  &stringProperty  = mMetaObject->addProperty<std::string> ("stringProperty",  defaultStringProperty );
    CMetaProperty<std::wstring> &wstringProperty = mMetaObject->addProperty<std::wstring>("wstringProperty", defaultWstringProperty);

    // SR_UNUSED(uint32Property);
    // SR_UNUSED(wstringProperty);

    mMetaObject->onPropertyRemoved("uint32Property");
    mMetaObject->onPropertyRemoved("wstringProperty");

    ASSERT_EQ(int8Property  .getValue(), defaultInt8Property  );
    ASSERT_EQ(int16Property .getValue(), defaultInt16Property );
    ASSERT_EQ(int32Property .getValue(), defaultInt32Property );
    ASSERT_EQ(int64Property .getValue(), defaultInt64Property );
    ASSERT_EQ(uint8Property .getValue(), defaultUint8Property );
    ASSERT_EQ(uint16Property.getValue(), defaultUint16Property);
    ASSERT_EQ(uint64Property.getValue(), defaultUint64Property);
    ASSERT_EQ(floatProperty .getValue(), defaultFloatProperty );
    ASSERT_EQ(doubleProperty.getValue(), defaultDoubleProperty);
    ASSERT_EQ(stringProperty.getValue(), defaultStringProperty);

    const int8_t      int8PropertyValue    = mMetaObject->at<int8_t>     ("int8Property"  ).getValue();
    const int16_t     int16PropertyValue   = mMetaObject->at<int16_t>    ("int16Property" ).getValue();
    const int32_t     int32PropertyValue   = mMetaObject->at<int32_t>    ("int32Property" ).getValue();
    const int64_t     int64PropertyValue   = mMetaObject->at<int64_t>    ("int64Property" ).getValue();
    const uint8_t     uint8PropertyValue   = mMetaObject->at<uint8_t>    ("uint8Property" ).getValue();
    const uint16_t    uint16PropertyValue  = mMetaObject->at<uint16_t>   ("uint16Property").getValue();
    const uint64_t    uint64PropertyValue  = mMetaObject->at<uint64_t>   ("uint64Property").getValue();
    const float       floatPropertyValue   = mMetaObject->at<float>      ("floatProperty" ).getValue();
    const double      doublePropertyValue  = mMetaObject->at<double>     ("doubleProperty").getValue();
    const std::string stringPropertyValue  = mMetaObject->at<std::string>("stringProperty").getValue();

    ASSERT_DEATH(mMetaObject->at<uint32_t>   ("uint32Property" ).getValue() , ".*");
    ASSERT_DEATH(mMetaObject->at<std::wstring>("wstringProperty").getValue(), ".*");

    const bool hasUint32Property  = mMetaObject->hasProperty("uint32Property");
    const bool hasWStringProperty = mMetaObject->hasProperty("wstringProperty");

    ASSERT_EQ(false, hasUint32Property);
    ASSERT_EQ(false, hasWStringProperty);

    ASSERT_EQ(int8PropertyValue   , defaultInt8Property  );
    ASSERT_EQ(int16PropertyValue  , defaultInt16Property );
    ASSERT_EQ(int32PropertyValue  , defaultInt32Property );
    ASSERT_EQ(int64PropertyValue  , defaultInt64Property );
    ASSERT_EQ(uint8PropertyValue  , defaultUint8Property );
    ASSERT_EQ(uint16PropertyValue , defaultUint16Property);
    ASSERT_EQ(uint64PropertyValue , defaultUint64Property);
    ASSERT_EQ(floatPropertyValue  , defaultFloatProperty );
    ASSERT_EQ(doublePropertyValue , defaultDoubleProperty);
    ASSERT_EQ(stringPropertyValue , defaultStringProperty);
}

/*!
 * Test case to check whether setting and getting values inside properties is functional.
 *
 * The default values are checked against in two ways:
 * 1. Accessing the value from the returned property reference and checking them against the defaults.
 * 2. Refetching the property from the MetaObject and checking them against the defaults.
 */
TEST_F(Fixture_MetaObject, Basic_SetPropertyValue)
{
    ASSERT_NE(mMetaObject, nullptr);

    const int8_t       defaultInt8Property    = std::numeric_limits<int8_t>  ::max();
    const int16_t      defaultInt16Property   = std::numeric_limits<int16_t> ::max();
    const int32_t      defaultInt32Property   = std::numeric_limits<int32_t> ::max();
    const int64_t      defaultInt64Property   = std::numeric_limits<int64_t> ::max();
    const uint8_t      defaultUint8Property   = std::numeric_limits<uint8_t> ::max();
    const uint16_t     defaultUint16Property  = std::numeric_limits<uint16_t>::max();
    const uint32_t     defaultUint32Property  = std::numeric_limits<uint32_t>::max();
    const uint64_t     defaultUint64Property  = std::numeric_limits<uint64_t>::max();
    const float        defaultFloatProperty   = std::numeric_limits<float>   ::max();
    const double       defaultDoubleProperty  = std::numeric_limits<double>  ::max();
    const std::string  defaultStringProperty  = "Default string";
    const std::wstring defaultWstringProperty = L"Default wstring";

    CMetaProperty<int8_t>       &int8Property    = mMetaObject->addProperty<int8_t>      ("int8Property",    defaultInt8Property   );
    CMetaProperty<int16_t>      &int16Property   = mMetaObject->addProperty<int16_t>     ("int16Property",   defaultInt16Property  );
    CMetaProperty<int32_t>      &int32Property   = mMetaObject->addProperty<int32_t>     ("int32Property",   defaultInt32Property  );
    CMetaProperty<int64_t>      &int64Property   = mMetaObject->addProperty<int64_t>     ("int64Property",   defaultInt64Property  );
    CMetaProperty<uint8_t>      &uint8Property   = mMetaObject->addProperty<uint8_t>     ("uint8Property",   defaultUint8Property  );
    CMetaProperty<uint16_t>     &uint16Property  = mMetaObject->addProperty<uint16_t>    ("uint16Property",  defaultUint16Property );
    CMetaProperty<uint32_t>     &uint32Property  = mMetaObject->addProperty<uint32_t>    ("uint32Property",  defaultUint32Property );
    CMetaProperty<uint64_t>     &uint64Property  = mMetaObject->addProperty<uint64_t>    ("uint64Property",  defaultUint64Property );
    CMetaProperty<float>        &floatProperty   = mMetaObject->addProperty<float>       ("floatProperty",   defaultFloatProperty  );
    CMetaProperty<double>       &doubleProperty  = mMetaObject->addProperty<double>      ("doubleProperty",  defaultDoubleProperty );
    CMetaProperty<std::string>  &stringProperty  = mMetaObject->addProperty<std::string> ("stringProperty",  defaultStringProperty );
    CMetaProperty<std::wstring> &wstringProperty = mMetaObject->addProperty<std::wstring>("wstringProperty", defaultWstringProperty);

    const int8_t       testInt8Property    = 64;
    const int16_t      testInt16Property   = -22555;
    const int32_t      testInt32Property   = -32522223;
    const int64_t      testInt64Property   = 234263463223;
    const uint8_t      testUint8Property   = 0;
    const uint16_t     testUint16Property  = 12112;
    const uint32_t     testUint32Property  = 4342341;
    const uint64_t     testUint64Property  = 345345632;
    const float        testFloatProperty   = 1.234567f;
    const double       testDoubleProperty  = M_PI;
    const std::string  testStringProperty  = "Fughidawubidth";
    const std::wstring testWstringProperty = L"asdfjkloe";

    int8Property   .setValue(testInt8Property    );
    int16Property  .setValue(testInt16Property   );
    int32Property  .setValue(testInt32Property   );
    int64Property  .setValue(testInt64Property   );
    uint8Property  .setValue(testUint8Property   );
    uint16Property .setValue(testUint16Property  );
    uint32Property .setValue(testUint32Property  );
    uint64Property .setValue(testUint64Property  );
    floatProperty  .setValue(testFloatProperty   );
    doubleProperty .setValue(testDoubleProperty  );
    stringProperty .setValue(testStringProperty  );
    wstringProperty.setValue(testWstringProperty );

    ASSERT_EQ(int8Property   .getValue(), testInt8Property   );
    ASSERT_EQ(int16Property  .getValue(), testInt16Property  );
    ASSERT_EQ(int32Property  .getValue(), testInt32Property  );
    ASSERT_EQ(int64Property  .getValue(), testInt64Property  );
    ASSERT_EQ(uint8Property  .getValue(), testUint8Property  );
    ASSERT_EQ(uint16Property .getValue(), testUint16Property );
    ASSERT_EQ(uint32Property .getValue(), testUint32Property );
    ASSERT_EQ(uint64Property .getValue(), testUint64Property );
    ASSERT_EQ(floatProperty  .getValue(), testFloatProperty  );
    ASSERT_EQ(doubleProperty .getValue(), testDoubleProperty );
    ASSERT_EQ(stringProperty .getValue(), testStringProperty );
    ASSERT_EQ(wstringProperty.getValue(), testWstringProperty);

    const int8_t       int8PropertyValue    = mMetaObject->at<int8_t>      ("int8Property"   ).getValue();
    const int16_t      int16PropertyValue   = mMetaObject->at<int16_t>     ("int16Property"  ).getValue();
    const int32_t      int32PropertyValue   = mMetaObject->at<int32_t>     ("int32Property"  ).getValue();
    const int64_t      int64PropertyValue   = mMetaObject->at<int64_t>     ("int64Property"  ).getValue();
    const uint8_t      uint8PropertyValue   = mMetaObject->at<uint8_t>     ("uint8Property"  ).getValue();
    const uint16_t     uint16PropertyValue  = mMetaObject->at<uint16_t>    ("uint16Property" ).getValue();
    const uint32_t     uint32PropertyValue  = mMetaObject->at<uint32_t>    ("uint32Property" ).getValue();
    const uint64_t     uint64PropertyValue  = mMetaObject->at<uint64_t>    ("uint64Property" ).getValue();
    const float        floatPropertyValue   = mMetaObject->at<float>       ("floatProperty"  ).getValue();
    const double       doublePropertyValue  = mMetaObject->at<double>      ("doubleProperty" ).getValue();
    const std::string  stringPropertyValue  = mMetaObject->at<std::string> ("stringProperty" ).getValue();
    const std::wstring wstringPropertyValue = mMetaObject->at<std::wstring>("wstringProperty").getValue();

    ASSERT_EQ(int8PropertyValue   , testInt8Property   );
    ASSERT_EQ(int16PropertyValue  , testInt16Property  );
    ASSERT_EQ(int32PropertyValue  , testInt32Property  );
    ASSERT_EQ(int64PropertyValue  , testInt64Property  );
    ASSERT_EQ(uint8PropertyValue  , testUint8Property  );
    ASSERT_EQ(uint16PropertyValue , testUint16Property );
    ASSERT_EQ(uint32PropertyValue , testUint32Property );
    ASSERT_EQ(uint64PropertyValue , testUint64Property );
    ASSERT_EQ(floatPropertyValue  , testFloatProperty  );
    ASSERT_EQ(doublePropertyValue , testDoubleProperty );
    ASSERT_EQ(stringPropertyValue , testStringProperty );
    ASSERT_EQ(wstringPropertyValue, testWstringProperty);
}

/*!
 * Test case to check whether setting and getting values of deleted properties is functional.
 * Death is expected for these operations.
 */
TEST_F(Fixture_MetaObject, Basic_SetPropertyValueWithDeletedProperty)
{
    ASSERT_NE(mMetaObject, nullptr);

    const int8_t       defaultInt8Property    = std::numeric_limits<int8_t>  ::max();
    const int16_t      defaultInt16Property   = std::numeric_limits<int16_t> ::max();
    const int32_t      defaultInt32Property   = std::numeric_limits<int32_t> ::max();
    const int64_t      defaultInt64Property   = std::numeric_limits<int64_t> ::max();
    const uint8_t      defaultUint8Property   = std::numeric_limits<uint8_t> ::max();
    const uint16_t     defaultUint16Property  = std::numeric_limits<uint16_t>::max();
    const uint32_t     defaultUint32Property  = std::numeric_limits<uint32_t>::max();
    const uint64_t     defaultUint64Property  = std::numeric_limits<uint64_t>::max();
    const float        defaultFloatProperty   = std::numeric_limits<float>   ::max();
    const double       defaultDoubleProperty  = std::numeric_limits<double>  ::max();
    const std::string  defaultStringProperty  = "Default string";
    const std::wstring defaultWstringProperty = L"Default wstring";

    CMetaProperty<int8_t>       &int8Property    = mMetaObject->addProperty<int8_t>      ("int8Property",    defaultInt8Property   );
    CMetaProperty<int16_t>      &int16Property   = mMetaObject->addProperty<int16_t>     ("int16Property",   defaultInt16Property  );
    CMetaProperty<int32_t>      &int32Property   = mMetaObject->addProperty<int32_t>     ("int32Property",   defaultInt32Property  );
    CMetaProperty<int64_t>      &int64Property   = mMetaObject->addProperty<int64_t>     ("int64Property",   defaultInt64Property  );
    CMetaProperty<uint8_t>      &uint8Property   = mMetaObject->addProperty<uint8_t>     ("uint8Property",   defaultUint8Property  );
    CMetaProperty<uint16_t>     &uint16Property  = mMetaObject->addProperty<uint16_t>    ("uint16Property",  defaultUint16Property );
    CMetaProperty<uint32_t>     &uint32Property  = mMetaObject->addProperty<uint32_t>    ("uint32Property",  defaultUint32Property );
    CMetaProperty<uint64_t>     &uint64Property  = mMetaObject->addProperty<uint64_t>    ("uint64Property",  defaultUint64Property );
    CMetaProperty<float>        &floatProperty   = mMetaObject->addProperty<float>       ("floatProperty",   defaultFloatProperty  );
    CMetaProperty<double>       &doubleProperty  = mMetaObject->addProperty<double>      ("doubleProperty",  defaultDoubleProperty );
    CMetaProperty<std::string>  &stringProperty  = mMetaObject->addProperty<std::string> ("stringProperty",  defaultStringProperty );
    CMetaProperty<std::wstring> &wstringProperty = mMetaObject->addProperty<std::wstring>("wstringProperty", defaultWstringProperty);

    // SR_UNUSED(uint32Property);
    // SR_UNUSED(wstringProperty);

    mMetaObject->onPropertyRemoved("uint32Property");
    mMetaObject->onPropertyRemoved("wstringProperty");

    const int8_t       testInt8Property    = 64;
    const int16_t      testInt16Property   = -22555;
    const int32_t      testInt32Property   = -32522223;
    const int64_t      testInt64Property   = 234263463223;
    const uint8_t      testUint8Property   = 0;
    const uint16_t     testUint16Property  = 12112;
    const uint32_t     testUint32Property  = 4342341;
    const uint64_t     testUint64Property  = 345345632;
    const float        testFloatProperty   = 1.234567f;
    const double       testDoubleProperty  = M_PI;
    const std::string  testStringProperty  = "Fughidawubidth";
    const std::wstring testWstringProperty = L"asdfjkloe";

    int8Property  .setValue(testInt8Property  );
    int16Property .setValue(testInt16Property );
    int32Property .setValue(testInt32Property );
    int64Property .setValue(testInt64Property );
    uint8Property .setValue(testUint8Property );
    uint16Property.setValue(testUint16Property);
    uint64Property.setValue(testUint64Property);
    floatProperty .setValue(testFloatProperty );
    doubleProperty.setValue(testDoubleProperty);
    stringProperty.setValue(testStringProperty);

    ASSERT_DEATH(mMetaObject->at<uint32_t>("uint32Property" ).setValue(4342341),          ".*");
    ASSERT_DEATH(mMetaObject->at<std::wstring>("wstringProperty").setValue(L"asdfjkloe"), ".*");

    const int8_t      int8PropertyValue    = mMetaObject->at<int8_t>      ("int8Property"   ).getValue();
    const int16_t     int16PropertyValue   = mMetaObject->at<int16_t>     ("int16Property"  ).getValue();
    const int32_t     int32PropertyValue   = mMetaObject->at<int32_t>     ("int32Property"  ).getValue();
    const int64_t     int64PropertyValue   = mMetaObject->at<int64_t>     ("int64Property"  ).getValue();
    const uint8_t     uint8PropertyValue   = mMetaObject->at<uint8_t>     ("uint8Property"  ).getValue();
    const uint16_t    uint16PropertyValue  = mMetaObject->at<uint16_t>    ("uint16Property" ).getValue();
    const uint64_t    uint64PropertyValue  = mMetaObject->at<uint64_t>    ("uint64Property" ).getValue();
    const float       floatPropertyValue   = mMetaObject->at<float>       ("floatProperty"  ).getValue();
    const double      doublePropertyValue  = mMetaObject->at<double>      ("doubleProperty" ).getValue();
    const std::string stringPropertyValue  = mMetaObject->at<std::string> ("stringProperty" ).getValue();

    ASSERT_DEATH(mMetaObject->at<uint32_t>("uint32Property" ).getValue(),     ".*");
    ASSERT_DEATH(mMetaObject->at<std::wstring>("wstringProperty").getValue(), ".*");

    ASSERT_EQ(int8PropertyValue   , testInt8Property  );
    ASSERT_EQ(int16PropertyValue  , testInt16Property );
    ASSERT_EQ(int32PropertyValue  , testInt32Property );
    ASSERT_EQ(int64PropertyValue  , testInt64Property );
    ASSERT_EQ(uint8PropertyValue  , testUint8Property );
    ASSERT_EQ(uint16PropertyValue , testUint16Property);
    ASSERT_EQ(uint64PropertyValue , testUint64Property);
    ASSERT_EQ(floatPropertyValue  , testFloatProperty );
    ASSERT_EQ(doublePropertyValue , testDoubleProperty);
    ASSERT_EQ(stringPropertyValue , testStringProperty);
}

/*!
 * Test case to check access with empty ids. Death is expected from these operations.
 */
TEST_F(Fixture_MetaObject, Basic_AddAndGetPropertiesWithEmptyId)
{
    ASSERT_NE   (mMetaObject, nullptr);
    ASSERT_DEATH(mMetaObject->addProperty("", std::numeric_limits<int8_t>::max()), ".*");
    ASSERT_EQ   (mMetaObject->getPropertyCount(), 0);

    mMetaObject->addProperty("int8Property", std::numeric_limits<int8_t>::max());
    ASSERT_EQ(mMetaObject->getPropertyCount(), 1);
}

/*!
 * Test case to check onChanged handlers for a property.
 */
TEST_F(Fixture_MetaObject, Basic_ListenToProperty)
{
    ASSERT_NE(mMetaObject, nullptr);

    static constexpr const int8_t sCheckValue = int8_t(134);
    CMetaProperty<int8_t>&        property    = mMetaObject->addProperty("int8Property", std::numeric_limits<int8_t>::max());

    bool handlerInvoked = false;
    CObserver<CMetaProperty<int8_t>, const int8_t, const uint32_t>::HandlerFunction_t handler =
            [&] (
            const CMetaProperty<int8_t>& aProperty,
            const int8_t&                aNewValue,
            const uint32_t&              aValueIndex)
    {
        SR_UNUSED(aProperty);
        SR_UNUSED(aValueIndex);

        ASSERT_EQ(sCheckValue, aNewValue);
        handlerInvoked = true;
    };

    // Setup of observers
    CStdSharedPtr_t<CObserver<CMetaProperty<int8_t>, const int8_t, const uint32_t>> observer  = nullptr;
    CStdSharedPtr_t<IObserver<const int8_t, const uint32_t>>                        iobserver = nullptr;

    observer  = makeStdSharedPtr<CObserver<CMetaProperty<int8_t>, const int8_t, const uint32_t>>(property);
    iobserver = std::static_pointer_cast<IObserver<const int8_t, const uint32_t>>(observer);

    observer->setHandlerFn(handler);

    // Observe and await value change from callback function.
    const bool observing = property.observe(iobserver, 0);
    ASSERT_EQ(true, observing);

    property.setValue(sCheckValue);
    ASSERT_EQ(true, handlerInvoked);

    // Reset value, ignore and expect the callback to not be called anymore...
    handlerInvoked = false;
    const bool ignoring = property.ignore(iobserver, 0);
    ASSERT_EQ(true, ignoring);

    property.setValue(sCheckValue);    
    ASSERT_EQ(false, handlerInvoked);
}

/*!
 * Test case to check instance creation from a prototype, adding another non-prototype property.
 * Since we create an object from the outside, then create and initialize a property with it,
 * copying the object is not wanted. Consequently we std::move it into the property, causing the
 * original pointer to become "nullptr".
 *
 * The check for a successful move can be performed using the underlying memory addressed.
 * If they're equal, the object has successfully been moved into the property storage.
 */
TEST_F(Fixture_MetaObject, Complex_AddObjectPropertyFromPrototype)
{
    ASSERT_NE(mMetaObject, nullptr);

    const CStdSharedPtr_t<CSimpleTestPrototype> prototype = CMetaSystem::get()->getPrototype<CSimpleTestPrototype>("CTestPrototype");
    const CStdSharedPtr_t<CSimpleTest>          object    = prototype->createTypedInstance(1337, "TestInstance");

    const uint64_t objectAddress = reinterpret_cast<uint64_t>(&(*object));

    mMetaObject->addProperty<CStdSharedPtr_t<CMetaObject>>("objectProperty",  std::move(object));

    const CStdSharedPtr_t<CMetaObject> objectPropertyValue = mMetaObject->at<CStdSharedPtr_t<CMetaObject>>("objectProperty").getValue();

    const uint64_t movedObjectAddress = reinterpret_cast<uint64_t>(&(*objectPropertyValue));

    ASSERT_EQ(nullptr, object);
    ASSERT_NE(nullptr, objectPropertyValue);
    ASSERT_EQ(objectAddress, movedObjectAddress);

    testAgainstSimpleTestPrototype(*objectPropertyValue);
}

/*!
 * Test to check, whether cloning works as expected.
 *
 * The test performs two cloning operations:
 * 1. Clone from prototype created instance.
 * 2. Clone from manually created object instance.
 *
 * Both variants will provide a base class pointer to CMetaObject (untyped).
 * A cast to the expected specific type will be performed, to make sure, that
 * the cloning from object and cloning from prototype worked properly.
 *
 */
TEST_F(Fixture_MetaObject, Complex_CloneTest)
{
    ASSERT_NE(mMetaObject, nullptr);

    // First clone: From Prototype
    CStdSharedPtr_t<CCloneTestPrototype> prototype = CMetaSystem::get()->getPrototype<CCloneTestPrototype>("CCloneTestPrototype");
    CStdSharedPtr_t<CCloneTest>          object    = prototype->createTypedInstance(1337, "TestInstance");

    CStdSharedPtr_t<CMetaObject> clonedUntyped = object->clone();
    ASSERT_NE(nullptr, clonedUntyped);

    CStdSharedPtr_t<CCloneTest> clonedTyped = std::static_pointer_cast<CCloneTest>(clonedUntyped);
    ASSERT_NE(nullptr, clonedTyped);

    testClonedObject(*clonedUntyped);
    testClonedObject(*clonedTyped);

    // Second clone: From manually created instance
    CMetaObject *instance = CMetaObject::create(nullptr, 1338, "TestInstanceManual", {});
    CStdSharedPtr_t<CMetaObject> manuallyCreatedInstance = CStdSharedPtr_t<CMetaObject>(instance);
    manuallyCreatedInstance->addProperty<int16_t>                     ("int16Property"  , -4232);
    manuallyCreatedInstance->addProperty<CStdSharedPtr_t<CMetaObject>>("objectProperty" , nullptr);

    CMetaObject *otherInstance = CMetaObject::create(nullptr, 1355, "DarnDarnDaaaarn", {});
    CStdSharedPtr_t<CMetaObject> manuallyCreatedOtherInstance = CStdSharedPtr_t<CMetaObject>(otherInstance);
    manuallyCreatedOtherInstance->addProperty<int16_t>                     ("int16Property"  , -4232);
    manuallyCreatedOtherInstance->addProperty<CStdSharedPtr_t<CMetaObject>>("objectProperty" , nullptr);

    instance->at<CStdSharedPtr_t<CMetaObject>>("objectProperty").setValue(std::move(manuallyCreatedOtherInstance));

    // Perform the cloning here...
    CStdSharedPtr_t<CMetaObject> clonedManualUntyped = manuallyCreatedInstance->clone();
    ASSERT_NE(nullptr, clonedManualUntyped);

    CStdSharedPtr_t<CMetaObject> clonedManualTyped = std::static_pointer_cast<CCloneTest>(clonedManualUntyped);
    ASSERT_NE(nullptr, clonedManualTyped);

    testClonedObject(*clonedManualUntyped);
    testClonedObject(*clonedManualTyped);

    // Since cloning should create new instance and consequently new UIDs, write out the trees for manual assessment
    // of the UIDs...
    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> originalObjectResult      = nullptr;
    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> untypedObjectResult       = nullptr;
    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> typedObjectResult         = nullptr;
    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> manualUntypedObjectResult = nullptr;
    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> manualTypedObjectResult   = nullptr;

    CPropertySerializer serializer {};
    serializer.initialize();
    serializer.serialize(*object,              originalObjectResult);
    serializer.serialize(*clonedUntyped,       untypedObjectResult);
    serializer.serialize(*clonedTyped,         typedObjectResult);
    serializer.serialize(*clonedManualUntyped, manualUntypedObjectResult);
    serializer.serialize(*clonedManualTyped,   manualTypedObjectResult);

    std::string originalObjectString = "";
    originalObjectResult->asString(originalObjectString);
    writeFile("OriginalObjectResult.data.txt", originalObjectString);

    std::string untypedObjectString = "";
    untypedObjectResult->asString(untypedObjectString);
    writeFile("UntypedPrototypeCloneResult.data.txt", untypedObjectString);

    std::string typedObjectString = "";
    typedObjectResult->asString(typedObjectString);
    writeFile("TypedPrototypeCloneResult.data.txt", typedObjectString);

    std::string untypedManualObjectString = "";
    manualUntypedObjectResult->asString(untypedManualObjectString);
    writeFile("UntypedManualCloneResult.data.txt", untypedManualObjectString);

    std::string typedManualObjectString = "";
    manualTypedObjectResult->asString(typedManualObjectString);
    writeFile("TypedManualCloneResult.data.txt", typedManualObjectString);
}

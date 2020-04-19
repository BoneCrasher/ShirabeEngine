#include "sr_pch.h"

#include <random>

#include <property_system/serialization/sr_property_json_de_serializer.h>

#include "sr_test_common.h"


/*!
 * @brief The Fixture_PropertySystemSerialization class
 */
class Fixture_PropertySystemSerialization
        : public ::testing::Test
{
public_methods:
    void SetUp()
    {
        ::testing::Test::SetUp();

        CStdSharedPtr_t<CExtendedTestPrototype> prototype = CMetaSystem::get()->getPrototype<CExtendedTestPrototype>("CExtendedTestPrototype");

        CStdSharedPtr_t<CMetaObject> otherObject = prototype->createAbstractInstance(50000, "OtherTestObject");
        {
        }

        mObject = prototype->createAbstractInstance(1337, "TestObject");
        {
            mObject->at<int8_t>                      ("int8Property"   ).setValue(23);
            mObject->at<int16_t>                     ("int16Property"  ).setValue(-4232);
            mObject->at<int32_t>                     ("int32Property"  ).setValue(2323333);
            mObject->at<int64_t>                     ("int64Property"  ).setValue(-23554545342);
            mObject->at<uint8_t>                     ("uint8Property"  ).setValue(45, 0);
            mObject->at<uint8_t>                     ("uint8Property"  ).setValue(123, 1);
            mObject->at<uint16_t>                    ("uint16Property" ).setValue(0);
            mObject->at<uint32_t>                    ("uint32Property" ).setValue(12351611);
            mObject->at<uint64_t>                    ("uint64Property" ).setValue(6646777643353);
            mObject->at<float>                       ("floatProperty"  ).setValue(9.9999999f);
            mObject->at<double>                      ("doubleProperty" ).setValue(M_PI_2);
            mObject->at<std::string>                 ("stringProperty" ).setValue("Tralala");
            mObject->at<std::wstring>                ("wstringProperty").setValue(L"This is ridiculous...");
            mObject->at<CStdSharedPtr_t<CMetaObject>>("objectProperty" ).setValue(otherObject);
        }
    }

    void TearDown()
    {
        ::testing::Test::TearDown();
    }

protected_members:
    CStdSharedPtr_t<CMetaObject> mObject;
};


/*!
 * @brief TEST_F
 */
TEST_F(Fixture_PropertySystemSerialization, Serialization)
{
    std::string          serializationRefString{};
    std::vector<uint8_t> serializationRefBuffer{};
    const bool refFileRead0 = readFile      ("serialization.data.ref.txt",        serializationRefString);
    const bool refFileRead1 = readBinaryFile("serialization.data.binary.ref.txt", serializationRefBuffer);
    ASSERT_EQ(true, refFileRead0);
    ASSERT_EQ(true, refFileRead1);

    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> result = nullptr;

    std::string          stringified = "";
    std::vector<uint8_t> buffer{};

    //CPropertySerializer::SSerializerSettings settings{};
    //settings.writeUID = false;

    CPropertySerializer serializer; //{settings};
    const bool initialized = serializer.initialize();
    ASSERT_EQ(true, initialized);
    const bool serialized  = serializer.serialize(*mObject, result);
    ASSERT_EQ(true, serialized);

    const bool stringFetchedSuccessfully = result->asString(stringified);
    ASSERT_EQ(true, stringFetchedSuccessfully);

    writeFile("serialization.data.txt", stringified);
    ASSERT_EQ(stringified, serializationRefString);

    const bool bufferFetchedSuccessfully = result->asBinaryBuffer(buffer);
    ASSERT_EQ(true, bufferFetchedSuccessfully);

    writeBinaryFile("serialization.data.binary.txt", buffer);
    ASSERT_EQ(buffer, serializationRefBuffer);

    const bool deinitialized = serializer.deinitialize();
    ASSERT_EQ(true, deinitialized);
}

TEST_F(Fixture_PropertySystemSerialization, Deserialization)
{
    // ----------------------------------------------------------------------
    // LOAD REFERENCE JSON DATA
    // ----------------------------------------------------------------------

    std::string          serializationRefString{};
    std::vector<uint8_t> serializationRefBuffer{};
    const bool refFileRead0 = readFile      ("deserialization.data.ref.txt",        serializationRefString);
    const bool refFileRead1 = readBinaryFile("deserialization.data.binary.ref.txt", serializationRefBuffer);
    ASSERT_EQ(true, refFileRead0);
    ASSERT_EQ(true, refFileRead1);

    // ----------------------------------------------------------------------
    // SERIALIZE FIRST, TO HAVE SOME INPUT DATA FOR SERIALIZATION.
    //
    // THIS TEST ASSUMES, THAT THE SERIALIZATION-TEST HAS PASSED.
    // ----------------------------------------------------------------------

    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> result = nullptr;

    std::string          stringified = "";
    std::vector<uint8_t> buffer{};

    CPropertySerializer serializer{};
    const bool initialized = serializer.initialize();
    ASSERT_EQ(true, initialized);
    const bool serialized  = serializer.serialize(*mObject, result);
    ASSERT_EQ(true, serialized);

    const bool stringFetchedSuccessfully = result->asString(stringified);
    ASSERT_EQ(true, stringFetchedSuccessfully);

    writeFile("deserialization.data.txt", stringified);

    ASSERT_EQ(stringified, serializationRefString);


    const bool bufferFetchedSuccessfully = result->asBinaryBuffer(buffer);
    ASSERT_EQ(true, bufferFetchedSuccessfully);

    writeBinaryFile("deserialization.data.binary.txt", buffer);

    ASSERT_EQ(buffer, serializationRefBuffer);

    const bool deinitialized = serializer.deinitialize();
    ASSERT_EQ(true, deinitialized);

    // ----------------------------------------------------------------------
    // NOW DESERIALIZE THE PREVIOUSLY GENERATED SERIALIZATION DATA
    // ----------------------------------------------------------------------

    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> deserializationStringResult = nullptr;
    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> deserializationBinaryResult = nullptr;
    std::string          stringData = "";
    std::vector<uint8_t> binaryData = {};

    const bool plainTextFileRead = readFile("deserialization.data.txt", stringData);
    const bool binaryFileRead    = readBinaryFile("deserialization.data.binary.txt", binaryData);
    ASSERT_EQ(true, plainTextFileRead);
    ASSERT_EQ(true, binaryFileRead);

    CPropertyDeserializer deserializer{};
    const bool deserializerInitialized        = deserializer.initialize();
    ASSERT_EQ(true, deserializerInitialized);
    const bool deserializerStringDeserialized = deserializer.deserialize(stringData, deserializationStringResult);
    ASSERT_EQ(true, deserializerStringDeserialized);

    CStdSharedPtr_t<CPropertyDeserializer::CDeserializationResult> specificResultType = std::static_pointer_cast<CPropertyDeserializer::CDeserializationResult>(deserializationStringResult);

    CPropertyDeserializer::PathMap_t pathMap{};

    const bool pathMapFetched = specificResultType->getPropertyPathMap(pathMap);
    ASSERT_EQ(true, pathMapFetched);

    std::vector<std::string> expectedPathMapKeys = {
        "/int8Property",
        "/int16Property",
        "/int32Property",
        "/int64Property",
        "/uint8Property",
        "/uint16Property",
        "/uint32Property",
        "/uint64Property",
        "/floatProperty",
        "/doubleProperty",
        "/stringProperty",
        "/wstringProperty",
        "/objectProperty/0/int8Property",
        "/objectProperty/0/int16Property",
        "/objectProperty/0/int32Property",
        "/objectProperty/0/int64Property",
        "/objectProperty/0/uint8Property",
        "/objectProperty/0/uint16Property",
        "/objectProperty/0/uint32Property",
        "/objectProperty/0/uint64Property",
        "/objectProperty/0/floatProperty",
        "/objectProperty/0/doubleProperty",
        "/objectProperty/0/stringProperty",
        "/objectProperty/0/wstringProperty",
        "/objectProperty/0/wstringProperty",
        "/objectProperty/0/objectProperty",
    };

    for(const std::string& checkKey : expectedPathMapKeys)
    {
        const bool contained = pathMap.contains(checkKey);
        ASSERT_EQ(true, contained);
    }

    const bool deserializerDeinitialized = deserializer.deinitialize();
    ASSERT_EQ(true, deserializerDeinitialized);

    CStdSharedPtr_t<CMetaObject> stringResult = nullptr;
    const bool hasStringValue = deserializationStringResult->asT(stringResult);
    ASSERT_EQ(true, hasStringValue);

    // ----------------------------------------------------------------------
    // DO A PER-ATTRIBUTE/VALUE COMPARISON OF THE DESERIALIZED OBJECT AGAINST SETTINGS.
    // ----------------------------------------------------------------------
    CMetaObject*   pObject = stringResult.get();
    CExtendedTest* pClass = dynamic_cast<CExtendedTest*>(pObject);
    bool isExtendedObjectInstance = (nullptr != pClass);
    ASSERT_EQ(true, isExtendedObjectInstance);

    testProperty<int8_t>  ((*stringResult), "int8Property",   1, 0, {23},            true, std::numeric_limits<int8_t>::min(),   std::numeric_limits<int8_t>::max());
    testProperty<int16_t> ((*stringResult), "int16Property",  1, 0, {-4232},         true, std::numeric_limits<int16_t>::min(),  std::numeric_limits<int16_t>::max());
    testProperty<int32_t> ((*stringResult), "int32Property",  1, 0, {2323333},      true, std::numeric_limits<int32_t>::min(),  std::numeric_limits<int32_t>::max());
    testProperty<int64_t> ((*stringResult), "int64Property",  1, 0, {-23554545342},  true, std::numeric_limits<int64_t>::min(),  std::numeric_limits<int64_t>::max());
    testProperty<uint8_t> ((*stringResult), "uint8Property",  2, 0, {45, 123},       true, std::numeric_limits<uint8_t>::min(),  std::numeric_limits<uint8_t>::max());
    testProperty<uint16_t>((*stringResult), "uint16Property", 1, 0, {0},             true, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
    testProperty<uint32_t>((*stringResult), "uint32Property", 1, 0, {12351611},      true, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
    testProperty<uint64_t>((*stringResult), "uint64Property", 1, 0, {6646777643353}, true, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
    testProperty<float>   ((*stringResult), "floatProperty",  1, 0, {9.9999999f},    true, std::numeric_limits<float>::min(),    std::numeric_limits<float>::max());
    testProperty<double>  ((*stringResult), "doubleProperty", 1, 0, {M_PI_2},        true, std::numeric_limits<double>::min(),   std::numeric_limits<double>::max());

    testProperty<std::string> ((*stringResult), "stringProperty",  1, "", {"Tralala"},                true,  "", "");
    testProperty<std::wstring>((*stringResult), "wstringProperty", 1, L"", {L"This is ridiculous..."}, true, L"", L"");

    const CMetaProperty<CStdSharedPtr_t<CMetaObject>>* objectProperty = nullptr;
    SR_FETCH_PROPERTY((*stringResult), CStdSharedPtr_t<CMetaObject>, object);
    testSerializedObject(*(objectProperty->getValue()));

    pObject = objectProperty->getValue().get();
    pClass = dynamic_cast<CExtendedTest*>(pObject);
    isExtendedObjectInstance = (nullptr != pClass);
    ASSERT_EQ(true, isExtendedObjectInstance);

    // ----------------------------------------------------------------------
    // SERIALIZE AGAIN AND CHECK AGAINST REFERENCE DATA
    // ----------------------------------------------------------------------

    serializer.initialize();
    serializer.serialize(*stringResult, result);

    const bool reserializationSuccessfulFromString = result->asString(stringified);
    ASSERT_EQ(stringified, serializationRefString);
    serializer.deinitialize();

    const bool deserializerBinaryDeserialized = deserializer.deserialize(binaryData, deserializationBinaryResult);
    ASSERT_EQ(true, deserializerBinaryDeserialized);

    CStdSharedPtr_t<CMetaObject> binaryResult = nullptr;
    bool hasBinaryValue = deserializationBinaryResult->asT(binaryResult);
    ASSERT_EQ(true, hasBinaryValue);

    serializer.initialize();
    serializer.serialize(*stringResult, result);
    const bool reserializationSuccessfulFromBinary = result->asBinaryBuffer(buffer);
    ASSERT_EQ(buffer, serializationRefBuffer);
    serializer.deinitialize();

    SR_UNUSED(reserializationSuccessfulFromString);
    SR_UNUSED(reserializationSuccessfulFromBinary);
}

/*!
 * @brief TEST_F
 */
TEST_F(Fixture_PropertySystemSerialization, DynamicDeserialization)
{
    // ----------------------------------------------------------------------
    // LOAD REFERENCE JSON DATA
    // ----------------------------------------------------------------------

    std::string serializationRefString{};
    std::string serializationRefStringDynamic{};
    const bool refFileRead0 = readFile("deserialization.prototype.data.ref.txt", serializationRefString);
    const bool refFileRead1 = readFile("deserialization.dynamic.data.ref.txt",   serializationRefStringDynamic);
    ASSERT_EQ(true, refFileRead0);
    ASSERT_EQ(true, refFileRead1);

    // ----------------------------------------------------------------------
    // SERIALIZE FIRST, TO HAVE SOME INPUT DATA FOR SERIALIZATION.
    //
    // THIS TEST ASSUMES, THAT THE SERIALIZATION-TEST HAS PASSED.
    // ----------------------------------------------------------------------

    CStdSharedPtr_t<ISerializer<CMetaObject>::IResult> result = nullptr;

    std::string stringified = "";

    CPropertySerializer serializer{};
    const bool initialized = serializer.initialize();
    ASSERT_EQ(true, initialized);
    const bool serialized  = serializer.serialize(*mObject, result);
    ASSERT_EQ(true, serialized);

    const bool stringFetchedSuccessfully = result->asString(stringified);
    ASSERT_EQ(true, stringFetchedSuccessfully);

    writeFile("deserialization.dynamic.input.data.txt", stringified);

    ASSERT_EQ(stringified, serializationRefString);

    const bool deinitialized = serializer.deinitialize();
    ASSERT_EQ(true, deinitialized);

    // ----------------------------------------------------------------------
    // NOW DESERIALIZE THE PREVIOUSLY GENERATED SERIALIZATION DATA
    // ----------------------------------------------------------------------

    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> deserializationStringResult = nullptr;

    std::string stringData = "";

    const bool plainTextFileRead = readFile("deserialization.dynamic.input.data.txt", stringData);
    ASSERT_EQ(true, plainTextFileRead);

    const bool dynamicDeserializationMode = true;
    CPropertyDeserializer deserializer = { dynamicDeserializationMode };

    const bool deserializerInitialized = deserializer.initialize();
    ASSERT_EQ(true, deserializerInitialized);
    const bool deserializerStringDeserialized = deserializer.deserialize(stringData, deserializationStringResult);
    ASSERT_EQ(true, deserializerStringDeserialized);

    const bool deserializerDeinitialized = deserializer.deinitialize();
    ASSERT_EQ(true, deserializerDeinitialized);

    CStdSharedPtr_t<CMetaObject> stringResult = nullptr;
    const bool hasStringValue = deserializationStringResult->asT(stringResult);
    ASSERT_EQ(true, hasStringValue);

    // ----------------------------------------------------------------------
    // DO A PER-ATTRIBUTE/VALUE COMPARISON OF THE DESERIALIZED OBJECT AGAINST SETTINGS.
    // ----------------------------------------------------------------------
    CMetaObject*   pObject = stringResult.get();
    CExtendedTest* pClass = dynamic_cast<CExtendedTest*>(pObject);
    bool isExtendedObjectInstance = (nullptr != pClass);
    ASSERT_EQ(false, isExtendedObjectInstance); // Due to dynamic deserialization, we do not base on the prototype!

    testProperty<int8_t>  ((*stringResult), "int8Property",   1, 0, {23},            true, std::numeric_limits<int8_t>::min(),   std::numeric_limits<int8_t>::max());
    testProperty<int16_t> ((*stringResult), "int16Property",  1, 0, {-4232},         true, std::numeric_limits<int16_t>::min(),  std::numeric_limits<int16_t>::max());
    testProperty<int32_t> ((*stringResult), "int32Property",  1, 0, {2323333},      true, std::numeric_limits<int32_t>::min(),  std::numeric_limits<int32_t>::max());
    testProperty<int64_t> ((*stringResult), "int64Property",  1, 0, {-23554545342},  true, std::numeric_limits<int64_t>::min(),  std::numeric_limits<int64_t>::max());
    testProperty<uint8_t> ((*stringResult), "uint8Property",  2, 0, {45, 123},       true, std::numeric_limits<uint8_t>::min(),  std::numeric_limits<uint8_t>::max());
    testProperty<uint16_t>((*stringResult), "uint16Property", 1, 0, {0},             true, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
    testProperty<uint32_t>((*stringResult), "uint32Property", 1, 0, {12351611},      true, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
    testProperty<uint64_t>((*stringResult), "uint64Property", 1, 0, {6646777643353}, true, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
    testProperty<float>   ((*stringResult), "floatProperty",  1, 0, {9.9999999f},    true, std::numeric_limits<float>::min(),    std::numeric_limits<float>::max());
    testProperty<double>  ((*stringResult), "doubleProperty", 1, 0, {M_PI_2},        true, std::numeric_limits<double>::min(),   std::numeric_limits<double>::max());

    testProperty<std::string> ((*stringResult), "stringProperty",  1, "", {"Tralala"},                true,  "", "");
    testProperty<std::wstring>((*stringResult), "wstringProperty", 1, L"", {L"This is ridiculous..."}, true, L"", L"");

    const CMetaProperty<CStdSharedPtr_t<CMetaObject>>* objectProperty = nullptr;
    SR_FETCH_PROPERTY((*stringResult), CStdSharedPtr_t<CMetaObject>, object);
    testSerializedObject(*(objectProperty->getValue()));

    // ----------------------------------------------------------------------
    // SERIALIZE AGAIN AND CHECK AGAINST REFERENCE DATA
    // ----------------------------------------------------------------------

    serializer.initialize();
    serializer.serialize(*stringResult, result);

    const bool reserializationSuccessfulFromString = result->asString(stringified);

    writeFile("deserialization.dynamic.output.data.txt", stringified);
    ASSERT_EQ(stringified, serializationRefStringDynamic);

    serializer.deinitialize();

    SR_UNUSED(reserializationSuccessfulFromString);
}

TEST_F(Fixture_PropertySystemSerialization, DemoFile)
{
    std::cerr << "Reading demo file.\n";

    std::string demoData{};
    const bool fileRead = readFile("demo_machine_file.ref.txt", demoData);

    std::cerr << "Read demo file\n";

    ASSERT_EQ(true, fileRead);

    const bool dynamicDeserializationMode = true;
    CPropertyDeserializer deserializer = { dynamicDeserializationMode };

    CStdSharedPtr_t<IDeserializer<CMetaObject>::IResult> deserializationResult = nullptr;

    const bool deserializerInitialized = deserializer.initialize();
    ASSERT_EQ(true, deserializerInitialized);

    std::cerr << "Initialized deserializer\n";

    const bool deserialized = deserializer.deserialize(demoData, deserializationResult);
    ASSERT_EQ(true, deserialized);

    std::cerr << "Deserialized data\n";

    const bool deserializerDeinitialized = deserializer.deinitialize();
    ASSERT_EQ(true, deserializerDeinitialized);

    std::cerr << "Deinitialized deserializer\n";

    CStdSharedPtr_t<CMetaObject> deserializedDemoData = nullptr;
    const bool hasStringValue = deserializationResult->asT(deserializedDemoData);
    ASSERT_EQ(true, hasStringValue);

    std::cerr << "Fetched string result.\n";

    std::cerr << deserializedDemoData << "\n";

    CStdSharedPtr_t<CPropertyDeserializer::CDeserializationResult> specificResultType = nullptr;
    specificResultType = std::static_pointer_cast<CPropertyDeserializer::CDeserializationResult>(deserializationResult);
    ASSERT_NE(nullptr, specificResultType);

    CPropertyDeserializer::PathMap_t pathMap{};
    const bool pathMapFetched = specificResultType->getPropertyPathMap(pathMap);
    ASSERT_EQ(true, pathMapFetched);

    std::cerr << "Fetched path map.\n";

    for(const auto& [k, v] : pathMap)
    {
        std::cerr << "Map Key: " << k << "\n";
    }

    MetaPropertyVariant_t &regionOfInterestOffsetX = pathMap.at("/Imagers/0/RegionOfInterest/0/Offset_X")->second;
    MetaPropertyVariant_t &regionOfInterestOffsetY = pathMap.at("/Imagers/0/RegionOfInterest/0/Offset_Y")->second;
    MetaPropertyVariant_t &regionOfInterestWidth   = pathMap.at("/Imagers/0/RegionOfInterest/0/Width")->second;
    MetaPropertyVariant_t &regionOfInterestHeight  = pathMap.at("/Imagers/0/RegionOfInterest/0/Height")->second;

    std::cerr << "Fetched path map entries.\n";

    uint32_t aPropertyValueIndex = 0;

    uint32_t const index = 0;
    uint32_t       value = 0;
    uint32_t       readValue = 0;

    static constexpr uint64_t const gIterations = 1000000;

    std::random_device randomDevice{};
    std::mt19937       merseinneTwister(randomDevice());
    std::uniform_int_distribution<uint32_t> distribution(0, 1920);

    // This test also tests performance...

    for(uint32_t k=0; k<gIterations; ++k)
    {
        const uint32_t refValue0 = distribution(merseinneTwister);
        const uint32_t refValue1 = (1920 - refValue0);

        value = refValue0;

        std::get<CMetaProperty<uint32_t>>(regionOfInterestOffsetX).setValue(value, 0);
        value = refValue1;
        std::get<CMetaProperty<uint32_t>>(regionOfInterestWidth).setValue(value, 0);

        readValue = std::get<CMetaProperty<uint32_t>>(regionOfInterestOffsetX).getValue();
        ASSERT_EQ(refValue0, readValue);
        readValue = std::get<CMetaProperty<uint32_t>>(regionOfInterestWidth).getValue();
        ASSERT_EQ(refValue1, readValue);


        // Refetch the properties from the table and check the value changes so that they are applied to the
        // correct property references...

        regionOfInterestOffsetX = pathMap.at("/Imagers/0/RegionOfInterest/0/Offset_X")->second;
        regionOfInterestWidth   = pathMap.at("/Imagers/0/RegionOfInterest/0/Width")->second;

        readValue = std::get<CMetaProperty<uint32_t>>(regionOfInterestOffsetX).getValue();
        ASSERT_EQ(refValue0, readValue);
        readValue = std::get<CMetaProperty<uint32_t>>(regionOfInterestWidth).getValue();
        ASSERT_EQ(refValue1, readValue);
    }
}

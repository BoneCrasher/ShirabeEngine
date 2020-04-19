#include "sr_pch.h"

#include "sr_test_common.h"

/*!
 * The MetaPrototype fixture is the backbone of prototype related tests.
 */
class Fixture_MetaPrototype
    : public testing::Test
{
public_constructors:
    SR_INLINE Fixture_MetaPrototype()
        : mPrototype(nullptr)
    { }

public_methods:
    SR_INLINE void SetUp()
    {
        ::testing::Test::SetUp();

        mPrototype = CMetaSystem::get()->getPrototype<CSimpleTestPrototype>("CTestPrototype");
    }

    SR_INLINE void TearDown()
    {
        ::testing::Test::TearDown();
    }

public_members:
    std::shared_ptr<CSimpleTestPrototype> mPrototype;
};

/*!
 * Test case used to verify prototype setup on startup.
 */
TEST_F(Fixture_MetaPrototype, Basic_VerifyPrototypeSetup)
{
    ASSERT_NE(mPrototype, nullptr);

    testAgainstSimpleTestPrototype(*mPrototype);
}

/*!
 * Test case used to verify a successful instance creation.
 */
TEST_F(Fixture_MetaPrototype, Basic_CreatePrototypeInstance)
{
    ASSERT_NE(nullptr, mPrototype);

    const CStdSharedPtr_t<CSimpleTest> identicalInstance = mPrototype->createTypedInstance(1337, "TestInstanceOfPrototype");
    ASSERT_NE(nullptr, identicalInstance);

    testAgainstSimpleTestPrototype(*mPrototype);
    testAgainstSimpleTestPrototype(*identicalInstance);
}

/*!
 * Test case used to verify, that the prototype's structure can be altered after initial creation.
 */
TEST_F(Fixture_MetaPrototype, Basic_ChangePrototypeStructure)
{
    ASSERT_NE(nullptr, mPrototype);

    EPropertySystemError status = EPropertySystemError::Ok;

    MetaPropertyMap_t properties{};
    properties["IntegralTest"] = CMetaProperty<int32_t>(12356, "IntegralTest", 0);

    CMetaProperty<int32_t>& property = std::get<CMetaProperty<int32_t>>(properties["IntegralTest"]);
    property
        .setCapacity(2)
        .setRange(250, 1000)
        .setDefaultValue(500)
        .setWriteable(true)
        .setValue(1000);

    // Create instance with identical values of prototype, but with an "override" set of properties.
    CStdSharedPtr_t<CSimpleTest> identicalInstance = mPrototype->createTypedInstance(1337, "TestInstanceOfPrototype", properties);
    ASSERT_NE(nullptr, identicalInstance);

    // Verify successful construction of the prototype using it's callback.
    const bool hasInt32Property = identicalInstance->hasProperty("IntegralTest");
    ASSERT_EQ(hasInt32Property, true);

    const CMetaProperty<int32_t>* int32Property = nullptr;
    status = identicalInstance->getProperty<int32_t>("IntegralTest", &int32Property);
    ASSERT_EQ(EPropertySystemError::Ok, status);
    ASSERT_NE(nullptr, int32Property);

    const int32_t int32PropertyAccessorValue = identicalInstance->at<int32_t>("IntegralTest").getValue();
    ASSERT_EQ(int32PropertyAccessorValue, 1000);

    const uint32_t& int32PropertyCapacity     = int32Property->getCapacity();
    const int32_t&  int32PropertyDefaultValue = int32Property->getDefaultValue();
    const int32_t&  int32PropertyValue0       = int32Property->getValue();
    const int32_t&  int32PropertyValue1To9    = int32Property->getValue(1);
    const bool      int32PropertyIsWritable   = int32Property->isWritable();
    const int32_t&  int32PropertyMinValue     = int32Property->getMinValue();
    const int32_t&  int32PropertyMaxValue     = int32Property->getMaxValue();

    ASSERT_EQ(int32PropertyCapacity,     2);
    ASSERT_EQ(int32PropertyDefaultValue, 500);
    // The next two lines also test a capacity resize without overwriting old values as
    // well as clamping on range change.
    ASSERT_EQ(int32PropertyValue0,     1000);
    ASSERT_EQ(int32PropertyValue1To9,  250);
    ASSERT_EQ(int32PropertyIsWritable, true);
    ASSERT_EQ(int32PropertyMinValue,   250);
    ASSERT_EQ(int32PropertyMaxValue,   1000);
}

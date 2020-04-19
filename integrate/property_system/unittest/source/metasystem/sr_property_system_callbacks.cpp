#include "sr_pch.h"

#include <fstream>
#include <random>

#include "sr_test_common.h"

/*!
 *
 */
class Fixture_SystemPropertyCallbacks
        : public ::testing::Test
{
public_methods:
    void SetUp()
    {
        ::testing::Test::SetUp();

    }

    void TearDown()
    {
        ::testing::Test::TearDown();
    }

protected_members:
};


/*!
 * Test case to verify that a newly created property is properly registered in the system
 * and that the required replication callback is properly invoked.
 */
TEST_F(Fixture_SystemPropertyCallbacks, TestSingleProperty)
{
    CStdSharedPtr_t<CMetaSystem> system = CMetaSystem::get();

    CMetaProperty<int8_t> randomProperty{};
    randomProperty
            .setCapacity(2)
            .setWriteable(true)
            .setDefaultValue(0)
            .setReplicationMode(CMetaPropertyCore::EPropertyReplicationMode::Replicate);

    MetaPropertyMap_t properties{};
    properties["randomProperty"] = std::move(randomProperty);

    const InstanceUID_t instanceUID = system->generateObjectUID();
    CMetaObject* obj = CMetaObject::create(nullptr, instanceUID, "SystemPropertyCallbackTest", properties);
    ASSERT_NE(nullptr, obj);


    const CMetaProperty<int8_t>* property = nullptr;
    obj->getProperty<int8_t>("randomProperty", &property);

    bool callbackInvoked = false;

    auto callback = [&] (const CMetaProperty<int8_t>& aProperty, const int8_t& aValue, const uint32_t& aValueIndex)
    {
        ASSERT_EQ(1, aValue);
        ASSERT_EQ(0, aValueIndex);
        ASSERT_EQ(&(*property), &aProperty);

        callbackInvoked = true;
    };

    bool const registered = system->registerReplicationCallback(*property, callback);
    ASSERT_EQ(true, registered);

    obj->at<int8_t>("randomProperty").setValue(1, 0);
    ASSERT_EQ(true, callbackInvoked);

    SR_SAFE_DELETE(obj);
}

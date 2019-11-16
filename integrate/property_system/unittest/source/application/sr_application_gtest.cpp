/*!
 * @file      sr_application_gtest.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"
#include "sr_application_gtest.h"


//<---------------------------------------------
CApplicationGTest::CApplicationGTest(const std::queue<std::string>& aExecutionOrderFilters)
    : CApplicationBase()
    , mExecutionOrderFilters(aExecutionOrderFilters)
{
}

//<---------------------------------------------
bool CApplicationGTest::onIdle()
{
    int32_t testResult = 0;
    while(!mExecutionOrderFilters.empty())
    {
        const std::string currentFilter = mExecutionOrderFilters.front();
        mExecutionOrderFilters.pop();

        ::testing::GTEST_FLAG(filter) = currentFilter;
        testResult = RUN_ALL_TESTS();
    }

    setExitCode( testResult );

    const bool canContinue = false;

    return canContinue;
}

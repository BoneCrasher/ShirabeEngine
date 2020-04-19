/*!
 * @file      sr_application_gtest.h
 * @author    andras.acsai
 * @date      2018/06/15
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#pragma once

#include <queue>
#include <string>
#include <core/base/defines/sr_defines.h>
#include <core/application/sr_application_base.h>


class CApplicationGTest
        : public CApplicationBase
{
public_constructors:
    /*!
     * @brief                        Constructor
     * @param aExecutionOrderFilters A queue filled with filter patterns to control test execution order.
     */
    CApplicationGTest(const std::queue<std::string>& aExecutionOrderFilters);

    /*!
     * Called when there was no message in the queue to process
     * 
     * @return bool     True if the application can proceed, otherwise false
     */
    virtual bool onIdle();

private_members:
    std::queue<std::string> mExecutionOrderFilters;
};


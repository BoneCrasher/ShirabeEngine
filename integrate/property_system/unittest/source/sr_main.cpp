/*!
 * @file      sr_main.cpp
 * @author    Marc-Anton Boehm-von Thenen
 * @date      27/06/2018
 * @copyright SmartRay GmbH (www.smartray.com)
 */

#include "sr_pch.h"

#define SR_APPLICATION_MAIN

#include <core/kernel/sr_kernel.h>

#include "application/sr_application_gtest.h"

//<---------------------------------------------
CApplicationBase* sr_main( CPlatform* const aPlatform, int aArgC, char* aArgV[] )
{
    SR_UNUSED(aPlatform);

    //!< andras.acsai [18/06/15]: gtest is "leaking": https://stackoverflow.com/questions/12704543/memory-leak-when-using-google-test-on-windows
    testing::InitGoogleTest( &aArgC, aArgV );

    std::queue<std::string> executionOrderFilters{};
    executionOrderFilters.push("Fixture_MetaObject.Basic_*");
    executionOrderFilters.push("Fixture_MetaPrototype.Basic_*");
    executionOrderFilters.push("Fixture_MetaObject.Complex_*");  // Run them last, since they depend on the prototype basic functionality to work.
    executionOrderFilters.push("Fixture_PropertySystemSerialization.*");
    executionOrderFilters.push("Fixture_SystemPropertyCallbacks.*");

    CApplicationGTest* const application = new CApplicationGTest(executionOrderFilters);

    return application;
}

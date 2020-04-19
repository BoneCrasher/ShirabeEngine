# include($ENV{SR_BUILDSYSTEM_ENV_FILEPATH}/integration/onecode.environment.cmake)
include(project_default_template)

set(SR_TEMPLATE "Application")

set(SR_PROJECT_ID_OVERRIDE "property_system_unittest")

set(
    SR_PROJECT_REQUESTED_LINK_TARGETS
        LINK_PROPERTY_SYSTEM)

if(WIN32)
    append(
        SR_PROJECT_REQUESTED_LINK_TARGETS
            LINK_CORE_WINDOWS)
else()
    append(
        SR_PROJECT_REQUESTED_LINK_TARGETS
            LINK_CORE_LINUX)
endif()

append(
    SR_PROJECT_REQUESTED_LINK_TARGETS
        LINK_CORE_BASE
        LINK_NLOHMANNJSON
        LINK_GTEST)

include(project_setup)

add_dependencies(${SR_MODULE_NAME}_GeneratePCH INSTALL_property_system)
add_dependencies(${SR_MODULE_NAME}_GeneratePCH thirdparty_googletest)
add_dependencies(${SR_MODULE_NAME}             ${SR_MODULE_NAME}_GeneratePCH)

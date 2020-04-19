# include($ENV{SR_BUILDSYSTEM_ENV_FILEPATH}/integration/onecode.environment.cmake)
include(project_default_template)

set(SR_TEMPLATE "StaticLib")

set(SR_PROJECT_REQUESTED_LINK_TARGETS)
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
        LINK_NLOHMANNJSON)

include(project_setup)

if(WIN32)
    add_dependencies(property_system INSTALL_core_windows)
else()
    add_dependencies(property_system_GeneratePCH INSTALL_core_linux)
    add_dependencies(property_system             property_system_GeneratePCH)
endif()

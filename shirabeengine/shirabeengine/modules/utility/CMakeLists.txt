cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)


set(SHIRABE_TEMPLATE "StaticLib")

# set(SHIRABE_EXCLUDED_HEADERS util/documents/json.h)
set(SHIRABE_EXCLUDED_SOURCES documents/json.cpp)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULECORE
        CRYPTOPP)

include(project_setup)

add_dependencies(utility INSTALL_core)
add_dependencies(utility INSTALL_base)
add_dependencies(utility INSTALL_math)
add_dependencies(utility INSTALL_log)
add_dependencies(utility INSTALL_platform)

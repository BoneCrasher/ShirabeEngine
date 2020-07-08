cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "StaticLib")
include(project_default_template)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULELOG)

include(project_setup)

add_dependencies(math INSTALL_platform)
add_dependencies(math INSTALL_base)
add_dependencies(math INSTALL_log)

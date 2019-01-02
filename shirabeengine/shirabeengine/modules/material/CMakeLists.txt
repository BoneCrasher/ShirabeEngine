cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULELOG
        SHIRABEMODULEPLATFORM
        SHIRABEMODULEBASE)

include(project_setup)

add_dependencies(math INSTALL_platform)
add_dependencies(math INSTALL_base)
add_dependencies(math INSTALL_log)

cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "Application")
include(project_default_template)

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEUTILITY)

include(project_setup)

add_dependencies(crc32generator INSTALL_utility)
add_dependencies(crc32generator INSTALL_platform)
add_dependencies(crc32generator INSTALL_base)
add_dependencies(crc32generator INSTALL_log)
add_dependencies(crc32generator INSTALL_math)
add_dependencies(crc32generator INSTALL_core)

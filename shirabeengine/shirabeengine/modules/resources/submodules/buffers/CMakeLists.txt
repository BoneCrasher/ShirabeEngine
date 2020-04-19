cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULERESOURCES)

include(project_setup)

add_dependencies(buffers INSTALL_platform)
add_dependencies(buffers INSTALL_base)
add_dependencies(buffers INSTALL_log)
add_dependencies(buffers INSTALL_core)
add_dependencies(buffers INSTALL_utility)
add_dependencies(buffers INSTALL_graphicsapi)
add_dependencies(buffers INSTALL_resources)
add_dependencies(buffers INSTALL_math)
add_dependencies(buffers INSTALL_asset)

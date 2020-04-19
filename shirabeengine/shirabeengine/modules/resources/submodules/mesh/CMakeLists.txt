cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULERESOURCES
        FXGLTF)

include(project_setup)

add_dependencies(mesh INSTALL_platform)
add_dependencies(mesh INSTALL_base)
add_dependencies(mesh INSTALL_log)
add_dependencies(mesh INSTALL_core)
add_dependencies(mesh INSTALL_utility)
add_dependencies(mesh INSTALL_graphicsapi)
add_dependencies(mesh INSTALL_resources)
add_dependencies(mesh INSTALL_math)
add_dependencies(mesh INSTALL_asset)

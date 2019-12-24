cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULERESOURCES)

include(project_setup)

add_dependencies(textures INSTALL_platform)
add_dependencies(textures INSTALL_base)
add_dependencies(textures INSTALL_log)
add_dependencies(textures INSTALL_core)
add_dependencies(textures INSTALL_utility)
add_dependencies(textures INSTALL_graphicsapi)
add_dependencies(textures INSTALL_resources)
add_dependencies(textures INSTALL_math)
add_dependencies(textures INSTALL_asset)

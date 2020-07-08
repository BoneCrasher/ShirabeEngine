cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "StaticLib")
include(project_default_template)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEASSET
        SHIRABEMODULEGRAPHICSAPI
        SHIRABEMODULEWSI)

set(SHIRABE_EXCLUDED_HEADERS)
set(SHIRABE_EXCLUDED_SOURCES)

include(project_setup)

add_dependencies(rendering INSTALL_asset)
add_dependencies(rendering INSTALL_graphicsapi)
add_dependencies(rendering INSTALL_wsi)
add_dependencies(rendering INSTALL_core)
add_dependencies(rendering INSTALL_os)
add_dependencies(rendering INSTALL_math)
add_dependencies(rendering INSTALL_log)
add_dependencies(rendering INSTALL_base)
add_dependencies(rendering INSTALL_platform)

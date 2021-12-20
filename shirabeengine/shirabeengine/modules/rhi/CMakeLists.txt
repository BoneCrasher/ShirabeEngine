cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "StaticLib")
include(project_default_template)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEASSET
        SHIRABEMODULEWSI)

set(SHIRABE_EXCLUDED_HEADERS)
set(SHIRABE_EXCLUDED_SOURCES)

include(project_setup)

add_dependencies(rhi INSTALL_asset)
add_dependencies(rhi INSTALL_wsi)
add_dependencies(rhi INSTALL_core)
add_dependencies(rhi INSTALL_os)
add_dependencies(rhi INSTALL_math)
add_dependencies(rhi INSTALL_log)
add_dependencies(rhi INSTALL_base)
add_dependencies(rhi INSTALL_platform)

cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(SHIRABE_EXCLUDED_HEADERS)
set(SHIRABE_EXCLUDED_SOURCES)

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULERENDERING
        VULKANSDK)

include(project_setup)

add_dependencies(vulkan_integration INSTALL_platform)
add_dependencies(vulkan_integration INSTALL_base)
add_dependencies(vulkan_integration INSTALL_log)
add_dependencies(vulkan_integration INSTALL_core)
add_dependencies(vulkan_integration INSTALL_os)
add_dependencies(vulkan_integration INSTALL_math)
add_dependencies(vulkan_integration INSTALL_resources)
add_dependencies(vulkan_integration INSTALL_rendering)
add_dependencies(vulkan_integration INSTALL_graphicsapi)
add_dependencies(vulkan_integration INSTALL_wsi)
add_dependencies(vulkan_integration INSTALL_asset)
add_dependencies(vulkan_integration INSTALL_material)

cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEGRAPHICSAPI
        SHIRABEMODULERESOURCES  
        SHIRABEMODULEOS         
        SHIRABEMODULECORE       
        SHIRABEMODULEMATH       
        SHIRABEMODULELOG        
        SHIRABEMODULEPLATFORM
        VULKANSDK
        JSON
        XML2)

include(project_setup)

add_dependencies(vulkan_integration INSTALL_log)
add_dependencies(vulkan_integration INSTALL_platform)
add_dependencies(vulkan_integration INSTALL_core)
add_dependencies(vulkan_integration INSTALL_os)
add_dependencies(vulkan_integration INSTALL_math)
add_dependencies(vulkan_integration INSTALL_resources)
add_dependencies(vulkan_integration INSTALL_graphicsapi)

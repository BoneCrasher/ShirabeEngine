cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEUTILITY    
        SHIRABEMODULERESOURCES  
        SHIRABEMODULEGRAPHICSAPI
        SHIRABEMODULECORE       
        SHIRABEMODULEMATH       
        SHIRABEMODULELOG        
        SHIRABEMODULEBASE
        SHIRABEMODULEPLATFORM
        VULKANHEADERS
        STBI                    
        FXGLTF                  
        JSON
        XML2
        ZLIB)

include(project_setup)

add_dependencies(asset INSTALL_platform)
add_dependencies(asset INSTALL_base)
add_dependencies(asset INSTALL_core)
add_dependencies(asset INSTALL_utility)
add_dependencies(asset INSTALL_graphicsapi)
add_dependencies(asset INSTALL_resources)
add_dependencies(asset INSTALL_math)
add_dependencies(asset INSTALL_log)

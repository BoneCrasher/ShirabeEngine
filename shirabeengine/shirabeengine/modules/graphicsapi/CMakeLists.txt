cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULERESOURCES
        SHIRABEMODULECORE     
        SHIRABEMODULEMATH     
        SHIRABEMODULELOG      
        SHIRABEMODULEBASE
        SHIRABEMODULEPLATFORM 
        JSON                  
        XML2)

include(project_setup)

add_dependencies(graphicsapi INSTALL_resources)
add_dependencies(graphicsapi INSTALL_base)
add_dependencies(graphicsapi INSTALL_platform)
add_dependencies(graphicsapi INSTALL_math)
add_dependencies(graphicsapi INSTALL_log)
add_dependencies(graphicsapi INSTALL_core)

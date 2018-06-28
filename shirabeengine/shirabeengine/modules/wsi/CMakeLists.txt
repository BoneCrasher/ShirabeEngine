cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEOS         
        SHIRABEMODULECORE       
        SHIRABEMODULEMATH       
        SHIRABEMODULELOG        
        SHIRABEMODULEPLATFORM
        JSON
        XML2)

include(project_setup)

add_dependencies(wsi INSTALL_platform)
add_dependencies(wsi INSTALL_os)
add_dependencies(wsi INSTALL_math)
add_dependencies(wsi INSTALL_log)
add_dependencies(wsi INSTALL_core)

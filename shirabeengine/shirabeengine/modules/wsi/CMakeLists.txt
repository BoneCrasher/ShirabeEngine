cmake_minimum_required(VERSION 3.10)

include($ENV{SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)
include(project_default_template)

set(SHIRABE_TEMPLATE "StaticLib")

if(MSVC)
elseif(UNIX AND NOT APPLE)
    set(SHIRABE_EXCLUDED_SOURCES
            windows/windowswindow.cpp
            windows/windowswindowfactory.cpp)
endif()

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEOS         
        SHIRABEMODULECORE       
        SHIRABEMODULEMATH       
        SHIRABEMODULELOG        
        SHIRABEMODULEBASE
        SHIRABEMODULEPLATFORM
        VULKANHEADERS
        XLIB
        JSON
        XML2
        ZLIB)

include(project_setup)

add_dependencies(wsi INSTALL_core)
add_dependencies(wsi INSTALL_os)
add_dependencies(wsi INSTALL_math)
add_dependencies(wsi INSTALL_log)
add_dependencies(wsi INSTALL_base)
add_dependencies(wsi INSTALL_platform)

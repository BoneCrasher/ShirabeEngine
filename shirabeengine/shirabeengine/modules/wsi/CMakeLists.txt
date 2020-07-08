cmake_minimum_required(VERSION 3.10)

include(${SHIRABE_BUILDSYSTEM_ENV_FILEPATH}/integration/shirabeengine.environment.cmake)

set(SHIRABE_TEMPLATE "StaticLib")
include(project_default_template)

if(MSVC)
elseif(UNIX AND NOT APPLE)
    set(SHIRABE_EXCLUDED_SOURCES
            windows/windowswindow.cpp
            windows/windowswindowfactory.cpp)
endif()

set(
    SHIRABE_PROJECT_REQUESTED_LINK_TARGETS
        SHIRABEMODULEOS
        XLIB)

include(project_setup)

add_dependencies(wsi INSTALL_core)
add_dependencies(wsi INSTALL_os)
add_dependencies(wsi INSTALL_math)
add_dependencies(wsi INSTALL_log)
add_dependencies(wsi INSTALL_base)
add_dependencies(wsi INSTALL_platform)

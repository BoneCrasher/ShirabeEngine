cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(LINK_JSON                  TRUE)
set(LINK_XML2                  TRUE)
set(LINK_SHIRABEMODULEMATH     TRUE)
set(LINK_SHIRABEMODULELOG      TRUE)
set(LINK_SHIRABEMODULECORE     TRUE)
set(LINK_SHIRABEMODULEPLATFORM TRUE)

include(GenericProject)

add_dependencies(Resources INSTALL_Math)
add_dependencies(Resources INSTALL_Log)
add_dependencies(Resources INSTALL_Core)
add_dependencies(Resources INSTALL_Platform)
cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(LINK_SHIRABEMODULECORE      TRUE)
set(LINK_SHIRABEMODULEMATH      TRUE)
set(LINK_SHIRABEMODULEPLATFORM  TRUE)

include(GenericProject)

add_dependencies(OS INSTALL_Platform)
add_dependencies(OS INSTALL_Core)
add_dependencies(OS INSTALL_Math)
cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(LINK_SHIRABEMODULEPLATFORM    TRUE)
set(LINK_SHIRABEMODULEMATH        TRUE)
set(LINK_SHIRABEMODULEOS          TRUE)
set(LINK_SHIRABEMODULECORE        TRUE)
set(LINK_SHIRABEMODULELOG         TRUE)

include(GenericProject)

add_dependencies(WSI INSTALL_Platform)
add_dependencies(WSI INSTALL_OS)
add_dependencies(WSI INSTALL_Math)
add_dependencies(WSI INSTALL_Log)
add_dependencies(WSI INSTALL_Core)
cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(LINK_SHIRABEMODULEGRAPHICSAPI TRUE)
set(LINK_SHIRABEMODULERESOURCES   TRUE)
set(LINK_SHIRABEMODULECORE        TRUE)
set(LINK_SHIRABEMODULEMATH        TRUE)
set(LINK_SHIRABEMODULELOG         TRUE)
set(LINK_SHIRABEMODULEPLATFORM    TRUE)

include(GenericProject)

add_dependencies(Rendering INSTALL_Math)
add_dependencies(Rendering INSTALL_Log)
add_dependencies(Rendering INSTALL_Core)
add_dependencies(Rendering INSTALL_Platform)
add_dependencies(Rendering INSTALL_Resources)
add_dependencies(Rendering INSTALL_GraphicsAPI)
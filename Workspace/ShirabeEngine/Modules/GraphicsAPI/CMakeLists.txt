cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "Module")

set(LINK_SHIRABEMODULERESOURCES TRUE)
set(LINK_SHIRABEMODULECORE      TRUE)
set(LINK_SHIRABEMODULELOG       TRUE)
set(LINK_SHIRABEMODULEPLATFORM  TRUE)
set(LINK_SHIRABEMODULEMATH      TRUE)
set(LINK_JSON                   TRUE)
set(LINK_XML2                   TRUE)

include(GenericProject)

add_dependencies(GraphicsAPI INSTALL_Resources)
add_dependencies(GraphicsAPI INSTALL_Platform)
add_dependencies(GraphicsAPI INSTALL_Math)
add_dependencies(GraphicsAPI INSTALL_Log)
add_dependencies(GraphicsAPI INSTALL_Core)
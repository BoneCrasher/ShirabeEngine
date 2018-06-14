cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(LINK_SHIRABEMODULECORE     TRUE)
set(LINK_SHIRABEMODULEPLATFORM TRUE)
set(LINK_JSON                  TRUE)
set(LINK_XML2                  TRUE)

include(GenericProject)

add_dependencies(Utility INSTALL_Platform)
add_dependencies(Utility INSTALL_Core)
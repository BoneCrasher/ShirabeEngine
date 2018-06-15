cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "Module")

set(LINK_SHIRABEMODULELOG      TRUE)
set(LINK_SHIRABEMODULEPLATFORM TRUE)
set(LINK_JSON                  TRUE)
set(LINK_XML2                  TRUE)

include(GenericProject)

add_dependencies(Math INSTALL_Platform)
add_dependencies(Math INSTALL_Log)
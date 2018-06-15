cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "Module")

set(LINK_SHIRABEMODULECORE     TRUE)
set(LINK_SHIRABEMODULEPLATFORM TRUE)

include(GenericProject)

add_dependencies(Log INSTALL_Platform)
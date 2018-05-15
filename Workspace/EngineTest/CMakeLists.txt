cmake_minimum_required(VERSION 3.9)

include(../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "Application")

set(SHIRABE_COMPILER__SUBSYSTEM CONSOLE)

set(LINK_SHIRABEENGINE TRUE)
set(LINK_FXGLTF        TRUE)
set(LINK_JSON          TRUE)
set(LINK_XML2          TRUE)

include(GenericProject)

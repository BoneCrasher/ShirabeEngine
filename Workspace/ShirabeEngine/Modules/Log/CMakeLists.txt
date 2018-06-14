cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "HeaderOnly")

set(LINK_JSON                  TRUE)
set(LINK_XML2                  TRUE)
set(LINK_SHIRABEMODULEPLATFORM TRUE)

include(GenericProject)

add_dependencies(Log INSTALL_Platform)
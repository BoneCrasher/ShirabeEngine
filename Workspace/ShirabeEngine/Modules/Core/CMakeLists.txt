cmake_minimum_required(VERSION 3.9)

include(../../../CMakeEnvironment.generated.cmake)
include(DefaultProject.template)

set(SHIRABE_TEMPLATE "StaticLib")

set(SHIRABE_EXCLUDED_SOURCES 
	Serialization/JSONObjectSerializer.cpp 
	Serialization/XMLObjectSerializer.cpp)

set(LINK_JSON                  TRUE)
set(LINK_XML2                  TRUE)
set(LINK_SHIRABEMODULEMATH     TRUE)
set(LINK_SHIRABEMODULELOG      TRUE)
set(LINK_SHIRABEMODULEPLATFORM TRUE)

include(GenericProject)

add_dependencies(Core INSTALL_Math)
add_dependencies(Core INSTALL_Log)
add_dependencies(Core INSTALL_Platform)